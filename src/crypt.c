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
#ضمن "vim.h"

#𐤀𐤌 defined(FEAT_CRYPT)
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

تعريفنوع بنية {
    حرف	*name;		// encryption name as used in 'cryptmethod'
    حرف	*magic;		// magic bytes stored in file header
    صحيح		salt_len;	// length of salt, or 0 when not using salt
    صحيح		seed_len;	// length of seed, or 0 when not using seed
    صحيح		add_len;	// additional length in the header needed for storing
				// custom data
#ifdef CRYPT_NOT_INPLACE
    صحيح		works_inplace;	// encryption/decryption can be done in-place
#endif
    صحيح		whole_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    صحيح (*self_test_fn)(فراغ);

    // Function pointer for initializing encryption/decryption.
    صحيح (* init_fn)(cryptstate_T *state, char_u *key, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    فراغ (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, صحيح last);
    فراغ (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, صحيح last);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    long (*encode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, صحيح last);
    long (*decode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, صحيح last);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    فراغ (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, صحيح last);
    فراغ (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, صحيح last);
} cryptmethod_T;

ساكن صحيح crypt_sodium_init_(cryptstate_T *state, char_u *key, crypt_arg_T *arg);
ساكن long crypt_sodium_buffer_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, صحيح last);
ساكن long crypt_sodium_buffer_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, صحيح last);
#𐤀𐤌 defined(FEAT_SODIUM)
ساكن فراغ crypt_long_long_to_char(long long n, char_u *s);
ساكن فراغ crypt_int_to_char(صحيح n, char_u *s);
ساكن long long crypt_char_to_long_long(char_u *s);
ساكن صحيح crypt_char_to_int(char_u *s);
#endif
#𐤀𐤌 defined(FEAT_EVAL) && defined(FEAT_SODIUM)
ساكن فراغ crypt_sodium_report_hash_params(unsigned long long opslimit, unsigned long long ops_def, size_t memlimit, size_t mem_def, صحيح alg, صحيح alg_def);
#endif

// index is method_nr of cryptstate_T, CRYPT_M_*
ساكن cryptmethod_T cryptmethods[CRYPT_M_COUNT] = {
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
	عدم,
	crypt_zip_init,
	crypt_zip_encode, crypt_zip_decode,
	عدم, عدم,
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
	عدم, عدم,
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
	عدم, عدم,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // XChaCha20 using libsodium; implementation issues
    {
	"xchacha20",
	"VimCrypt~04!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#وإلا
	16,
#endif
	8,
	0,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	عدم,
	crypt_sodium_init_,
	عدم, عدم,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	عدم, عدم,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#وإلا
	16,
#endif
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	عدم,
	crypt_sodium_init_,
	عدم, عدم,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	عدم, عدم,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#𐤀𐤌 defined(FEAT_SODIUM)
تعريفنوع بنية {
    size_t	    count;
    unsigned حرف   key[crypto_box_SEEDBYTES];
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
#  وإلا
#   error Dynamic loading of libsodium is not supported لكل now.
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

ساكن صحيح (*dll_sodium_init)(فراغ) = عدم;
ساكن فراغ (*dll_sodium_free)(فراغ *) = عدم;
ساكن فراغ *(*dll_sodium_malloc)(ثابت size_t) = عدم;
ساكن فراغ (*dll_sodium_memzero)(فراغ * ثابت, ثابت size_t) = عدم;
ساكن صحيح (*dll_sodium_mlock)(فراغ * ثابت, ثابت size_t) = عدم;
ساكن صحيح (*dll_sodium_munlock)(فراغ * ثابت, ثابت size_t) = عدم;
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_init_push)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned حرف [],
    ثابت unsigned حرف []) = عدم;
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_push)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned حرف *c, unsigned long long *clen_p,
    ثابت unsigned حرف *m, unsigned long long mlen,
    ثابت unsigned حرف *ad, unsigned long long adlen, unsigned حرف tag)
    = عدم;
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ثابت unsigned حرف [],
    ثابت unsigned حرف []) = عدم;
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned حرف *m, unsigned long long *mlen_p, unsigned حرف *tag_p,
    ثابت unsigned حرف *c, unsigned long long clen,
    ثابت unsigned حرف *ad, unsigned long long adlen) = عدم;
