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

#إذا defined(FEAT_CRYPT)
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
    صحيح		دانه_len;	// length of seed, or 0 when not using seed
    صحيح		جمع_کن_len;	// additional length in the header needed for storing
				// custom data
#اگر_تعریف_شده CRYPT_NOT_INPLACE
    صحيح		کار_میکند_inplace;	// encryption/decryption can be done in-place
#پایان_اگر
    صحيح		کامل_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    صحيح (*self_test_fn)(فراغ);

    // Function pointer for initializing encryption/decryption.
    صحيح (* init_fn)(cryptstate_T *state, char_u *کلید, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    فراغ (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *مقصد, صحيح آخرین);
    فراغ (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *مقصد, صحيح آخرین);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    دراز (*encode_میانگیر_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, صحيح آخرین);
    دراز (*decode_میانگیر_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, صحيح آخرین);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    فراغ (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, صحيح آخرین);
    فراغ (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, صحيح آخرین);
} cryptmethod_T;

ساكن صحيح crypt_sodium_init_(cryptstate_T *state, char_u *کلید, crypt_arg_T *arg);
ساكن دراز crypt_sodium_میانگیر_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_بیرون, صحيح آخرین);
ساكن دراز crypt_sodium_میانگیر_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_بیرون, صحيح آخرین);
#إذا defined(FEAT_SODIUM)
ساكن فراغ crypt_long_long_مقصد_char(دراز دراز n, char_u *s);
ساكن فراغ crypt_int_مقصد_char(صحيح n, char_u *s);
ساكن دراز دراز crypt_char_مقصد_long_long(char_u *s);
ساكن صحيح crypt_char_مقصد_int(char_u *s);
#پایان_اگر
#إذا defined(FEAT_EVAL) && defined(FEAT_SODIUM)
ساكن فراغ crypt_sodium_report_hash_params(بیعلامت دراز دراز opslimit, بیعلامت دراز دراز ops_def, size_t memlimit, size_t mem_def, صحيح alg, صحيح alg_def);
#پایان_اگر

// index is method_nr of cryptstate_T, CRYPT_M_*
ساكن cryptmethod_T cryptmethods[CRYPT_M_شمار] = {
    // PK_Zip; very weak
    {
	"zip",
	"VimCrypt~01!",
	0,
	0,
	0,
#اگر_تعریف_شده CRYPT_NOT_INPLACE
	TRUE,
#پایان_اگر
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
#اگر_تعریف_شده CRYPT_NOT_INPLACE
	TRUE,
#پایان_اگر
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
#اگر_تعریف_شده CRYPT_NOT_INPLACE
	TRUE,
#پایان_اگر
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
#اگر_تعریف_شده FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#وإلا
	16,
#پایان_اگر
	8,
	0,
#اگر_تعریف_شده CRYPT_NOT_INPLACE
	FALSE,
#پایان_اگر
	FALSE,
	عدم,
	crypt_sodium_init_,
	عدم, عدم,
	crypt_sodium_میانگیر_encode, crypt_sodium_میانگیر_decode,
	عدم, عدم,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#اگر_تعریف_شده FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#وإلا
	16,
#پایان_اگر
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#اگر_تعریف_شده CRYPT_NOT_INPLACE
	FALSE,
#پایان_اگر
	FALSE,
	عدم,
	crypt_sodium_init_,
	عدم, عدم,
	crypt_sodium_میانگیر_encode, crypt_sodium_میانگیر_decode,
	عدم, عدم,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#إذا defined(FEAT_SODIUM)
تعريفنوع بنية {
    size_t	    شمار;
    بیعلامت حرف   کلید[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define بارگیری_dll vim_بارگیری_Lib
#   define نماد_from_dll Get_Proc_آدرس
#   define close_dll FreeLibrary
#   define بارگیری_dll_error GetWin32Error
#  وإلا
#   error Dynamic loading از libsodium is not supported لكل now.
//#   define HINSTANCE void*
//#   define SODIUM_PROC void*
//#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
//#   define symbol_from_dll dlsym
//#   define close_dll dlclose
//#   define load_dll_error dlerror
#  endif

#  define sodium_init	    بارگیری_sodium
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

ساكن صحيح (*dll_sodium_init)(فراغ) = عدم;
ساكن فراغ (*dll_sodium_free)(فراغ *) = عدم;
ساكن فراغ *(*dll_sodium_malloc)(ثابت size_t) = عدم;
ساكن فراغ (*dll_sodium_memzero)(فراغ * ثابت, ثابت size_t) = عدم;
ساكن صحيح (*dll_sodium_mlock)(فراغ * ثابت, ثابت size_t) = عدم;
ساكن صحيح (*dll_sodium_munlock)(فراغ * ثابت, ثابت size_t) = عدم;
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_init_افزودن)
   (crypto_secretstream_xchacha20poly1305_state *state,
    بیعلامت حرف [],
    ثابت بیعلامت حرف []) = عدم;
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_افزودن)
   (crypto_secretstream_xchacha20poly1305_state *state,
    بیعلامت حرف *c, بیعلامت دراز دراز *clen_p,
    ثابت بیعلامت حرف *m, بیعلامت دراز دراز mlen,
    ثابت بیعلامت حرف *ad, بیعلامت دراز دراز adlen, بیعلامت حرف برچسب)
    = عدم;
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ثابت بیعلامت حرف [],
    ثابت بیعلامت حرف []) = عدم;
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    بیعلامت حرف *m, بیعلامت دراز دراز *mlen_p, بیعلامت حرف *برچسب_p,
    ثابت بیعلامت حرف *c, بیعلامت دراز دراز clen,
    ثابت بیعلامت حرف *ad, بیعلامت دراز دراز adlen) = عدم;
