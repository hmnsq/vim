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
#شامل "vim.h"

#اگر defined(FEAT_CRYPT)
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

تعریفنوع ساختار {
    نویسه	*name;		// encryption name as used in 'cryptmethod'
    نویسه	*magic;		// magic bytes stored in file header
    صحیح		salt_len;	// length of salt, or 0 when not using salt
    صحیح		seed_len;	// length of seed, or 0 when not using seed
    صحیح		add_len;	// additional length in the header needed for storing
				// custom data
#ifdef CRYPT_NOT_INPLACE
    صحیح		works_inplace;	// encryption/decryption can be done in-place
#endif
    صحیح		whole_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    صحیح (*self_test_fn)(تهی);

    // Function pointer for initializing encryption/decryption.
    صحیح (* init_fn)(cryptstate_T *state, char_u *کلید, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    تهی (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, صحیح last);
    تهی (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, صحیح last);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    long (*encode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, صحیح last);
    long (*decode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, صحیح last);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    تهی (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, صحیح last);
    تهی (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, صحیح last);
} cryptmethod_T;

ایستا صحیح crypt_sodium_init_(cryptstate_T *state, char_u *کلید, crypt_arg_T *arg);
ایستا long crypt_sodium_buffer_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, صحیح last);
ایستا long crypt_sodium_buffer_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, صحیح last);
#اگر defined(FEAT_SODIUM)
ایستا تهی crypt_long_long_to_char(long long n, char_u *s);
ایستا تهی crypt_int_to_char(صحیح n, char_u *s);
ایستا long long crypt_char_to_long_long(char_u *s);
ایستا صحیح crypt_char_to_int(char_u *s);
#endif
#اگر defined(FEAT_EVAL) && defined(FEAT_SODIUM)
ایستا تهی crypt_sodium_report_hash_params(unsigned long long opslimit, unsigned long long ops_def, size_t memlimit, size_t mem_def, صحیح alg, صحیح alg_def);
#endif