ساكن صحيح (*dll_crypto_pwhash)(unsigned حرف * ثابت out,
    unsigned long long outlen,
    ثابت حرف * ثابت passwd, unsigned long long passwdlen,
    ثابت unsigned حرف * ثابت salt,
    unsigned long long opslimit, size_t memlimit, صحيح alg)
    = عدم;
ساكن فراغ (*dll_randombytes_buf)(فراغ * ثابت buf, ثابت size_t size);
ساكن uint32_t (*dll_randombytes_random)(فراغ);

ساكن بنية {
    ثابت حرف *name;
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
    {عدم, عدم}
};

    ساكن صحيح
sodium_runtime_link_init(صحيح verbose)
{
    ساكن HINSTANCE hsodium = عدم;
    ثابت حرف *libname = DYNAMIC_SODIUM_DLL;
    صحيح i;

    𐤀𐤌 (hsodium != عدم)
	أرجع OK;

    hsodium = load_dll(libname);
    𐤀𐤌 (hsodium == عدم)
    {
	𐤀𐤌 (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	أرجع FAIL;
    }

    لكل (i = 0; sodium_funcname_table[i].ptr; ++i)
    {
	𐤀𐤌 ((*sodium_funcname_table[i].ptr = symbol_from_dll(hsodium,
			sodium_funcname_table[i].name)) == عدم)
	{
	    close_dll(hsodium);
	    hsodium = عدم;
	    𐤀𐤌 (verbose)
		semsg(_(e_could_not_load_library_function_str), sodium_funcname_table[i].name);
	    أرجع FAIL;
	}
    }
    أرجع OK;
}

    ساكن صحيح
load_sodium(فراغ)
{
    𐤀𐤌 (sodium_runtime_link_init(TRUE) == FAIL)
	أرجع -1;
    أرجع dll_sodium_init();
}
# endif

# 𐤀𐤌 defined(DYNAMIC_SODIUM)
    صحيح
sodium_enabled(صحيح verbose)
{
    أرجع sodium_runtime_link_init(verbose) == OK;
}
# endif
#endif

#define CRYPT_MAGIC_LEN	12	// cannot change
ساكن ثابت حرف crypt_magic_head[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    صحيح
crypt_method_nr_from_name(char_u *name)
{
    صحيح i;

    لكل (i = 0; i < CRYPT_M_COUNT; ++i)
	𐤀𐤌 (STRCMP(name, cryptmethods[i].name) == 0)
	    أرجع i;
    أرجع 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    صحيح
crypt_method_nr_from_magic(حرف *ptr, صحيح len)
{
    صحيح i;

    𐤀𐤌 (len < CRYPT_MAGIC_LEN)
	أرجع -1;

    لكل (i = 0; i < CRYPT_M_COUNT; i++)
	𐤀𐤌 (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    أرجع i;

    i = (صحيح)STRLEN(crypt_magic_head);
    𐤀𐤌 (len >= i && memcmp(ptr, crypt_magic_head, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_method));

    أرجع -1;
}

#ifdef CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    صحيح
crypt_works_inplace(cryptstate_T *state)
{
    أرجع cryptmethods[state->method_nr].works_inplace;
}
#endif

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    صحيح
crypt_get_method_nr(buf_T *buf)
{
    أرجع crypt_method_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    صحيح
crypt_method_is_sodium(صحيح method)
{
    أرجع method == CRYPT_M_SOD || method == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    صحيح
crypt_whole_undofile(صحيح method_nr)
{
    أرجع cryptmethods[method_nr].whole_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    صحيح
crypt_get_header_len(صحيح method_nr)
{
    أرجع CRYPT_MAGIC_LEN
	+ cryptmethods[method_nr].salt_len
	+ cryptmethods[method_nr].seed_len
	+ cryptmethods[method_nr].add_len;
}


#𐤀𐤌 defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    صحيح
crypt_get_max_header_len(فراغ)
{
    صحيح i;
    صحيح max = 0;
    صحيح temp = 0;

    لكل (i = 0; i < CRYPT_M_COUNT; ++i)
    {
	temp = crypt_get_header_len(i);
	𐤀𐤌 (temp > max)
	    max = temp;
    }
    أرجع max;
}
#endif

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    فراغ
crypt_set_cm_option(buf_T *buf, صحيح method_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[method_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    صحيح
crypt_self_test(فراغ)
{
    صحيح method_nr = crypt_get_method_nr(curbuf);

    𐤀𐤌 (cryptmethods[method_nr].self_test_fn == عدم)
	أرجع OK;
    أرجع cryptmethods[method_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    صحيح		method_nr,
    char_u	*key,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    𐤀𐤌 (state == عدم)
	أرجع state;

    state->method_nr = method_nr;
    𐤀𐤌 (cryptmethods[method_nr].init_fn(state, key, crypt_arg) == FAIL)
    {
	vim_free(state);
	أرجع عدم;
    }
    أرجع state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_header(
    صحيح		method_nr,
    char_u	*key,
    char_u	*header)
{
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len = cryptmethods[method_nr].add_len;
    𐤀𐤌 (arg.cat_salt_len > 0)
	arg.cat_salt = header + CRYPT_MAGIC_LEN;
    𐤀𐤌 (arg.cat_seed_len > 0)
	arg.cat_seed = header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    𐤀𐤌 (arg.cat_add_len > 0)
	arg.cat_add = header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

    أرجع crypt_create(method_nr, key, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *key)
{
    صحيح		method_nr;
    صحيح		header_len;
    حرف	magic_buffer[CRYPT_MAGIC_LEN];
    char_u	*buffer;
    cryptstate_T *state;

    𐤀𐤌 (fread(magic_buffer, CRYPT_MAGIC_LEN, 1, fp) != 1)
	أرجع عدم;
    method_nr = crypt_method_nr_from_magic(magic_buffer, CRYPT_MAGIC_LEN);
    𐤀𐤌 (method_nr < 0)
	أرجع عدم;

    header_len = crypt_get_header_len(method_nr);
    𐤀𐤌 ((buffer = alloc(header_len)) == عدم)
	أرجع عدم;
    mch_memmove(buffer, magic_buffer, CRYPT_MAGIC_LEN);
    𐤀𐤌 (header_len > CRYPT_MAGIC_LEN
	    && fread(buffer + CRYPT_MAGIC_LEN,
				    header_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(buffer);
	أرجع عدم;
    }

    state = crypt_create_from_header(method_nr, key, buffer);
    vim_free(buffer);
    أرجع state;
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
    صحيح	    method_nr,
    char_u  *key,
    char_u  **header,
    صحيح	    *header_len)
{
    صحيح	    len = crypt_get_header_len(method_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_FIELD(arg);
    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len  = cryptmethods[method_nr].add_len;
    arg.cat_init_from_file = FALSE;

    *header_len = len;
    *header = alloc(len);
    𐤀𐤌 (*header == عدم)
	أرجع عدم;

    mch_memmove(*header, cryptmethods[method_nr].magic, CRYPT_MAGIC_LEN);
    𐤀𐤌 (arg.cat_salt_len > 0 || arg.cat_seed_len > 0 || arg.cat_add_len > 0)
    {
	𐤀𐤌 (arg.cat_salt_len > 0)
	    arg.cat_salt = *header + CRYPT_MAGIC_LEN;
	𐤀𐤌 (arg.cat_seed_len > 0)
	    arg.cat_seed = *header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	𐤀𐤌 (arg.cat_add_len > 0)
	    arg.cat_add = *header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#ifdef FEAT_SODIUM
	𐤀𐤌 (sodium_init() >= 0)
	{
	    𐤀𐤌 (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    𐤀𐤌 (arg.cat_seed_len > 0)
		randombytes_buf(arg.cat_seed, arg.cat_seed_len);
	}
	وإلا
#endif
	    sha2_seed(arg.cat_salt, arg.cat_salt_len, arg.cat_seed, arg.cat_seed_len);
    }
    state = crypt_create(method_nr, key, &arg);
    𐤀𐤌 (state == عدم)
	VIM_CLEAR(*header);
    أرجع state;
}

/*
 * Free the crypt state.
 */
    فراغ
crypt_free_state(cryptstate_T *state)
{
#ifdef FEAT_SODIUM
    𐤀𐤌 (crypt_method_is_sodium(state->method_nr))
    {
	sodium_munlock(((sodium_state_T *)state->method_state)->key,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->method_state, حجم(sodium_state_T));
	sodium_free(state->method_state);
    }
    وإلا
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
    صحيح		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    𐤀𐤌 (method->encode_buffer_fn != عدم)
	// Has buffer function, pass through.
	أرجع method->encode_buffer_fn(state, from, len, newptr, last);
    𐤀𐤌 (len == 0)
	// Not buffering, just return EOF.
	أرجع (long)len;

    *newptr = alloc(len + 50);
    𐤀𐤌 (*newptr == عدم)
	أرجع -1;
    method->encode_fn(state, from, len, *newptr, last);
    أرجع (long)len;
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
    صحيح		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    𐤀𐤌 (method->decode_buffer_fn != عدم)
	// Has buffer function, pass through.
	أرجع method->decode_buffer_fn(state, ptr, len, newptr, last);

    𐤀𐤌 (len == 0)
	// Not buffering, just return EOF.
	أرجع len;

    *newptr = alloc(len);
    𐤀𐤌 (*newptr == عدم)
	أرجع -1;
    method->decode_fn(state, ptr, len, *newptr, last);
    أرجع len;
}
#endif

/*
 * Encrypting "from[len]" into "to[len]".
 */
    فراغ
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    صحيح		last)
{
    cryptmethods[state->method_nr].encode_fn(state, from, len, to, last);
}

#𐤀𐤌 0  // unused
/*
 * decrypting "from[len]" into "to[len]".
 */
    فراغ
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    صحيح		last)
{
    cryptmethods[state->method_nr].decode_fn(state, from, len, to, last);
}
#endif

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    فراغ
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    صحيح		last)
{
    cryptmethods[state->method_nr].encode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    فراغ
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    صحيح		last)
{
    cryptmethods[state->method_nr].decode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    فراغ
crypt_free_key(char_u *key)
{
    // Create a safe memset which cannot be optimized away by compiler
    ساكن فراغ *(* متطاير vim_memset_safe)(فراغ *s, صحيح c, size_t n) =
	memset;
    𐤀𐤌 (key != عدم)
    {
#ifdef FEAT_SODIUM
	𐤀𐤌 (sodium_init() >= 0)
	    sodium_memzero(key, STRLEN(key));
	وإلا
#endif
	    vim_memset_safe(key, 0, STRLEN(key));
	vim_free(key);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    فراغ
crypt_check_method(صحيح method)
{
    𐤀𐤌 (method < CRYPT_M_BF2 || method == CRYPT_M_SOD)
    {
	msg_scroll = TRUE;
	msg(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    فراغ
crypt_check_swapfile_curbuf(فراغ)
{
#ifdef FEAT_SODIUM
    صحيح method = crypt_get_method_nr(curbuf);
    𐤀𐤌 (crypt_method_is_sodium(method))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	set_option_value_give_err((char_u *)"swf", 0, عدم, OPT_LOCAL);
	msg_scroll = TRUE;
	msg(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#endif
}

    فراغ
crypt_check_current_method(فراغ)
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
    صحيح		store,
    صحيح		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = عدم;
    صحيح		round;

    لكل (round = 0; ; ++round)
    {
	cmdline_star = TRUE;
	cmdline_row = msg_row;
	p1 = getcmdline_prompt(NUL, round == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		عدم);
	cmdline_star = FALSE;

	𐤀𐤌 (p1 == عدم)
	    اكسر;

	𐤀𐤌 (round == twice)
	{
	    𐤀𐤌 (p2 != عدم && STRCMP(p1, p2) != 0)
	    {
		msg(_("Keys don't match!"));
		crypt_free_key(p1);
		crypt_free_key(p2);
		p2 = عدم;
		round = -1;		// do it again
		استمر;
	    }

	    𐤀𐤌 (store)
	    {
		set_option_value_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_key(p1);
		p1 = curbuf->b_p_key;
		crypt_check_swapfile_curbuf();
	    }
	    اكسر;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    𐤀𐤌 (!crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
    {
	𐤀𐤌 (msg_didout)
	    msg_putchar('\n');
	need_wait_return = FALSE;
	msg_didout = FALSE;
    }

    crypt_free_key(p2);
    أرجع p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    فراغ
crypt_append_msg(
    buf_T *buf)
{
    𐤀𐤌 (crypt_get_method_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    وإلا
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    ساكن صحيح
crypt_sodium_init_(
    cryptstate_T	*state UNUSED,
    char_u		*key UNUSED,
    crypt_arg_T		*arg UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned حرف	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    صحيح			retval = 0;
    unsigned long long	opslimit;
    unsigned long long	memlimit;
    صحيح			alg;

    𐤀𐤌 (sodium_init() < 0)
	أرجع FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(حجم(sodium_state_T));
    sodium_memzero(sd_state, حجم(sodium_state_T));

    𐤀𐤌 ((state->method_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->method_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# 𐤀𐤌 0
	// For testing
	𐤀𐤌 (state->method_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	𐤀𐤌 (crypto_pwhash(dkey, حجم(dkey), (ثابت حرف *)key, STRLEN(key),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
	memcpy(sd_state->key, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->key, crypto_box_SEEDBYTES);
	retval += sodium_mlock(key, STRLEN(key));

	𐤀𐤌 (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	𐤀𐤌 (state->method_nr == CRYPT_M_SOD2 && arg->cat_add != عدم)
	{
	    char_u	buffer[20];
	    char_u	*p = buffer;
	    vim_memset(buffer, 0, 20);

	    crypt_long_long_to_char(opslimit, p);
	    p += حجم(opslimit);

	    crypt_long_long_to_char(memlimit, p);
	    p += حجم(memlimit);

	    crypt_int_to_char(alg, p);
	    memcpy(arg->cat_add, buffer, حجم(opslimit) + حجم(memlimit) + حجم(alg));
	}
    }
    وإلا
    {
	char_u	buffer[20];
	char_u	*p = buffer;
	vim_memset(buffer, 0, 20);
	صحيح	size = حجم(opslimit) +
	    حجم(memlimit) + حجم(alg);

	// Reading parameters from file
	𐤀𐤌 (arg->cat_add_len < size)
	{
	    sodium_free(sd_state);
	    أرجع FAIL;
	}

	// derive the key from the file header
	memcpy(p, arg->cat_add, size);
	arg->cat_add += size;

	opslimit = crypt_char_to_long_long(p);
	p += حجم(opslimit);
	memlimit = crypt_char_to_long_long(p);
	p += حجم(memlimit);
	alg = crypt_char_to_int(p);
	p += حجم(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	𐤀𐤌 (crypto_pwhash(dkey, حجم(dkey), (ثابت حرف *)key, STRLEN(key),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
	memcpy(sd_state->key, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->key, crypto_box_SEEDBYTES);
	retval += sodium_mlock(key, STRLEN(key));

	𐤀𐤌 (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
    }
    sd_state->count = 0;
    state->method_state = sd_state;

    أرجع OK;
#وإلا
    emsg(_(e_libsodium_not_built_in));
    أرجع FAIL;
#endif
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#𐤀𐤌 0  // Currently unused
    فراغ
crypt_sodium_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    صحيح		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned حرف  tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;

    𐤀𐤌 (sod_st->count == 0)
    {
	𐤀𐤌 (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_header));
	    أرجع;
	}
	crypto_secretstream_xchacha20poly1305_init_push(&sod_st->state,
							      to, sod_st->key);
	to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    𐤀𐤌 (sod_st->count && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_buffer));
	أرجع;
    }

    crypto_secretstream_xchacha20poly1305_push(&sod_st->state, to, عدم,
						      from, len, عدم, 0, tag);

    sod_st->count++;
# endif
}
#endif

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#𐤀𐤌 0  // Currently unused
    فراغ
crypt_sodium_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    صحيح		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned حرف  tag;
    unsigned long long buf_len;
    char_u *p1 = from;
    char_u *p2 = to;
    char_u *buf_out;

    𐤀𐤌 (sod_st->count == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_header));
	أرجع;
    }

    buf_out = (char_u *)alloc(len);

    𐤀𐤌 (buf_out == عدم)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	أرجع;
    }
    𐤀𐤌 (sod_st->count == 0)
    {
	𐤀𐤌 (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->key) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    اذهب fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	𐤀𐤌 (p1 == p2)
	    to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    𐤀𐤌 (sod_st->count && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_buffer));
	اذهب fail;
    }
    𐤀𐤌 (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_out, &buf_len, &tag, from, len, عدم, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	اذهب fail;
    }
    sod_st->count++;

    𐤀𐤌 (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	اذهب fail;
    }
    𐤀𐤌 (p1 == p2)
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
    ساكن long
crypt_sodium_buffer_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    صحيح		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned long long	out_len;
    char_u		*ptr;
    unsigned حرف	tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;
    صحيح			length;
    sodium_state_T	*sod_st = state->method_state;
    صحيح			first = (sod_st->count == 0);

    length = (صحيح)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (first ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_out = alloc_clear(length);
    𐤀𐤌 (*buf_out == عدم)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	أرجع -1;
    }
    ptr = *buf_out;

    𐤀𐤌 (first)
    {
	crypto_secretstream_xchacha20poly1305_init_push(&sod_st->state,
		ptr, sod_st->key);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_push(&sod_st->state, ptr,
	    &out_len, from, len, عدم, 0, tag);

    sod_st->count++;
    أرجع out_len + (first
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#وإلا
    أرجع -1;
#endif
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ساكن long
crypt_sodium_buffer_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    صحيح		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned حرف  tag;
    unsigned long long out_len;

    𐤀𐤌 (sod_st->count == 0
	    && state->method_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].add_len;

    *buf_out = alloc_clear(len);
    𐤀𐤌 (*buf_out == عدم)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	أرجع -1;
    }

    𐤀𐤌 (sod_st->count == 0)
    {
	𐤀𐤌 (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->key) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    أرجع -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->count++;
    }
    𐤀𐤌 (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_out, &out_len, &tag, from, len, عدم, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	أرجع -1;
    }

    𐤀𐤌 (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
	emsg(_(e_libsodium_decryption_failed_premature));
    أرجع (long) out_len;
#وإلا
    أرجع -1;
#endif
}

#𐤀𐤌 defined(FEAT_SODIUM)
    فراغ
crypt_sodium_lock_key(char_u *key)
{
    𐤀𐤌 (sodium_init() >= 0)
	sodium_mlock(key, STRLEN(key));
}

    صحيح
crypt_sodium_munlock(فراغ *ثابت addr, ثابت size_t len)
{
    أرجع sodium_munlock(addr, len);
}

    فراغ
crypt_sodium_randombytes_buf(فراغ *ثابت buf, ثابت size_t size)
{
    randombytes_buf(buf, size);
}

    صحيح
crypt_sodium_init(فراغ)
{
    أرجع sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(فراغ)
{
    أرجع randombytes_random();
}

# 𐤀𐤌 defined(FEAT_EVAL)
    ساكن فراغ
crypt_sodium_report_hash_params(
	unsigned long long opslimit,
	unsigned long long ops_def,
	size_t memlimit,
	size_t mem_def,
	صحيح alg,
	صحيح alg_def)
{
    𐤀𐤌 (p_verbose > 0)
    {
	verbose_enter();
	𐤀𐤌 (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	وإلا
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	𐤀𐤌 (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	وإلا
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	𐤀𐤌 (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	وإلا
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    ساكن فراغ
crypt_long_long_to_char(long long n, char_u *s)
{
    صحيح i;
    لكل (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    ساكن فراغ
crypt_int_to_char(صحيح n, char_u *s)
{
    صحيح i;
    لكل (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    ساكن long long
crypt_char_to_long_long(char_u *s)
{
    unsigned long long    retval = 0;
    صحيح i;
    لكل (i = 7; i >= 0; i--)
    {
	𐤀𐤌 (i == 7)
	    retval = s[i];
	وإلا
	    retval |= s[i];
	𐤀𐤌 (i > 0)
	    retval <<= 8;
    }
    أرجع retval;
}

    ساكن صحيح
crypt_char_to_int(char_u *s)
{
    صحيح retval = 0;
    صحيح i;

    لكل (i = 3; i >= 0; i--)
    {
	𐤀𐤌 (i == 3)
	    retval = s[i];
	وإلا
	    retval |= s[i];
	𐤀𐤌 (i > 0)
	    retval <<= 8;
    }
    أرجع retval;
}
#endif

#endif // FEAT_CRYPT