ساكن صحيح (*dll_crypto_pwhash)(بیعلامت حرف * ثابت بیرون,
    بیعلامت دراز دراز outlen,
    ثابت حرف * ثابت passwd, بیعلامت دراز دراز passwdlen,
    ثابت بیعلامت حرف * ثابت salt,
    بیعلامت دراز دراز opslimit, size_t memlimit, صحيح alg)
    = عدم;
ساكن فراغ (*dll_randombytes_buf)(فراغ * ثابت buf, ثابت size_t اندازه);
ساكن uint32_t (*dll_randombytes_random)(فراغ);

ساكن بنية {
    ثابت حرف *name;
    SODIUM_PROC *ptr;
} sodium_funcname_جدول[] = {
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
    {عدم, عدم}
};

    ساكن صحيح
sodium_runtime_link_init(صحيح verbose)
{
    ساكن HINSTANCE hsodium = عدم;
    ثابت حرف *libname = DYNAMIC_SODIUM_DLL;
    صحيح i;

    إذا (hsodium != عدم)
	أرجع OK;

    hsodium = بارگیری_dll(libname);
    إذا (hsodium == عدم)
    {
	إذا (verbose)
	    semsg(_(e_could_not_بارگیری_library_str_str), libname, بارگیری_dll_error());
	أرجع FAIL;
    }

    لكل (i = 0; sodium_funcname_جدول[i].ptr; ++i)
    {
	إذا ((*sodium_funcname_جدول[i].ptr = نماد_from_dll(hsodium,
			sodium_funcname_جدول[i].name)) == عدم)
	{
	    close_dll(hsodium);
	    hsodium = عدم;
	    إذا (verbose)
		semsg(_(e_could_not_بارگیری_library_function_str), sodium_funcname_جدول[i].name);
	    أرجع FAIL;
	}
    }
    أرجع OK;
}

    ساكن صحيح
بارگیری_sodium(فراغ)
{
    إذا (sodium_runtime_link_init(TRUE) == FAIL)
	أرجع -1;
    أرجع dll_sodium_init();
}
# endif