// index is method_nr of cryptstate_T, CRYPT_M_*
ایستا cryptmethod_T cryptmethods[CRYPT_M_شمار] = {
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
	پوچ,
	crypt_zip_init,
	crypt_zip_encode, crypt_zip_decode,
	پوچ, پوچ,
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
	پوچ, پوچ,
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
	پوچ, پوچ,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // XChaCha20 using libsodium; implementation issues
    {
	"xchacha20",
	"VimCrypt~04!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#وگرنه
	16,
#endif
	8,
	0,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	پوچ,
	crypt_sodium_init_,
	پوچ, پوچ,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	پوچ, پوچ,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#وگرنه
	16,
#endif
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	پوچ,
	crypt_sodium_init_,
	پوچ, پوچ,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	پوچ, پوچ,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#اگر defined(FEAT_SODIUM)
تعریفنوع ساختار {
    size_t	    شمار;
    unsigned نویسه   کلید[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define load_dll vimLoadLib
#   define نماد_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  وگرنه
#   error Dynamic loading of libsodium is not supported برای now.
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
#  define crypto_secretstream_xchacha20poly1305_init_افزودن \
    dll_crypto_secretstream_xchacha20poly1305_init_افزودن
#  define crypto_secretstream_xchacha20poly1305_افزودن \
    dll_crypto_secretstream_xchacha20poly1305_افزودن
#  define crypto_secretstream_xchacha20poly1305_init_pull \
    dll_crypto_secretstream_xchacha20poly1305_init_pull
#  define crypto_secretstream_xchacha20poly1305_pull \
    dll_crypto_secretstream_xchacha20poly1305_pull
#  define crypto_pwhash	    dll_crypto_pwhash
#  define randombytes_buf   dll_randombytes_buf
#  define randombytes_random dll_randombytes_random

ایستا صحیح (*dll_sodium_init)(تهی) = پوچ;
ایستا تهی (*dll_sodium_free)(تهی *) = پوچ;
ایستا تهی *(*dll_sodium_malloc)(ثابت size_t) = پوچ;
ایستا تهی (*dll_sodium_memzero)(تهی * ثابت, ثابت size_t) = پوچ;
ایستا صحیح (*dll_sodium_mlock)(تهی * ثابت, ثابت size_t) = پوچ;
ایستا صحیح (*dll_sodium_munlock)(تهی * ثابت, ثابت size_t) = پوچ;
ایستا صحیح (*dll_crypto_secretstream_xchacha20poly1305_init_افزودن)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned نویسه [],
    ثابت unsigned نویسه []) = پوچ;
ایستا صحیح (*dll_crypto_secretstream_xchacha20poly1305_افزودن)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned نویسه *c, unsigned long long *clen_p,
    ثابت unsigned نویسه *m, unsigned long long mlen,
    ثابت unsigned نویسه *ad, unsigned long long adlen, unsigned نویسه tag)
    = پوچ;
ایستا صحیح (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ثابت unsigned نویسه [],
    ثابت unsigned نویسه []) = پوچ;
ایستا صحیح (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned نویسه *m, unsigned long long *mlen_p, unsigned نویسه *tag_p,
    ثابت unsigned نویسه *c, unsigned long long clen,
    ثابت unsigned نویسه *ad, unsigned long long adlen) = پوچ;
ایستا صحیح (*dll_crypto_pwhash)(unsigned نویسه * ثابت out,
    unsigned long long outlen,
    ثابت نویسه * ثابت passwd, unsigned long long passwdlen,
    ثابت unsigned نویسه * ثابت salt,
    unsigned long long opslimit, size_t memlimit, صحیح alg)
    = پوچ;
ایستا تهی (*dll_randombytes_buf)(تهی * ثابت buf, ثابت size_t size);
ایستا uint32_t (*dll_randombytes_random)(تهی);

ایستا ساختار {
    ثابت نویسه *name;
    SODIUM_PROC *ptr;
} sodium_funcname_table[] = {
    {"sodium_init", (SODIUM_PROC*)&dll_sodium_init},
    {"sodium_free", (SODIUM_PROC*)&dll_sodium_free},
    {"sodium_malloc", (SODIUM_PROC*)&dll_sodium_malloc},
    {"sodium_memzero", (SODIUM_PROC*)&dll_sodium_memzero},
    {"sodium_mlock", (SODIUM_PROC*)&dll_sodium_mlock},
    {"sodium_munlock", (SODIUM_PROC*)&dll_sodium_munlock},
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_افزودن},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_افزودن},
    {"crypto_secretstream_xchacha20poly1305_init_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_pull},
    {"crypto_secretstream_xchacha20poly1305_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_pull},
    {"crypto_pwhash", (SODIUM_PROC*)&dll_crypto_pwhash},
    {"randombytes_buf", (SODIUM_PROC*)&dll_randombytes_buf},
    {"randombytes_random", (SODIUM_PROC*)&dll_randombytes_random},
    {پوچ, پوچ}
};

    ایستا صحیح
sodium_runtime_link_init(صحیح verbose)
{
    ایستا HINSTANCE hsodium = پوچ;
    ثابت نویسه *libname = DYNAMIC_SODIUM_DLL;
    صحیح i;

    اگر (hsodium != پوچ)
	بازگشت OK;

    hsodium = load_dll(libname);
    اگر (hsodium == پوچ)
    {
	اگر (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	بازگشت FAIL;
    }

    برای (i = 0; sodium_funcname_table[i].ptr; ++i)
    {
	اگر ((*sodium_funcname_table[i].ptr = نماد_from_dll(hsodium,
			sodium_funcname_table[i].name)) == پوچ)
	{
	    close_dll(hsodium);
	    hsodium = پوچ;
	    اگر (verbose)
		semsg(_(e_could_not_load_library_function_str), sodium_funcname_table[i].name);
	    بازگشت FAIL;
	}
    }
    بازگشت OK;
}

    ایستا صحیح
load_sodium(تهی)
{
    اگر (sodium_runtime_link_init(TRUE) == FAIL)
	بازگشت -1;
    بازگشت dll_sodium_init();
}
# endif

# اگر defined(DYNAMIC_SODIUM)
    صحیح
sodium_enabled(صحیح verbose)
{
    بازگشت sodium_runtime_link_init(verbose) == OK;
}
# endif
#endif

#define CRYPT_MAGIC_LEN	12	// cannot change
ایستا ثابت نویسه crypt_magic_head[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    صحیح
crypt_method_nr_from_name(char_u *name)
{
    صحیح i;

    برای (i = 0; i < CRYPT_M_شمار; ++i)
	اگر (STRCMP(name, cryptmethods[i].name) == 0)
	    بازگشت i;
    بازگشت 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    صحیح
crypt_method_nr_from_magic(نویسه *ptr, صحیح len)
{
    صحیح i;

    اگر (len < CRYPT_MAGIC_LEN)
	بازگشت -1;

    برای (i = 0; i < CRYPT_M_شمار; i++)
	اگر (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    بازگشت i;

    i = (صحیح)STRLEN(crypt_magic_head);
    اگر (len >= i && memcmp(ptr, crypt_magic_head, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_method));

    بازگشت -1;
}

#ifdef CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    صحیح
crypt_works_inplace(cryptstate_T *state)
{
    بازگشت cryptmethods[state->method_nr].works_inplace;
}
#endif

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    صحیح
crypt_get_method_nr(buf_T *buf)
{
    بازگشت crypt_method_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    صحیح
crypt_method_is_sodium(صحیح method)
{
    بازگشت method == CRYPT_M_SOD || method == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    صحیح
crypt_whole_undofile(صحیح method_nr)
{
    بازگشت cryptmethods[method_nr].whole_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    صحیح
crypt_get_header_len(صحیح method_nr)
{
    بازگشت CRYPT_MAGIC_LEN
	+ cryptmethods[method_nr].salt_len
	+ cryptmethods[method_nr].seed_len
	+ cryptmethods[method_nr].add_len;
}


#اگر defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    صحیح
crypt_get_max_header_len(تهی)
{
    صحیح i;
    صحیح max = 0;
    صحیح temp = 0;

    برای (i = 0; i < CRYPT_M_شمار; ++i)
    {
	temp = crypt_get_header_len(i);
	اگر (temp > max)
	    max = temp;
    }
    بازگشت max;
}
#endif

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    تهی
crypt_set_cm_option(buf_T *buf, صحیح method_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[method_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    صحیح
crypt_self_test(تهی)
{
    صحیح method_nr = crypt_get_method_nr(curbuf);

    اگر (cryptmethods[method_nr].self_test_fn == پوچ)
	بازگشت OK;
    بازگشت cryptmethods[method_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    صحیح		method_nr,
    char_u	*کلید,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    اگر (state == پوچ)
	بازگشت state;

    state->method_nr = method_nr;
    اگر (cryptmethods[method_nr].init_fn(state, کلید, crypt_arg) == FAIL)
    {
	vim_free(state);
	بازگشت پوچ;
    }
    بازگشت state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_header(
    صحیح		method_nr,
    char_u	*کلید,
    char_u	*header)
{
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len = cryptmethods[method_nr].add_len;
    اگر (arg.cat_salt_len > 0)
	arg.cat_salt = header + CRYPT_MAGIC_LEN;
    اگر (arg.cat_seed_len > 0)
	arg.cat_seed = header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    اگر (arg.cat_add_len > 0)
	arg.cat_add = header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

    بازگشت crypt_create(method_nr, کلید, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *کلید)
{
    صحیح		method_nr;
    صحیح		header_len;
    نویسه	magic_buffer[CRYPT_MAGIC_LEN];
    char_u	*buffer;
    cryptstate_T *state;

    اگر (fread(magic_buffer, CRYPT_MAGIC_LEN, 1, fp) != 1)
	بازگشت پوچ;
    method_nr = crypt_method_nr_from_magic(magic_buffer, CRYPT_MAGIC_LEN);
    اگر (method_nr < 0)
	بازگشت پوچ;

    header_len = crypt_get_header_len(method_nr);
    اگر ((buffer = alloc(header_len)) == پوچ)
	بازگشت پوچ;
    mch_memmove(buffer, magic_buffer, CRYPT_MAGIC_LEN);
    اگر (header_len > CRYPT_MAGIC_LEN
	    && fread(buffer + CRYPT_MAGIC_LEN,
				    header_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(buffer);
	بازگشت پوچ;
    }

    state = crypt_create_from_header(method_nr, کلید, buffer);
    vim_free(buffer);
    بازگشت state;
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
    صحیح	    method_nr,
    char_u  *کلید,
    char_u  **header,
    صحیح	    *header_len)
{
    صحیح	    len = crypt_get_header_len(method_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_FIELD(arg);
    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len  = cryptmethods[method_nr].add_len;
    arg.cat_init_from_file = FALSE;

    *header_len = len;
    *header = alloc(len);
    اگر (*header == پوچ)
	بازگشت پوچ;

    mch_memmove(*header, cryptmethods[method_nr].magic, CRYPT_MAGIC_LEN);
    اگر (arg.cat_salt_len > 0 || arg.cat_seed_len > 0 || arg.cat_add_len > 0)
    {
	اگر (arg.cat_salt_len > 0)
	    arg.cat_salt = *header + CRYPT_MAGIC_LEN;
	اگر (arg.cat_seed_len > 0)
	    arg.cat_seed = *header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	اگر (arg.cat_add_len > 0)
	    arg.cat_add = *header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#ifdef FEAT_SODIUM
	اگر (sodium_init() >= 0)
	{
	    اگر (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    اگر (arg.cat_seed_len > 0)
		randombytes_buf(arg.cat_seed, arg.cat_seed_len);
	}
	وگرنه
#endif
	    sha2_seed(arg.cat_salt, arg.cat_salt_len, arg.cat_seed, arg.cat_seed_len);
    }
    state = crypt_create(method_nr, کلید, &arg);
    اگر (state == پوچ)
	VIM_CLEAR(*header);
    بازگشت state;
}

/*
 * Free the crypt state.
 */
    تهی
crypt_free_state(cryptstate_T *state)
{
#ifdef FEAT_SODIUM
    اگر (crypt_method_is_sodium(state->method_nr))
    {
	sodium_munlock(((sodium_state_T *)state->method_state)->کلید,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->method_state, اندازه(sodium_state_T));
	sodium_free(state->method_state);
    }
    وگرنه
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
    صحیح		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    اگر (method->encode_buffer_fn != پوچ)
	// Has buffer function, pass through.
	بازگشت method->encode_buffer_fn(state, from, len, newptr, last);
    اگر (len == 0)
	// Not buffering, just return EOF.
	بازگشت (long)len;

    *newptr = alloc(len + 50);
    اگر (*newptr == پوچ)
	بازگشت -1;
    method->encode_fn(state, from, len, *newptr, last);
    بازگشت (long)len;
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
    صحیح		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    اگر (method->decode_buffer_fn != پوچ)
	// Has buffer function, pass through.
	بازگشت method->decode_buffer_fn(state, ptr, len, newptr, last);

    اگر (len == 0)
	// Not buffering, just return EOF.
	بازگشت len;

    *newptr = alloc(len);
    اگر (*newptr == پوچ)
	بازگشت -1;
    method->decode_fn(state, ptr, len, *newptr, last);
    بازگشت len;
}
#endif

/*
 * Encrypting "from[len]" into "to[len]".
 */
    تهی
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    صحیح		last)
{
    cryptmethods[state->method_nr].encode_fn(state, from, len, to, last);
}

#اگر 0  // unused
/*
 * decrypting "from[len]" into "to[len]".
 */
    تهی
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    صحیح		last)
{
    cryptmethods[state->method_nr].decode_fn(state, from, len, to, last);
}
#endif

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    تهی
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    صحیح		last)
{
    cryptmethods[state->method_nr].encode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    تهی
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    صحیح		last)
{
    cryptmethods[state->method_nr].decode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    تهی
crypt_free_کلید(char_u *کلید)
{
    // Create a safe memset which cannot be optimized away by compiler
    ایستا تهی *(* فرار vim_memset_safe)(تهی *s, صحیح c, size_t n) =
	memset;
    اگر (کلید != پوچ)
    {
#ifdef FEAT_SODIUM
	اگر (sodium_init() >= 0)
	    sodium_memzero(کلید, STRLEN(کلید));
	وگرنه
#endif
	    vim_memset_safe(کلید, 0, STRLEN(کلید));
	vim_free(کلید);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    تهی
crypt_check_method(صحیح method)
{
    اگر (method < CRYPT_M_BF2 || method == CRYPT_M_SOD)
    {
	پیام_scroll = TRUE;
	پیام(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    تهی
crypt_check_swapfile_curbuf(تهی)
{
#ifdef FEAT_SODIUM
    صحیح method = crypt_get_method_nr(curbuf);
    اگر (crypt_method_is_sodium(method))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	set_option_مقدار_give_err((char_u *)"swf", 0, پوچ, OPT_LOCAL);
	پیام_scroll = TRUE;
	پیام(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#endif
}

    تهی
crypt_check_current_method(تهی)
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
crypt_get_کلید(
    صحیح		store,
    صحیح		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = پوچ;
    صحیح		round;

    برای (round = 0; ; ++round)
    {
	cmdline_star = TRUE;
	cmdline_row = پیام_row;
	p1 = getcmdline_prompt(NUL, round == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		پوچ);
	cmdline_star = FALSE;

	اگر (p1 == پوچ)
	    بشکن;

	اگر (round == twice)
	{
	    اگر (p2 != پوچ && STRCMP(p1, p2) != 0)
	    {
		پیام(_("Keys don't match!"));
		crypt_free_کلید(p1);
		crypt_free_کلید(p2);
		p2 = پوچ;
		round = -1;		// do it again
		ادامه;
	    }

	    اگر (store)
	    {
		set_option_مقدار_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_کلید(p1);
		p1 = curbuf->b_p_کلید;
		crypt_check_swapfile_curbuf();
	    }
	    بشکن;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    اگر (!crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
    {
	اگر (پیام_didout)
	    پیام_putchar('\n');
	need_wait_return = FALSE;
	پیام_didout = FALSE;
    }

    crypt_free_کلید(p2);
    بازگشت p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    تهی
crypt_append_پیام(
    buf_T *buf)
{
    اگر (crypt_get_method_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    وگرنه
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    ایستا صحیح
crypt_sodium_init_(
    cryptstate_T	*state UNUSED,
    char_u		*کلید UNUSED,
    crypt_arg_T		*arg UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned نویسه	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    صحیح			retval = 0;
    unsigned long long	opslimit;
    unsigned long long	memlimit;
    صحیح			alg;

    اگر (sodium_init() < 0)
	بازگشت FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(اندازه(sodium_state_T));
    sodium_memzero(sd_state, اندازه(sodium_state_T));

    اگر ((state->method_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->method_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# اگر 0
	// For testing
	اگر (state->method_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	اگر (crypto_pwhash(dkey, اندازه(dkey), (ثابت نویسه *)کلید, STRLEN(کلید),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    بازگشت FAIL;
	}
	memcpy(sd_state->کلید, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->کلید, crypto_box_SEEDBYTES);
	retval += sodium_mlock(کلید, STRLEN(کلید));

	اگر (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    بازگشت FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	اگر (state->method_nr == CRYPT_M_SOD2 && arg->cat_add != پوچ)
	{
	    char_u	buffer[20];
	    char_u	*p = buffer;
	    vim_memset(buffer, 0, 20);

	    crypt_long_long_to_char(opslimit, p);
	    p += اندازه(opslimit);

	    crypt_long_long_to_char(memlimit, p);
	    p += اندازه(memlimit);

	    crypt_int_to_char(alg, p);
	    memcpy(arg->cat_add, buffer, اندازه(opslimit) + اندازه(memlimit) + اندازه(alg));
	}
    }
    وگرنه
    {
	char_u	buffer[20];
	char_u	*p = buffer;
	vim_memset(buffer, 0, 20);
	صحیح	size = اندازه(opslimit) +
	    اندازه(memlimit) + اندازه(alg);

	// Reading parameters from file
	اگر (arg->cat_add_len < size)
	{
	    sodium_free(sd_state);
	    بازگشت FAIL;
	}

	// derive the key from the file header
	memcpy(p, arg->cat_add, size);
	arg->cat_add += size;

	opslimit = crypt_char_to_long_long(p);
	p += اندازه(opslimit);
	memlimit = crypt_char_to_long_long(p);
	p += اندازه(memlimit);
	alg = crypt_char_to_int(p);
	p += اندازه(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	اگر (crypto_pwhash(dkey, اندازه(dkey), (ثابت نویسه *)کلید, STRLEN(کلید),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    بازگشت FAIL;
	}
	memcpy(sd_state->کلید, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->کلید, crypto_box_SEEDBYTES);
	retval += sodium_mlock(کلید, STRLEN(کلید));

	اگر (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    بازگشت FAIL;
	}
    }
    sd_state->شمار = 0;
    state->method_state = sd_state;

    بازگشت OK;
#وگرنه
    emsg(_(e_libsodium_not_built_in));
    بازگشت FAIL;
#endif
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#اگر 0  // Currently unused
    تهی
crypt_sodium_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    صحیح		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned نویسه  tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;

    اگر (sod_st->شمار == 0)
    {
	اگر (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_header));
	    بازگشت;
	}
	crypto_secretstream_xchacha20poly1305_init_افزودن(&sod_st->state,
							      to, sod_st->کلید);
	to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    اگر (sod_st->شمار && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_buffer));
	بازگشت;
    }

    crypto_secretstream_xchacha20poly1305_افزودن(&sod_st->state, to, پوچ,
						      from, len, پوچ, 0, tag);

    sod_st->شمار++;
# endif
}
#endif

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#اگر 0  // Currently unused
    تهی
crypt_sodium_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    صحیح		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned نویسه  tag;
    unsigned long long buf_len;
    char_u *p1 = from;
    char_u *p2 = to;
    char_u *buf_out;

    اگر (sod_st->شمار == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_header));
	بازگشت;
    }

    buf_out = (char_u *)alloc(len);

    اگر (buf_out == پوچ)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	بازگشت;
    }
    اگر (sod_st->شمار == 0)
    {
	اگر (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->کلید) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    برو fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	اگر (p1 == p2)
	    to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    اگر (sod_st->شمار && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_buffer));
	برو fail;
    }
    اگر (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_out, &buf_len, &tag, from, len, پوچ, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	برو fail;
    }
    sod_st->شمار++;

    اگر (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	برو fail;
    }
    اگر (p1 == p2)
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
    ایستا long
crypt_sodium_buffer_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    صحیح		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned long long	out_len;
    char_u		*ptr;
    unsigned نویسه	tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;
    صحیح			length;
    sodium_state_T	*sod_st = state->method_state;
    صحیح			first = (sod_st->شمار == 0);

    length = (صحیح)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (first ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_out = alloc_clear(length);
    اگر (*buf_out == پوچ)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	بازگشت -1;
    }
    ptr = *buf_out;

    اگر (first)
    {
	crypto_secretstream_xchacha20poly1305_init_افزودن(&sod_st->state,
		ptr, sod_st->کلید);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_افزودن(&sod_st->state, ptr,
	    &out_len, from, len, پوچ, 0, tag);

    sod_st->شمار++;
    بازگشت out_len + (first
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#وگرنه
    بازگشت -1;
#endif
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ایستا long
crypt_sodium_buffer_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    صحیح		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned نویسه  tag;
    unsigned long long out_len;

    اگر (sod_st->شمار == 0
	    && state->method_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].add_len;

    *buf_out = alloc_clear(len);
    اگر (*buf_out == پوچ)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	بازگشت -1;
    }

    اگر (sod_st->شمار == 0)
    {
	اگر (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->کلید) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    بازگشت -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->شمار++;
    }
    اگر (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_out, &out_len, &tag, from, len, پوچ, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	بازگشت -1;
    }

    اگر (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
	emsg(_(e_libsodium_decryption_failed_premature));
    بازگشت (long) out_len;
#وگرنه
    بازگشت -1;
#endif
}

#اگر defined(FEAT_SODIUM)
    تهی
crypt_sodium_lock_کلید(char_u *کلید)
{
    اگر (sodium_init() >= 0)
	sodium_mlock(کلید, STRLEN(کلید));
}

    صحیح
crypt_sodium_munlock(تهی *ثابت addr, ثابت size_t len)
{
    بازگشت sodium_munlock(addr, len);
}

    تهی
crypt_sodium_randombytes_buf(تهی *ثابت buf, ثابت size_t size)
{
    randombytes_buf(buf, size);
}

    صحیح
crypt_sodium_init(تهی)
{
    بازگشت sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(تهی)
{
    بازگشت randombytes_random();
}

# اگر defined(FEAT_EVAL)
    ایستا تهی
crypt_sodium_report_hash_params(
	unsigned long long opslimit,
	unsigned long long ops_def,
	size_t memlimit,
	size_t mem_def,
	صحیح alg,
	صحیح alg_def)
{
    اگر (p_verbose > 0)
    {
	verbose_enter();
	اگر (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	وگرنه
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	اگر (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	وگرنه
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	اگر (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	وگرنه
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    ایستا تهی
crypt_long_long_to_char(long long n, char_u *s)
{
    صحیح i;
    برای (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    ایستا تهی
crypt_int_to_char(صحیح n, char_u *s)
{
    صحیح i;
    برای (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    ایستا long long
crypt_char_to_long_long(char_u *s)
{
    unsigned long long    retval = 0;
    صحیح i;
    برای (i = 7; i >= 0; i--)
    {
	اگر (i == 7)
	    retval = s[i];
	وگرنه
	    retval |= s[i];
	اگر (i > 0)
	    retval <<= 8;
    }
    بازگشت retval;
}

    ایستا صحیح
crypt_char_to_int(char_u *s)
{
    صحیح retval = 0;
    صحیح i;

    برای (i = 3; i >= 0; i--)
    {
	اگر (i == 3)
	    retval = s[i];
	وگرنه
	    retval |= s[i];
	اگر (i > 0)
	    retval <<= 8;
    }
    بازگشت retval;
}
#endif

#endif // FEAT_CRYPT
