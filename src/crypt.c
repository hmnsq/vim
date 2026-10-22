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

#जर defined(FEAT_CRYPT)
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

प्रकारव्याख्या संरचना {
    अक्षर	*name;		// encryption name as used in 'cryptmethod'
    अक्षर	*magic;		// magic bytes stored in file header
    पूर्णांक		salt_len;	// length of salt, or 0 when not using salt
    पूर्णांक		seed_len;	// length of seed, or 0 when not using seed
    पूर्णांक		add_len;	// additional length in the header needed for storing
				// custom data
#ifdef CRYPT_NOT_INPLACE
    पूर्णांक		works_inplace;	// encryption/decryption can be done in-place
#endif
    पूर्णांक		whole_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    पूर्णांक (*self_test_fn)(शून्य);

    // Function pointer for initializing encryption/decryption.
    पूर्णांक (* init_fn)(cryptstate_T *state, char_u *किल्ली, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    शून्य (*encode_fn)(cryptstate_T *state, char_u *from, माप_प्रकार len,
							char_u *to, पूर्णांक last);
    शून्य (*decode_fn)(cryptstate_T *state, char_u *from, माप_प्रकार len,
							char_u *to, पूर्णांक last);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    दीर्घ (*encode_buffer_fn)(cryptstate_T *state, char_u *from, माप_प्रकार len,
						    char_u **newptr, पूर्णांक last);
    दीर्घ (*decode_buffer_fn)(cryptstate_T *state, char_u *from, माप_प्रकार len,
						    char_u **newptr, पूर्णांक last);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    शून्य (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, माप_प्रकार len,
							char_u *p2, पूर्णांक last);
    शून्य (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, माप_प्रकार len,
							char_u *p2, पूर्णांक last);
} cryptmethod_T;

स्थिर पूर्णांक crypt_sodium_init_(cryptstate_T *state, char_u *किल्ली, crypt_arg_T *arg);
स्थिर दीर्घ crypt_sodium_buffer_decode(cryptstate_T *state, char_u *from, माप_प्रकार len, char_u **buf_out, पूर्णांक last);
स्थिर दीर्घ crypt_sodium_buffer_encode(cryptstate_T *state, char_u *from, माप_प्रकार len, char_u **buf_out, पूर्णांक last);
#जर defined(FEAT_SODIUM)
स्थिर शून्य crypt_long_long_to_char(दीर्घ दीर्घ n, char_u *s);
स्थिर शून्य crypt_int_to_char(पूर्णांक n, char_u *s);
स्थिर दीर्घ दीर्घ crypt_char_to_long_long(char_u *s);
स्थिर पूर्णांक crypt_char_to_int(char_u *s);
#endif
#जर defined(FEAT_EVAL) && defined(FEAT_SODIUM)
स्थिर शून्य crypt_sodium_report_hash_params(अचिन्हित दीर्घ दीर्घ opslimit, अचिन्हित दीर्घ दीर्घ ops_def, माप_प्रकार memlimit, माप_प्रकार mem_def, पूर्णांक alg, पूर्णांक alg_def);
#endif

// index is method_nr of cryptstate_T, CRYPT_M_*
स्थिर cryptmethod_T cryptmethods[CRYPT_M_मोजणी] = {
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
#अन्यथा
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
#अन्यथा
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

#जर defined(FEAT_SODIUM)
प्रकारव्याख्या संरचना {
    माप_प्रकार	    मोजणी;
    अचिन्हित अक्षर   किल्ली[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define load_dll vimLoadLib
#   define चिन्ह_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  अन्यथा
#   error Dynamic loading of libsodium is not supported साठी now.
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
#  define crypto_secretstream_xchacha20poly1305_init_ढकला \
    dll_crypto_secretstream_xchacha20poly1305_init_ढकला
#  define crypto_secretstream_xchacha20poly1305_ढकला \
    dll_crypto_secretstream_xchacha20poly1305_ढकला
#  define crypto_secretstream_xchacha20poly1305_init_pull \
    dll_crypto_secretstream_xchacha20poly1305_init_pull
#  define crypto_secretstream_xchacha20poly1305_pull \
    dll_crypto_secretstream_xchacha20poly1305_pull
#  define crypto_pwhash	    dll_crypto_pwhash
#  define randombytes_buf   dll_randombytes_buf
#  define randombytes_random dll_randombytes_random

स्थिर पूर्णांक (*dll_sodium_init)(शून्य) = रिक्त;
स्थिर शून्य (*dll_sodium_free)(शून्य *) = रिक्त;
स्थिर शून्य *(*dll_sodium_malloc)(स्थिरांक माप_प्रकार) = रिक्त;
स्थिर शून्य (*dll_sodium_memzero)(शून्य * स्थिरांक, स्थिरांक माप_प्रकार) = रिक्त;
स्थिर पूर्णांक (*dll_sodium_mlock)(शून्य * स्थिरांक, स्थिरांक माप_प्रकार) = रिक्त;
स्थिर पूर्णांक (*dll_sodium_munlock)(शून्य * स्थिरांक, स्थिरांक माप_प्रकार) = रिक्त;
स्थिर पूर्णांक (*dll_crypto_secretstream_xchacha20poly1305_init_ढकला)
   (crypto_secretstream_xchacha20poly1305_state *state,
    अचिन्हित अक्षर [],
    स्थिरांक अचिन्हित अक्षर []) = रिक्त;
स्थिर पूर्णांक (*dll_crypto_secretstream_xchacha20poly1305_ढकला)
   (crypto_secretstream_xchacha20poly1305_state *state,
    अचिन्हित अक्षर *c, अचिन्हित दीर्घ दीर्घ *clen_p,
    स्थिरांक अचिन्हित अक्षर *m, अचिन्हित दीर्घ दीर्घ mlen,
    स्थिरांक अचिन्हित अक्षर *ad, अचिन्हित दीर्घ दीर्घ adlen, अचिन्हित अक्षर tag)
    = रिक्त;
स्थिर पूर्णांक (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    स्थिरांक अचिन्हित अक्षर [],
    स्थिरांक अचिन्हित अक्षर []) = रिक्त;
स्थिर पूर्णांक (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    अचिन्हित अक्षर *m, अचिन्हित दीर्घ दीर्घ *mlen_p, अचिन्हित अक्षर *tag_p,
    स्थिरांक अचिन्हित अक्षर *c, अचिन्हित दीर्घ दीर्घ clen,
    स्थिरांक अचिन्हित अक्षर *ad, अचिन्हित दीर्घ दीर्घ adlen) = रिक्त;
स्थिर पूर्णांक (*dll_crypto_pwhash)(अचिन्हित अक्षर * स्थिरांक out,
    अचिन्हित दीर्घ दीर्घ outlen,
    स्थिरांक अक्षर * स्थिरांक passwd, अचिन्हित दीर्घ दीर्घ passwdlen,
    स्थिरांक अचिन्हित अक्षर * स्थिरांक salt,
    अचिन्हित दीर्घ दीर्घ opslimit, माप_प्रकार memlimit, पूर्णांक alg)
    = रिक्त;
स्थिर शून्य (*dll_randombytes_buf)(शून्य * स्थिरांक buf, स्थिरांक माप_प्रकार size);
स्थिर uint32_t (*dll_randombytes_random)(शून्य);

स्थिर संरचना {
    स्थिरांक अक्षर *name;
    SODIUM_PROC *ptr;
} sodium_funcname_table[] = {
    {"sodium_init", (SODIUM_PROC*)&dll_sodium_init},
    {"sodium_free", (SODIUM_PROC*)&dll_sodium_free},
    {"sodium_malloc", (SODIUM_PROC*)&dll_sodium_malloc},
    {"sodium_memzero", (SODIUM_PROC*)&dll_sodium_memzero},
    {"sodium_mlock", (SODIUM_PROC*)&dll_sodium_mlock},
    {"sodium_munlock", (SODIUM_PROC*)&dll_sodium_munlock},
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_ढकला},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_ढकला},
    {"crypto_secretstream_xchacha20poly1305_init_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_pull},
    {"crypto_secretstream_xchacha20poly1305_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_pull},
    {"crypto_pwhash", (SODIUM_PROC*)&dll_crypto_pwhash},
    {"randombytes_buf", (SODIUM_PROC*)&dll_randombytes_buf},
    {"randombytes_random", (SODIUM_PROC*)&dll_randombytes_random},
    {रिक्त, रिक्त}
};

    स्थिर पूर्णांक
sodium_runtime_link_init(पूर्णांक verbose)
{
    स्थिर HINSTANCE hsodium = रिक्त;
    स्थिरांक अक्षर *libname = DYNAMIC_SODIUM_DLL;
    पूर्णांक i;

    जर (hsodium != रिक्त)
	परत OK;

    hsodium = load_dll(libname);
    जर (hsodium == रिक्त)
    {
	जर (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	परत FAIL;
    }

    साठी (i = 0; sodium_funcname_table[i].ptr; ++i)
    {
	जर ((*sodium_funcname_table[i].ptr = चिन्ह_from_dll(hsodium,
			sodium_funcname_table[i].name)) == रिक्त)
	{
	    close_dll(hsodium);
	    hsodium = रिक्त;
	    जर (verbose)
		semsg(_(e_could_not_load_library_function_str), sodium_funcname_table[i].name);
	    परत FAIL;
	}
    }
    परत OK;
}

    स्थिर पूर्णांक
load_sodium(शून्य)
{
    जर (sodium_runtime_link_init(TRUE) == FAIL)
	परत -1;
    परत dll_sodium_init();
}
# endif

# जर defined(DYNAMIC_SODIUM)
    पूर्णांक
sodium_enabled(पूर्णांक verbose)
{
    परत sodium_runtime_link_init(verbose) == OK;
}
# endif
#endif

#define CRYPT_MAGIC_LEN	12	// cannot change
स्थिर स्थिरांक अक्षर crypt_magic_head[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    पूर्णांक
crypt_method_nr_from_name(char_u *name)
{
    पूर्णांक i;

    साठी (i = 0; i < CRYPT_M_मोजणी; ++i)
	जर (STRCMP(name, cryptmethods[i].name) == 0)
	    परत i;
    परत 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    पूर्णांक
crypt_method_nr_from_magic(अक्षर *ptr, पूर्णांक len)
{
    पूर्णांक i;

    जर (len < CRYPT_MAGIC_LEN)
	परत -1;

    साठी (i = 0; i < CRYPT_M_मोजणी; i++)
	जर (स_भेद(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    परत i;

    i = (पूर्णांक)STRLEN(crypt_magic_head);
    जर (len >= i && स_भेद(ptr, crypt_magic_head, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_method));

    परत -1;
}

#ifdef CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    पूर्णांक
crypt_works_inplace(cryptstate_T *state)
{
    परत cryptmethods[state->method_nr].works_inplace;
}
#endif

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    पूर्णांक
crypt_get_method_nr(buf_T *buf)
{
    परत crypt_method_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    पूर्णांक
crypt_method_is_sodium(पूर्णांक method)
{
    परत method == CRYPT_M_SOD || method == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    पूर्णांक
crypt_whole_undofile(पूर्णांक method_nr)
{
    परत cryptmethods[method_nr].whole_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    पूर्णांक
crypt_get_header_len(पूर्णांक method_nr)
{
    परत CRYPT_MAGIC_LEN
	+ cryptmethods[method_nr].salt_len
	+ cryptmethods[method_nr].seed_len
	+ cryptmethods[method_nr].add_len;
}


#जर defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    पूर्णांक
crypt_get_max_header_len(शून्य)
{
    पूर्णांक i;
    पूर्णांक max = 0;
    पूर्णांक temp = 0;

    साठी (i = 0; i < CRYPT_M_मोजणी; ++i)
    {
	temp = crypt_get_header_len(i);
	जर (temp > max)
	    max = temp;
    }
    परत max;
}
#endif

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    शून्य
crypt_set_cm_option(buf_T *buf, पूर्णांक method_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[method_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    पूर्णांक
crypt_self_test(शून्य)
{
    पूर्णांक method_nr = crypt_get_method_nr(curbuf);

    जर (cryptmethods[method_nr].self_test_fn == रिक्त)
	परत OK;
    परत cryptmethods[method_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    पूर्णांक		method_nr,
    char_u	*किल्ली,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    जर (state == रिक्त)
	परत state;

    state->method_nr = method_nr;
    जर (cryptmethods[method_nr].init_fn(state, किल्ली, crypt_arg) == FAIL)
    {
	vim_free(state);
	परत रिक्त;
    }
    परत state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_header(
    पूर्णांक		method_nr,
    char_u	*किल्ली,
    char_u	*header)
{
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len = cryptmethods[method_nr].add_len;
    जर (arg.cat_salt_len > 0)
	arg.cat_salt = header + CRYPT_MAGIC_LEN;
    जर (arg.cat_seed_len > 0)
	arg.cat_seed = header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    जर (arg.cat_add_len > 0)
	arg.cat_add = header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

    परत crypt_create(method_nr, किल्ली, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(खाता *fp, char_u *किल्ली)
{
    पूर्णांक		method_nr;
    पूर्णांक		header_len;
    अक्षर	magic_buffer[CRYPT_MAGIC_LEN];
    char_u	*buffer;
    cryptstate_T *state;

    जर (ख_पढ़ो(magic_buffer, CRYPT_MAGIC_LEN, 1, fp) != 1)
	परत रिक्त;
    method_nr = crypt_method_nr_from_magic(magic_buffer, CRYPT_MAGIC_LEN);
    जर (method_nr < 0)
	परत रिक्त;

    header_len = crypt_get_header_len(method_nr);
    जर ((buffer = alloc(header_len)) == रिक्त)
	परत रिक्त;
    mch_memmove(buffer, magic_buffer, CRYPT_MAGIC_LEN);
    जर (header_len > CRYPT_MAGIC_LEN
	    && ख_पढ़ो(buffer + CRYPT_MAGIC_LEN,
				    header_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(buffer);
	परत रिक्त;
    }

    state = crypt_create_from_header(method_nr, किल्ली, buffer);
    vim_free(buffer);
    परत state;
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
    पूर्णांक	    method_nr,
    char_u  *किल्ली,
    char_u  **header,
    पूर्णांक	    *header_len)
{
    पूर्णांक	    len = crypt_get_header_len(method_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_FIELD(arg);
    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len  = cryptmethods[method_nr].add_len;
    arg.cat_init_from_file = FALSE;

    *header_len = len;
    *header = alloc(len);
    जर (*header == रिक्त)
	परत रिक्त;

    mch_memmove(*header, cryptmethods[method_nr].magic, CRYPT_MAGIC_LEN);
    जर (arg.cat_salt_len > 0 || arg.cat_seed_len > 0 || arg.cat_add_len > 0)
    {
	जर (arg.cat_salt_len > 0)
	    arg.cat_salt = *header + CRYPT_MAGIC_LEN;
	जर (arg.cat_seed_len > 0)
	    arg.cat_seed = *header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	जर (arg.cat_add_len > 0)
	    arg.cat_add = *header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#ifdef FEAT_SODIUM
	जर (sodium_init() >= 0)
	{
	    जर (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    जर (arg.cat_seed_len > 0)
		randombytes_buf(arg.cat_seed, arg.cat_seed_len);
	}
	अन्यथा
#endif
	    sha2_seed(arg.cat_salt, arg.cat_salt_len, arg.cat_seed, arg.cat_seed_len);
    }
    state = crypt_create(method_nr, किल्ली, &arg);
    जर (state == रिक्त)
	VIM_CLEAR(*header);
    परत state;
}

/*
 * Free the crypt state.
 */
    शून्य
crypt_free_state(cryptstate_T *state)
{
#ifdef FEAT_SODIUM
    जर (crypt_method_is_sodium(state->method_nr))
    {
	sodium_munlock(((sodium_state_T *)state->method_state)->किल्ली,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->method_state, आकार(sodium_state_T));
	sodium_free(state->method_state);
    }
    अन्यथा
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
    दीर्घ
crypt_encode_alloc(
    cryptstate_T *state,
    char_u	*from,
    माप_प्रकार	len,
    char_u	**newptr,
    पूर्णांक		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    जर (method->encode_buffer_fn != रिक्त)
	// Has buffer function, pass through.
	परत method->encode_buffer_fn(state, from, len, newptr, last);
    जर (len == 0)
	// Not buffering, just return EOF.
	परत (दीर्घ)len;

    *newptr = alloc(len + 50);
    जर (*newptr == रिक्त)
	परत -1;
    method->encode_fn(state, from, len, *newptr, last);
    परत (दीर्घ)len;
}

/*
 * Decrypt "ptr[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    दीर्घ
crypt_decode_alloc(
    cryptstate_T *state,
    char_u	*ptr,
    दीर्घ	len,
    char_u      **newptr,
    पूर्णांक		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    जर (method->decode_buffer_fn != रिक्त)
	// Has buffer function, pass through.
	परत method->decode_buffer_fn(state, ptr, len, newptr, last);

    जर (len == 0)
	// Not buffering, just return EOF.
	परत len;

    *newptr = alloc(len);
    जर (*newptr == रिक्त)
	परत -1;
    method->decode_fn(state, ptr, len, *newptr, last);
    परत len;
}
#endif

/*
 * Encrypting "from[len]" into "to[len]".
 */
    शून्य
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    माप_प्रकार	len,
    char_u	*to,
    पूर्णांक		last)
{
    cryptmethods[state->method_nr].encode_fn(state, from, len, to, last);
}

#जर 0  // unused
/*
 * decrypting "from[len]" into "to[len]".
 */
    शून्य
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    माप_प्रकार	len,
    char_u	*to,
    पूर्णांक		last)
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
    माप_प्रकार	len,
    पूर्णांक		last)
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
    माप_प्रकार	len,
    पूर्णांक		last)
{
    cryptmethods[state->method_nr].decode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    शून्य
crypt_free_किल्ली(char_u *किल्ली)
{
    // Create a safe memset which cannot be optimized away by compiler
    स्थिर शून्य *(* अस्थिर vim_memset_safe)(शून्य *s, पूर्णांक c, माप_प्रकार n) =
	स_रखो;
    जर (किल्ली != रिक्त)
    {
#ifdef FEAT_SODIUM
	जर (sodium_init() >= 0)
	    sodium_memzero(किल्ली, STRLEN(किल्ली));
	अन्यथा
#endif
	    vim_memset_safe(किल्ली, 0, STRLEN(किल्ली));
	vim_free(किल्ली);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    शून्य
crypt_check_method(पूर्णांक method)
{
    जर (method < CRYPT_M_BF2 || method == CRYPT_M_SOD)
    {
	संदेश_scroll = TRUE;
	संदेश(_("Warning: Using a weak encryption method; see :help 'cm'"));
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
    पूर्णांक method = crypt_get_method_nr(curbuf);
    जर (crypt_method_is_sodium(method))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	set_option_मूल्य_give_err((char_u *)"swf", 0, रिक्त, OPT_LOCAL);
	संदेश_scroll = TRUE;
	संदेश(_("Note: Encryption of swapfile not supported, disabling swap file"));
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
crypt_get_किल्ली(
    पूर्णांक		store,
    पूर्णांक		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = रिक्त;
    पूर्णांक		round;

    साठी (round = 0; ; ++round)
    {
	cmdline_star = TRUE;
	cmdline_row = संदेश_row;
	p1 = getcmdline_prompt(NUL, round == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		रिक्त);
	cmdline_star = FALSE;

	जर (p1 == रिक्त)
	    थांबा;

	जर (round == twice)
	{
	    जर (p2 != रिक्त && STRCMP(p1, p2) != 0)
	    {
		संदेश(_("Keys don't match!"));
		crypt_free_किल्ली(p1);
		crypt_free_किल्ली(p2);
		p2 = रिक्त;
		round = -1;		// do it again
		सुरूठेवा;
	    }

	    जर (store)
	    {
		set_option_मूल्य_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_किल्ली(p1);
		p1 = curbuf->b_p_किल्ली;
		crypt_check_swapfile_curbuf();
	    }
	    थांबा;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    जर (!crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
    {
	जर (संदेश_didout)
	    संदेश_putchar('\n');
	need_wait_return = FALSE;
	संदेश_didout = FALSE;
    }

    crypt_free_किल्ली(p2);
    परत p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    शून्य
crypt_append_संदेश(
    buf_T *buf)
{
    जर (crypt_get_method_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    अन्यथा
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    स्थिर पूर्णांक
crypt_sodium_init_(
    cryptstate_T	*state UNUSED,
    char_u		*किल्ली UNUSED,
    crypt_arg_T		*arg UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    अचिन्हित अक्षर	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    पूर्णांक			retval = 0;
    अचिन्हित दीर्घ दीर्घ	opslimit;
    अचिन्हित दीर्घ दीर्घ	memlimit;
    पूर्णांक			alg;

    जर (sodium_init() < 0)
	परत FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(आकार(sodium_state_T));
    sodium_memzero(sd_state, आकार(sodium_state_T));

    जर ((state->method_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->method_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# जर 0
	// For testing
	जर (state->method_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	जर (crypto_pwhash(dkey, आकार(dkey), (स्थिरांक अक्षर *)किल्ली, STRLEN(किल्ली),
			  arg->cat_salt, opslimit, (माप_प्रकार)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    परत FAIL;
	}
	स_नकल(sd_state->किल्ली, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->किल्ली, crypto_box_SEEDBYTES);
	retval += sodium_mlock(किल्ली, STRLEN(किल्ली));

	जर (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    परत FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	जर (state->method_nr == CRYPT_M_SOD2 && arg->cat_add != रिक्त)
	{
	    char_u	buffer[20];
	    char_u	*p = buffer;
	    vim_memset(buffer, 0, 20);

	    crypt_long_long_to_char(opslimit, p);
	    p += आकार(opslimit);

	    crypt_long_long_to_char(memlimit, p);
	    p += आकार(memlimit);

	    crypt_int_to_char(alg, p);
	    स_नकल(arg->cat_add, buffer, आकार(opslimit) + आकार(memlimit) + आकार(alg));
	}
    }
    अन्यथा
    {
	char_u	buffer[20];
	char_u	*p = buffer;
	vim_memset(buffer, 0, 20);
	पूर्णांक	size = आकार(opslimit) +
	    आकार(memlimit) + आकार(alg);

	// Reading parameters from file
	जर (arg->cat_add_len < size)
	{
	    sodium_free(sd_state);
	    परत FAIL;
	}

	// derive the key from the file header
	स_नकल(p, arg->cat_add, size);
	arg->cat_add += size;

	opslimit = crypt_char_to_long_long(p);
	p += आकार(opslimit);
	memlimit = crypt_char_to_long_long(p);
	p += आकार(memlimit);
	alg = crypt_char_to_int(p);
	p += आकार(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(माप_प्रकार)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	जर (crypto_pwhash(dkey, आकार(dkey), (स्थिरांक अक्षर *)किल्ली, STRLEN(किल्ली),
			  arg->cat_salt, opslimit, (माप_प्रकार)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    परत FAIL;
	}
	स_नकल(sd_state->किल्ली, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->किल्ली, crypto_box_SEEDBYTES);
	retval += sodium_mlock(किल्ली, STRLEN(किल्ली));

	जर (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    परत FAIL;
	}
    }
    sd_state->मोजणी = 0;
    state->method_state = sd_state;

    परत OK;
#अन्यथा
    emsg(_(e_libsodium_not_built_in));
    परत FAIL;
#endif
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#जर 0  // Currently unused
    शून्य
crypt_sodium_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    माप_प्रकार	len UNUSED,
    char_u	*to UNUSED,
    पूर्णांक		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    अचिन्हित अक्षर  tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;

    जर (sod_st->मोजणी == 0)
    {
	जर (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_header));
	    परत;
	}
	crypto_secretstream_xchacha20poly1305_init_ढकला(&sod_st->state,
							      to, sod_st->किल्ली);
	to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    जर (sod_st->मोजणी && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_buffer));
	परत;
    }

    crypto_secretstream_xchacha20poly1305_ढकला(&sod_st->state, to, रिक्त,
						      from, len, रिक्त, 0, tag);

    sod_st->मोजणी++;
# endif
}
#endif

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#जर 0  // Currently unused
    शून्य
crypt_sodium_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    माप_प्रकार	len UNUSED,
    char_u	*to UNUSED,
    पूर्णांक		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    अचिन्हित अक्षर  tag;
    अचिन्हित दीर्घ दीर्घ buf_len;
    char_u *p1 = from;
    char_u *p2 = to;
    char_u *buf_out;

    जर (sod_st->मोजणी == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_header));
	परत;
    }

    buf_out = (char_u *)alloc(len);

    जर (buf_out == रिक्त)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	परत;
    }
    जर (sod_st->मोजणी == 0)
    {
	जर (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->किल्ली) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    जा fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	जर (p1 == p2)
	    to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    जर (sod_st->मोजणी && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_buffer));
	जा fail;
    }
    जर (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_out, &buf_len, &tag, from, len, रिक्त, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	जा fail;
    }
    sod_st->मोजणी++;

    जर (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	जा fail;
    }
    जर (p1 == p2)
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
    स्थिर दीर्घ
crypt_sodium_buffer_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    माप_प्रकार	len UNUSED,
    char_u	**buf_out UNUSED,
    पूर्णांक		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    अचिन्हित दीर्घ दीर्घ	out_len;
    char_u		*ptr;
    अचिन्हित अक्षर	tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;
    पूर्णांक			length;
    sodium_state_T	*sod_st = state->method_state;
    पूर्णांक			first = (sod_st->मोजणी == 0);

    length = (पूर्णांक)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (first ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_out = alloc_clear(length);
    जर (*buf_out == रिक्त)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	परत -1;
    }
    ptr = *buf_out;

    जर (first)
    {
	crypto_secretstream_xchacha20poly1305_init_ढकला(&sod_st->state,
		ptr, sod_st->किल्ली);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_ढकला(&sod_st->state, ptr,
	    &out_len, from, len, रिक्त, 0, tag);

    sod_st->मोजणी++;
    परत out_len + (first
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#अन्यथा
    परत -1;
#endif
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    स्थिर दीर्घ
crypt_sodium_buffer_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    माप_प्रकार	len UNUSED,
    char_u	**buf_out UNUSED,
    पूर्णांक		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    अचिन्हित अक्षर  tag;
    अचिन्हित दीर्घ दीर्घ out_len;

    जर (sod_st->मोजणी == 0
	    && state->method_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].add_len;

    *buf_out = alloc_clear(len);
    जर (*buf_out == रिक्त)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	परत -1;
    }

    जर (sod_st->मोजणी == 0)
    {
	जर (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->किल्ली) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    परत -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->मोजणी++;
    }
    जर (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_out, &out_len, &tag, from, len, रिक्त, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	परत -1;
    }

    जर (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
	emsg(_(e_libsodium_decryption_failed_premature));
    परत (दीर्घ) out_len;
#अन्यथा
    परत -1;
#endif
}

#जर defined(FEAT_SODIUM)
    शून्य
crypt_sodium_lock_किल्ली(char_u *किल्ली)
{
    जर (sodium_init() >= 0)
	sodium_mlock(किल्ली, STRLEN(किल्ली));
}

    पूर्णांक
crypt_sodium_munlock(शून्य *स्थिरांक addr, स्थिरांक माप_प्रकार len)
{
    परत sodium_munlock(addr, len);
}

    शून्य
crypt_sodium_randombytes_buf(शून्य *स्थिरांक buf, स्थिरांक माप_प्रकार size)
{
    randombytes_buf(buf, size);
}

    पूर्णांक
crypt_sodium_init(शून्य)
{
    परत sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(शून्य)
{
    परत randombytes_random();
}

# जर defined(FEAT_EVAL)
    स्थिर शून्य
crypt_sodium_report_hash_params(
	अचिन्हित दीर्घ दीर्घ opslimit,
	अचिन्हित दीर्घ दीर्घ ops_def,
	माप_प्रकार memlimit,
	माप_प्रकार mem_def,
	पूर्णांक alg,
	पूर्णांक alg_def)
{
    जर (p_verbose > 0)
    {
	verbose_enter();
	जर (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	अन्यथा
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	जर (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (अचिन्हित दीर्घ)memlimit);
	अन्यथा
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (अचिन्हित दीर्घ)memlimit);
	जर (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	अन्यथा
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    स्थिर शून्य
crypt_long_long_to_char(दीर्घ दीर्घ n, char_u *s)
{
    पूर्णांक i;
    साठी (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (अचिन्हित दीर्घ दीर्घ)n >> 8;
    }
}

    स्थिर शून्य
crypt_int_to_char(पूर्णांक n, char_u *s)
{
    पूर्णांक i;
    साठी (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (अचिन्हित दीर्घ दीर्घ)n >> 8;
    }
}

    स्थिर दीर्घ दीर्घ
crypt_char_to_long_long(char_u *s)
{
    अचिन्हित दीर्घ दीर्घ    retval = 0;
    पूर्णांक i;
    साठी (i = 7; i >= 0; i--)
    {
	जर (i == 7)
	    retval = s[i];
	अन्यथा
	    retval |= s[i];
	जर (i > 0)
	    retval <<= 8;
    }
    परत retval;
}

    स्थिर पूर्णांक
crypt_char_to_int(char_u *s)
{
    पूर्णांक retval = 0;
    पूर्णांक i;

    साठी (i = 3; i >= 0; i--)
    {
	जर (i == 3)
	    retval = s[i];
	अन्यथा
	    retval |= s[i];
	जर (i > 0)
	    retval <<= 8;
    }
    परत retval;
}
#endif

#endif // FEAT_CRYPT