# إذا defined(DYNAMIC_SODIUM)
    صحيح
sodium_enabled(صحيح verbose)
{
    أرجع sodium_runtime_link_init(verbose) == OK;
}
# endif
#پایان_اگر

#تعریف_کن CRYPT_MAGIC_LEN	12	// cannot change
ساكن ثابت حرف crypt_magic_سر[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    صحيح
crypt_روش_nr_from_name(char_u *name)
{
    صحيح i;

    لكل (i = 0; i < CRYPT_M_شمار; ++i)
	إذا (STRCMP(name, cryptmethods[i].name) == 0)
	    أرجع i;
    أرجع 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    صحيح
crypt_روش_nr_from_magic(حرف *ptr, صحيح len)
{
    صحيح i;

    إذا (len < CRYPT_MAGIC_LEN)
	أرجع -1;

    لكل (i = 0; i < CRYPT_M_شمار; i++)
	إذا (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    أرجع i;

    i = (صحيح)STRLEN(crypt_magic_سر);
    إذا (len >= i && memcmp(ptr, crypt_magic_سر, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_روش));

    أرجع -1;
}

#اگر_تعریف_شده CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    صحيح
crypt_کار_میکند_inplace(cryptstate_T *state)
{
    أرجع cryptmethods[state->روش_nr].کار_میکند_inplace;
}
#پایان_اگر

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    صحيح
crypt_get_روش_nr(buf_T *buf)
{
    أرجع crypt_روش_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    صحيح
crypt_روش_is_sodium(صحيح روش)
{
    أرجع روش == CRYPT_M_SOD || روش == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    صحيح
crypt_کامل_undofile(صحيح روش_nr)
{
    أرجع cryptmethods[روش_nr].کامل_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    صحيح
crypt_get_سرآیند_len(صحيح روش_nr)
{
    أرجع CRYPT_MAGIC_LEN
	+ cryptmethods[روش_nr].salt_len
	+ cryptmethods[روش_nr].دانه_len
	+ cryptmethods[روش_nr].جمع_کن_len;
}


#إذا defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    صحيح
crypt_get_max_سرآیند_len(فراغ)
{
    صحيح i;
    صحيح max = 0;
    صحيح موقت = 0;

    لكل (i = 0; i < CRYPT_M_شمار; ++i)
    {
	موقت = crypt_get_سرآیند_len(i);
	إذا (موقت > max)
	    max = موقت;
    }
    أرجع max;
}
#پایان_اگر

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    فراغ
crypt_مجموعه_cm_option(buf_T *buf, صحيح روش_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[روش_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    صحيح
crypt_self_test(فراغ)
{
    صحيح روش_nr = crypt_get_روش_nr(curbuf);

    إذا (cryptmethods[روش_nr].self_test_fn == عدم)
	أرجع OK;
    أرجع cryptmethods[روش_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    صحيح		روش_nr,
    char_u	*کلید,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    إذا (state == عدم)
	أرجع state;

    state->روش_nr = روش_nr;
    إذا (cryptmethods[روش_nr].init_fn(state, کلید, crypt_arg) == FAIL)
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
crypt_create_from_سرآیند(
    صحيح		روش_nr,
    char_u	*کلید,
    char_u	*سرآیند)
{
    crypt_arg_T arg;

    CLEAR_فیلد(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[روش_nr].salt_len;
    arg.cat_دانه_len = cryptmethods[روش_nr].دانه_len;
    arg.cat_جمع_کن_len = cryptmethods[روش_nr].جمع_کن_len;
    إذا (arg.cat_salt_len > 0)
	arg.cat_salt = سرآیند + CRYPT_MAGIC_LEN;
    إذا (arg.cat_دانه_len > 0)
	arg.cat_دانه = سرآیند + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    إذا (arg.cat_جمع_کن_len > 0)
	arg.cat_جمع_کن = سرآیند + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_دانه_len;

    أرجع crypt_create(روش_nr, کلید, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *کلید)
{
    صحيح		روش_nr;
    صحيح		سرآیند_len;
    حرف	magic_میانگیر[CRYPT_MAGIC_LEN];
    char_u	*میانگیر;
    cryptstate_T *state;

    إذا (fread(magic_میانگیر, CRYPT_MAGIC_LEN, 1, fp) != 1)
	أرجع عدم;
    روش_nr = crypt_روش_nr_from_magic(magic_میانگیر, CRYPT_MAGIC_LEN);
    إذا (روش_nr < 0)
	أرجع عدم;

    سرآیند_len = crypt_get_سرآیند_len(روش_nr);
    إذا ((میانگیر = alloc(سرآیند_len)) == عدم)
	أرجع عدم;
    mch_memmove(میانگیر, magic_میانگیر, CRYPT_MAGIC_LEN);
    إذا (سرآیند_len > CRYPT_MAGIC_LEN
	    && fread(میانگیر + CRYPT_MAGIC_LEN,
				    سرآیند_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(میانگیر);
	أرجع عدم;
    }

    state = crypt_create_from_سرآیند(روش_nr, کلید, میانگیر);
    vim_free(میانگیر);
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
    صحيح	    روش_nr,
    char_u  *کلید,
    char_u  **سرآیند,
    صحيح	    *سرآیند_len)
{
    صحيح	    len = crypt_get_سرآیند_len(روش_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_فیلد(arg);
    arg.cat_salt_len = cryptmethods[روش_nr].salt_len;
    arg.cat_دانه_len = cryptmethods[روش_nr].دانه_len;
    arg.cat_جمع_کن_len  = cryptmethods[روش_nr].جمع_کن_len;
    arg.cat_init_from_file = FALSE;

    *سرآیند_len = len;
    *سرآیند = alloc(len);
    إذا (*سرآیند == عدم)
	أرجع عدم;

    mch_memmove(*سرآیند, cryptmethods[روش_nr].magic, CRYPT_MAGIC_LEN);
    إذا (arg.cat_salt_len > 0 || arg.cat_دانه_len > 0 || arg.cat_جمع_کن_len > 0)
    {
	إذا (arg.cat_salt_len > 0)
	    arg.cat_salt = *سرآیند + CRYPT_MAGIC_LEN;
	إذا (arg.cat_دانه_len > 0)
	    arg.cat_دانه = *سرآیند + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	إذا (arg.cat_جمع_کن_len > 0)
	    arg.cat_جمع_کن = *سرآیند + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_دانه_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#اگر_تعریف_شده FEAT_SODIUM
	إذا (sodium_init() >= 0)
	{
	    إذا (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    إذا (arg.cat_دانه_len > 0)
		randombytes_buf(arg.cat_دانه, arg.cat_دانه_len);
	}
	وإلا
#پایان_اگر
	    sha2_دانه(arg.cat_salt, arg.cat_salt_len, arg.cat_دانه, arg.cat_دانه_len);
    }
    state = crypt_create(روش_nr, کلید, &arg);
    إذا (state == عدم)
	VIM_CLEAR(*سرآیند);
    أرجع state;
}

/*
 * Free the crypt state.
 */
    فراغ
crypt_free_state(cryptstate_T *state)
{
#اگر_تعریف_شده FEAT_SODIUM
    إذا (crypt_روش_is_sodium(state->روش_nr))
    {
	sodium_munlock(((sodium_state_T *)state->روش_state)->کلید,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->روش_state, حجم(sodium_state_T));
	sodium_free(state->روش_state);
    }
    وإلا
#پایان_اگر
	vim_free(state->روش_state);
    vim_free(state);
}

#اگر_تعریف_شده CRYPT_NOT_INPLACE
/*
 * Encode "from[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    دراز
crypt_encode_alloc(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	**newptr,
    صحيح		آخرین)
{
    cryptmethod_T *روش = &cryptmethods[state->روش_nr];

    إذا (روش->encode_میانگیر_fn != عدم)
	// Has buffer function, pass through.
	أرجع روش->encode_میانگیر_fn(state, from, len, newptr, آخرین);
    إذا (len == 0)
	// Not buffering, just return EOF.
	أرجع (دراز)len;

    *newptr = alloc(len + 50);
    إذا (*newptr == عدم)
	أرجع -1;
    روش->encode_fn(state, from, len, *newptr, آخرین);
    أرجع (دراز)len;
}

/*
 * Decrypt "ptr[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    دراز
crypt_decode_alloc(
    cryptstate_T *state,
    char_u	*ptr,
    دراز	len,
    char_u      **newptr,
    صحيح		آخرین)
{
    cryptmethod_T *روش = &cryptmethods[state->روش_nr];

    إذا (روش->decode_میانگیر_fn != عدم)
	// Has buffer function, pass through.
	أرجع روش->decode_میانگیر_fn(state, ptr, len, newptr, آخرین);

    إذا (len == 0)
	// Not buffering, just return EOF.
	أرجع len;

    *newptr = alloc(len);
    إذا (*newptr == عدم)
	أرجع -1;
    روش->decode_fn(state, ptr, len, *newptr, آخرین);
    أرجع len;
}
#پایان_اگر

/*
 * Encrypting "from[len]" into "to[len]".
 */
    فراغ
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*مقصد,
    صحيح		آخرین)
{
    cryptmethods[state->روش_nr].encode_fn(state, from, len, مقصد, آخرین);
}

#إذا 0  //استفادهنشده
/*
 * decrypting "from[len]" into "to[len]".
 */
    فراغ
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*مقصد,
    صحيح		آخرین)
{
    cryptmethods[state->روش_nr].decode_fn(state, from, len, مقصد, آخرین);
}
#پایان_اگر

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    فراغ
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    صحيح		آخرین)
{
    cryptmethods[state->روش_nr].encode_inplace_fn(state, buf, len,
								    buf, آخرین);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    فراغ
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    صحيح		آخرین)
{
    cryptmethods[state->روش_nr].decode_inplace_fn(state, buf, len,
								    buf, آخرین);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    فراغ
crypt_free_کلید(char_u *کلید)
{
    // Create a safe memset which cannot be optimized away by compiler
    ساكن فراغ *(* متطاير vim_memset_امن)(فراغ *s, صحيح c, size_t n) =
	memset;
    إذا (کلید != عدم)
    {
#اگر_تعریف_شده FEAT_SODIUM
	إذا (sodium_init() >= 0)
	    sodium_memzero(کلید, STRLEN(کلید));
	وإلا
#پایان_اگر
	    vim_memset_امن(کلید, 0, STRLEN(کلید));
	vim_free(کلید);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    فراغ
crypt_check_روش(صحيح روش)
{
    إذا (روش < CRYPT_M_BF2 || روش == CRYPT_M_SOD)
    {
	پیام_scroll = TRUE;
	پیام(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    فراغ
crypt_check_swapfile_curbuf(فراغ)
{
#اگر_تعریف_شده FEAT_SODIUM
    صحيح روش = crypt_get_روش_nr(curbuf);
    إذا (crypt_روش_is_sodium(روش))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	مجموعه_option_مقدار_give_err((char_u *)"swf", 0, عدم, OPT_LOCAL);
	پیام_scroll = TRUE;
	پیام(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#پایان_اگر
}

    فراغ
crypt_check_current_روش(فراغ)
{
    crypt_check_روش(crypt_get_روش_nr(curbuf));
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
    صحيح		store,
    صحيح		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = عدم;
    صحيح		دور;

    لكل (دور = 0; ; ++دور)
    {
	cmdline_ستاره = TRUE;
	cmdline_ردیف = پیام_ردیف;
	p1 = getcmdline_prompt(NUL, دور == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		عدم);
	cmdline_ستاره = FALSE;

	إذا (p1 == عدم)
	    اكسر;

	إذا (دور == twice)
	{
	    إذا (p2 != عدم && STRCMP(p1, p2) != 0)
	    {
		پیام(_("Keys don't match!"));
		crypt_free_کلید(p1);
		crypt_free_کلید(p2);
		p2 = عدم;
		دور = -1;		// do it again
		استمر;
	    }

	    إذا (store)
	    {
		مجموعه_option_مقدار_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_کلید(p1);
		p1 = curbuf->b_p_کلید;
		crypt_check_swapfile_curbuf();
	    }
	    اكسر;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    إذا (!crypt_روش_is_sodium(crypt_get_روش_nr(curbuf)))
    {
	إذا (پیام_didout)
	    پیام_putchar('\n');
	need_wait_return = FALSE;
	پیام_didout = FALSE;
    }

    crypt_free_کلید(p2);
    أرجع p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    فراغ
crypt_append_پیام(
    buf_T *buf)
{
    إذا (crypt_get_روش_nr(buf) == 0)
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
    cryptstate_T	*state استفادهنشده,
    char_u		*کلید استفادهنشده,
    crypt_arg_T		*arg استفادهنشده)
{
#اگر_تعریف_شده FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    بیعلامت حرف	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    صحيح			retval = 0;
    بیعلامت دراز دراز	opslimit;
    بیعلامت دراز دراز	memlimit;
    صحيح			alg;

    إذا (sodium_init() < 0)
	أرجع FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(حجم(sodium_state_T));
    sodium_memzero(sd_state, حجم(sodium_state_T));

    إذا ((state->روش_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->روش_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# إذا 0
	// For testing
	إذا (state->روش_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	إذا (crypto_pwhash(dkey, حجم(dkey), (ثابت حرف *)کلید, STRLEN(کلید),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
	کپی_حافظه(sd_state->کلید, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->کلید, crypto_box_SEEDBYTES);
	retval += sodium_mlock(کلید, STRLEN(کلید));

	إذا (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	إذا (state->روش_nr == CRYPT_M_SOD2 && arg->cat_جمع_کن != عدم)
	{
	    char_u	میانگیر[20];
	    char_u	*p = میانگیر;
	    vim_memset(میانگیر, 0, 20);

	    crypt_long_long_مقصد_char(opslimit, p);
	    p += حجم(opslimit);

	    crypt_long_long_مقصد_char(memlimit, p);
	    p += حجم(memlimit);

	    crypt_int_مقصد_char(alg, p);
	    کپی_حافظه(arg->cat_جمع_کن, میانگیر, حجم(opslimit) + حجم(memlimit) + حجم(alg));
	}
    }
    وإلا
    {
	char_u	میانگیر[20];
	char_u	*p = میانگیر;
	vim_memset(میانگیر, 0, 20);
	صحيح	اندازه = حجم(opslimit) +
	    حجم(memlimit) + حجم(alg);

	// Reading parameters from file
	إذا (arg->cat_جمع_کن_len < اندازه)
	{
	    sodium_free(sd_state);
	    أرجع FAIL;
	}

	// derive the key from the file header
	کپی_حافظه(p, arg->cat_جمع_کن, اندازه);
	arg->cat_جمع_کن += اندازه;

	opslimit = crypt_char_مقصد_long_long(p);
	p += حجم(opslimit);
	memlimit = crypt_char_مقصد_long_long(p);
	p += حجم(memlimit);
	alg = crypt_char_مقصد_int(p);
	p += حجم(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	إذا (crypto_pwhash(dkey, حجم(dkey), (ثابت حرف *)کلید, STRLEN(کلید),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
	کپی_حافظه(sd_state->کلید, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->کلید, crypto_box_SEEDBYTES);
	retval += sodium_mlock(کلید, STRLEN(کلید));

	إذا (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
    }
    sd_state->شمار = 0;
    state->روش_state = sd_state;

    أرجع OK;
#وإلا
    emsg(_(e_libsodium_not_built_درون));
    أرجع FAIL;
#پایان_اگر
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#إذا 0  // Currently unused
    فراغ
crypt_sodium_encode(
    cryptstate_T *state استفادهنشده,
    char_u	*from استفادهنشده,
    size_t	len استفادهنشده,
    char_u	*مقصد استفادهنشده,
    صحيح		آخرین استفادهنشده)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->روش_state;
    بیعلامت حرف  برچسب = آخرین
			? crypto_secretstream_xchacha20poly1305_برچسب_FINAL  : 0;

    إذا (sod_st->شمار == 0)
    {
	إذا (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_سرآیند));
	    أرجع;
	}
	crypto_secretstream_xchacha20poly1305_init_افزودن(&sod_st->state,
							      مقصد, sod_st->کلید);
	مقصد += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    إذا (sod_st->شمار && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_میانگیر));
	أرجع;
    }

    crypto_secretstream_xchacha20poly1305_افزودن(&sod_st->state, مقصد, عدم,
						      from, len, عدم, 0, برچسب);

    sod_st->شمار++;
# endif
}
#پایان_اگر

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#إذا 0  // Currently unused
    فراغ
crypt_sodium_decode(
    cryptstate_T *state استفادهنشده,
    char_u	*from استفادهنشده,
    size_t	len استفادهنشده,
    char_u	*مقصد استفادهنشده,
    صحيح		آخرین استفادهنشده)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->روش_state;
    بیعلامت حرف  برچسب;
    بیعلامت دراز دراز buf_len;
    char_u *p1 = from;
    char_u *p2 = مقصد;
    char_u *buf_بیرون;

    إذا (sod_st->شمار == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_سرآیند));
	أرجع;
    }

    buf_بیرون = (char_u *)alloc(len);

    إذا (buf_بیرون == عدم)
    {
	emsg(_(e_libsodium_cannot_allocate_میانگیر));
	أرجع;
    }
    إذا (sod_st->شمار == 0)
    {
	إذا (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->کلید) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_سرآیند_incomplete));
	    اذهب fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	إذا (p1 == p2)
	    مقصد += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    إذا (sod_st->شمار && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_میانگیر));
	اذهب fail;
    }
    إذا (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_بیرون, &buf_len, &برچسب, from, len, عدم, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	اذهب fail;
    }
    sod_st->شمار++;

    إذا (برچسب == crypto_secretstream_xchacha20poly1305_برچسب_FINAL && !آخرین)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	اذهب fail;
    }
    إذا (p1 == p2)
	mch_memmove(p2, buf_بیرون, buf_len);

fail:
    vim_free(buf_بیرون);
# endif
}
#پایان_اگر

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ساكن دراز
crypt_sodium_میانگیر_encode(
    cryptstate_T *state استفادهنشده,
    char_u	*from استفادهنشده,
    size_t	len استفادهنشده,
    char_u	**buf_بیرون استفادهنشده,
    صحيح		آخرین استفادهنشده)
{
#اگر_تعریف_شده FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    بیعلامت دراز دراز	بیرون_len;
    char_u		*ptr;
    بیعلامت حرف	برچسب = آخرین
			? crypto_secretstream_xchacha20poly1305_برچسب_FINAL  : 0;
    صحيح			درازی;
    sodium_state_T	*sod_st = state->روش_state;
    صحيح			اولی = (sod_st->شمار == 0);

    درازی = (صحيح)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (اولی ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_بیرون = alloc_clear(درازی);
    إذا (*buf_بیرون == عدم)
    {
	emsg(_(e_libsodium_cannot_allocate_میانگیر));
	أرجع -1;
    }
    ptr = *buf_بیرون;

    إذا (اولی)
    {
	crypto_secretstream_xchacha20poly1305_init_افزودن(&sod_st->state,
		ptr, sod_st->کلید);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_افزودن(&sod_st->state, ptr,
	    &بیرون_len, from, len, عدم, 0, برچسب);

    sod_st->شمار++;
    أرجع بیرون_len + (اولی
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#وإلا
    أرجع -1;
#پایان_اگر
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ساكن دراز
crypt_sodium_میانگیر_decode(
    cryptstate_T *state استفادهنشده,
    char_u	*from استفادهنشده,
    size_t	len استفادهنشده,
    char_u	**buf_بیرون استفادهنشده,
    صحيح		آخرین استفادهنشده)
{
#اگر_تعریف_شده FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->روش_state;
    بیعلامت حرف  برچسب;
    بیعلامت دراز دراز بیرون_len;

    إذا (sod_st->شمار == 0
	    && state->روش_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].جمع_کن_len;

    *buf_بیرون = alloc_clear(len);
    إذا (*buf_بیرون == عدم)
    {
	emsg(_(e_libsodium_cannot_allocate_میانگیر));
	أرجع -1;
    }

    إذا (sod_st->شمار == 0)
    {
	إذا (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->کلید) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_سرآیند_incomplete));
	    أرجع -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->شمار++;
    }
    إذا (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_بیرون, &بیرون_len, &برچسب, from, len, عدم, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	أرجع -1;
    }

    إذا (برچسب == crypto_secretstream_xchacha20poly1305_برچسب_FINAL && !آخرین)
	emsg(_(e_libsodium_decryption_failed_premature));
    أرجع (دراز) بیرون_len;
#وإلا
    أرجع -1;
#پایان_اگر
}

#إذا defined(FEAT_SODIUM)
    فراغ
crypt_sodium_قفل_کلید(char_u *کلید)
{
    إذا (sodium_init() >= 0)
	sodium_mlock(کلید, STRLEN(کلید));
}

    صحيح
crypt_sodium_munlock(فراغ *ثابت addr, ثابت size_t len)
{
    أرجع sodium_munlock(addr, len);
}

    فراغ
crypt_sodium_randombytes_buf(فراغ *ثابت buf, ثابت size_t اندازه)
{
    randombytes_buf(buf, اندازه);
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

# إذا defined(FEAT_EVAL)
    ساكن فراغ
crypt_sodium_report_hash_params(
	بیعلامت دراز دراز opslimit,
	بیعلامت دراز دراز ops_def,
	size_t memlimit,
	size_t mem_def,
	صحيح alg,
	صحيح alg_def)
{
    إذا (p_verbose > 0)
    {
	verbose_enter();
	إذا (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	وإلا
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	إذا (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (بیعلامت دراز)memlimit);
	وإلا
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (بیعلامت دراز)memlimit);
	إذا (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	وإلا
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    ساكن فراغ
crypt_long_long_مقصد_char(دراز دراز n, char_u *s)
{
    صحيح i;
    لكل (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (بیعلامت دراز دراز)n >> 8;
    }
}

    ساكن فراغ
crypt_int_مقصد_char(صحيح n, char_u *s)
{
    صحيح i;
    لكل (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (بیعلامت دراز دراز)n >> 8;
    }
}

    ساكن دراز دراز
crypt_char_مقصد_long_long(char_u *s)
{
    بیعلامت دراز دراز    retval = 0;
    صحيح i;
    لكل (i = 7; i >= 0; i--)
    {
	إذا (i == 7)
	    retval = s[i];
	وإلا
	    retval |= s[i];
	إذا (i > 0)
	    retval <<= 8;
    }
    أرجع retval;
}

    ساكن صحيح
crypt_char_مقصد_int(char_u *s)
{
    صحيح retval = 0;
    صحيح i;

    لكل (i = 3; i >= 0; i--)
    {
	إذا (i == 3)
	    retval = s[i];
	وإلا
	    retval |= s[i];
	إذا (i > 0)
	    retval <<= 8;
    }
    أرجع retval;
}
#پایان_اگر

#پایان_اگر // FEAT_CRYPT
