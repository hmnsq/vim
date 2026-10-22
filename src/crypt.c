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
    صحيح		تخم_len;	// length of seed, or 0 when not using seed
    صحيح		جمع_کړه_len;	// additional length in the header needed for storing
				// custom data
#که_تعریف_شوی CRYPT_NOT_INPLACE
    صحيح		کار_کوي_inplace;	// encryption/decryption can be done in-place
#د_که_پای
    صحيح		ټولیز_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    صحيح (*self_test_fn)(فراغ);

    // Function pointer for initializing encryption/decryption.
    صحيح (* init_fn)(cryptstate_T *state, char_u *کلی, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    فراغ (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *منزل, صحيح وروستی);
    فراغ (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *منزل, صحيح وروستی);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    اوږد (*encode_بفر_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, صحيح وروستی);
    اوږد (*decode_بفر_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, صحيح وروستی);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    فراغ (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, صحيح وروستی);
    فراغ (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, صحيح وروستی);
} cryptmethod_T;

ساكن صحيح crypt_sodium_init_(cryptstate_T *state, char_u *کلی, crypt_arg_T *arg);
ساكن اوږد crypt_sodium_بفر_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_بهر, صحيح وروستی);
ساكن اوږد crypt_sodium_بفر_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_بهر, صحيح وروستی);
#إذا defined(FEAT_SODIUM)
ساكن فراغ crypt_long_long_منزل_char(اوږد اوږد n, char_u *s);
ساكن فراغ crypt_int_منزل_char(صحيح n, char_u *s);
ساكن اوږد اوږد crypt_char_منزل_long_long(char_u *s);
ساكن صحيح crypt_char_منزل_int(char_u *s);
#د_که_پای
#إذا defined(FEAT_EVAL) && defined(FEAT_SODIUM)
ساكن فراغ crypt_sodium_report_hash_params(بینښه اوږد اوږد opslimit, بینښه اوږد اوږد ops_def, size_t memlimit, size_t mem_def, صحيح alg, صحيح alg_def);
#د_که_پای

// index is method_nr of cryptstate_T, CRYPT_M_*
ساكن cryptmethod_T cryptmethods[CRYPT_M_شمېر] = {
    // PK_Zip; very weak
    {
	"zip",
	"VimCrypt~01!",
	0,
	0,
	0,
#که_تعریف_شوی CRYPT_NOT_INPLACE
	TRUE,
#د_که_پای
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
#که_تعریف_شوی CRYPT_NOT_INPLACE
	TRUE,
#د_که_پای
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
#که_تعریف_شوی CRYPT_NOT_INPLACE
	TRUE,
#د_که_پای
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
#که_تعریف_شوی FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#وإلا
	16,
#د_که_پای
	8,
	0,
#که_تعریف_شوی CRYPT_NOT_INPLACE
	FALSE,
#د_که_پای
	FALSE,
	عدم,
	crypt_sodium_init_,
	عدم, عدم,
	crypt_sodium_بفر_encode, crypt_sodium_بفر_decode,
	عدم, عدم,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#که_تعریف_شوی FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#وإلا
	16,
#د_که_پای
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#که_تعریف_شوی CRYPT_NOT_INPLACE
	FALSE,
#د_که_پای
	FALSE,
	عدم,
	crypt_sodium_init_,
	عدم, عدم,
	crypt_sodium_بفر_encode, crypt_sodium_بفر_decode,
	عدم, عدم,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#إذا defined(FEAT_SODIUM)
تعريفنوع بنية {
    size_t	    شمېر;
    بینښه حرف   کلی[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define بارول_dll vim_بارول_Lib
#   define نښه_from_dll Get_Proc_پته
#   define close_dll FreeLibrary
#   define بارول_dll_error GetWin32Error
#  وإلا
#   error Dynamic loading د libsodium is not supported لكل now.
//#   define HINSTANCE void*
//#   define SODIUM_PROC void*
//#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
//#   define symbol_from_dll dlsym
//#   define close_dll dlclose
//#   define load_dll_error dlerror
#  endif

#  define sodium_init	    بارول_sodium
#  define sodium_free	    dll_sodium_free
#  define sodium_malloc	    dll_sodium_malloc
#  define sodium_memzero    dll_sodium_memzero
#  define sodium_mlock	    dll_sodium_mlock
#  define sodium_munlock    dll_sodium_munlock
#  define crypto_secretstream_xchacha20poly1305_init_ټېل \
    dll_crypto_secretstream_xchacha20poly1305_init_ټېل
#  define crypto_secretstream_xchacha20poly1305_ټېل \
    dll_crypto_secretstream_xchacha20poly1305_ټېل
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
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_init_ټېل)
   (crypto_secretstream_xchacha20poly1305_state *state,
    بینښه حرف [],
    ثابت بینښه حرف []) = عدم;
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_ټېل)
   (crypto_secretstream_xchacha20poly1305_state *state,
    بینښه حرف *c, بینښه اوږد اوږد *clen_p,
    ثابت بینښه حرف *m, بینښه اوږد اوږد mlen,
    ثابت بینښه حرف *ad, بینښه اوږد اوږد adlen, بینښه حرف نښان)
    = عدم;
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ثابت بینښه حرف [],
    ثابت بینښه حرف []) = عدم;
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    بینښه حرف *m, بینښه اوږد اوږد *mlen_p, بینښه حرف *نښان_p,
    ثابت بینښه حرف *c, بینښه اوږد اوږد clen,
    ثابت بینښه حرف *ad, بینښه اوږد اوږد adlen) = عدم;
ساكن صحيح (*dll_crypto_pwhash)(بینښه حرف * ثابت بهر,
    بینښه اوږد اوږد outlen,
    ثابت حرف * ثابت passwd, بینښه اوږد اوږد passwdlen,
    ثابت بینښه حرف * ثابت salt,
    بینښه اوږد اوږد opslimit, size_t memlimit, صحيح alg)
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
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_ټېل},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_ټېل},
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

    hsodium = بارول_dll(libname);
    إذا (hsodium == عدم)
    {
	إذا (verbose)
	    semsg(_(e_could_not_بارول_library_str_str), libname, بارول_dll_error());
	أرجع FAIL;
    }

    لكل (i = 0; sodium_funcname_جدول[i].ptr; ++i)
    {
	إذا ((*sodium_funcname_جدول[i].ptr = نښه_from_dll(hsodium,
			sodium_funcname_جدول[i].name)) == عدم)
	{
	    close_dll(hsodium);
	    hsodium = عدم;
	    إذا (verbose)
		semsg(_(e_could_not_بارول_library_function_str), sodium_funcname_جدول[i].name);
	    أرجع FAIL;
	}
    }
    أرجع OK;
}

    ساكن صحيح
بارول_sodium(فراغ)
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
#د_که_پای

#تعریف_کړه CRYPT_MAGIC_LEN	12	// cannot change
ساكن ثابت حرف crypt_magic_سر[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    صحيح
crypt_لاره_nr_from_name(char_u *name)
{
    صحيح i;

    لكل (i = 0; i < CRYPT_M_شمېر; ++i)
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
crypt_لاره_nr_from_magic(حرف *ptr, صحيح len)
{
    صحيح i;

    إذا (len < CRYPT_MAGIC_LEN)
	أرجع -1;

    لكل (i = 0; i < CRYPT_M_شمېر; i++)
	إذا (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    أرجع i;

    i = (صحيح)STRLEN(crypt_magic_سر);
    إذا (len >= i && memcmp(ptr, crypt_magic_سر, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_لاره));

    أرجع -1;
}

#که_تعریف_شوی CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    صحيح
crypt_کار_کوي_inplace(cryptstate_T *state)
{
    أرجع cryptmethods[state->لاره_nr].کار_کوي_inplace;
}
#د_که_پای

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    صحيح
crypt_get_لاره_nr(buf_T *buf)
{
    أرجع crypt_لاره_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    صحيح
crypt_لاره_is_sodium(صحيح لاره)
{
    أرجع لاره == CRYPT_M_SOD || لاره == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    صحيح
crypt_ټولیز_undofile(صحيح لاره_nr)
{
    أرجع cryptmethods[لاره_nr].ټولیز_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    صحيح
crypt_get_سرلیک_len(صحيح لاره_nr)
{
    أرجع CRYPT_MAGIC_LEN
	+ cryptmethods[لاره_nr].salt_len
	+ cryptmethods[لاره_nr].تخم_len
	+ cryptmethods[لاره_nr].جمع_کړه_len;
}


#إذا defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    صحيح
crypt_get_max_سرلیک_len(فراغ)
{
    صحيح i;
    صحيح max = 0;
    صحيح لنډمهاله = 0;

    لكل (i = 0; i < CRYPT_M_شمېر; ++i)
    {
	لنډمهاله = crypt_get_سرلیک_len(i);
	إذا (لنډمهاله > max)
	    max = لنډمهاله;
    }
    أرجع max;
}
#د_که_پای

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    فراغ
crypt_سېټ_cm_option(buf_T *buf, صحيح لاره_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[لاره_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    صحيح
crypt_self_test(فراغ)
{
    صحيح لاره_nr = crypt_get_لاره_nr(curbuf);

    إذا (cryptmethods[لاره_nr].self_test_fn == عدم)
	أرجع OK;
    أرجع cryptmethods[لاره_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    صحيح		لاره_nr,
    char_u	*کلی,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    إذا (state == عدم)
	أرجع state;

    state->لاره_nr = لاره_nr;
    إذا (cryptmethods[لاره_nr].init_fn(state, کلی, crypt_arg) == FAIL)
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
crypt_create_from_سرلیک(
    صحيح		لاره_nr,
    char_u	*کلی,
    char_u	*سرلیک)
{
    crypt_arg_T arg;

    CLEAR_ډګر(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[لاره_nr].salt_len;
    arg.cat_تخم_len = cryptmethods[لاره_nr].تخم_len;
    arg.cat_جمع_کړه_len = cryptmethods[لاره_nr].جمع_کړه_len;
    إذا (arg.cat_salt_len > 0)
	arg.cat_salt = سرلیک + CRYPT_MAGIC_LEN;
    إذا (arg.cat_تخم_len > 0)
	arg.cat_تخم = سرلیک + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    إذا (arg.cat_جمع_کړه_len > 0)
	arg.cat_جمع_کړه = سرلیک + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_تخم_len;

    أرجع crypt_create(لاره_nr, کلی, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *کلی)
{
    صحيح		لاره_nr;
    صحيح		سرلیک_len;
    حرف	magic_بفر[CRYPT_MAGIC_LEN];
    char_u	*بفر;
    cryptstate_T *state;

    إذا (fread(magic_بفر, CRYPT_MAGIC_LEN, 1, fp) != 1)
	أرجع عدم;
    لاره_nr = crypt_لاره_nr_from_magic(magic_بفر, CRYPT_MAGIC_LEN);
    إذا (لاره_nr < 0)
	أرجع عدم;

    سرلیک_len = crypt_get_سرلیک_len(لاره_nr);
    إذا ((بفر = alloc(سرلیک_len)) == عدم)
	أرجع عدم;
    mch_memmove(بفر, magic_بفر, CRYPT_MAGIC_LEN);
    إذا (سرلیک_len > CRYPT_MAGIC_LEN
	    && fread(بفر + CRYPT_MAGIC_LEN,
				    سرلیک_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(بفر);
	أرجع عدم;
    }

    state = crypt_create_from_سرلیک(لاره_nr, کلی, بفر);
    vim_free(بفر);
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
    صحيح	    لاره_nr,
    char_u  *کلی,
    char_u  **سرلیک,
    صحيح	    *سرلیک_len)
{
    صحيح	    len = crypt_get_سرلیک_len(لاره_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_ډګر(arg);
    arg.cat_salt_len = cryptmethods[لاره_nr].salt_len;
    arg.cat_تخم_len = cryptmethods[لاره_nr].تخم_len;
    arg.cat_جمع_کړه_len  = cryptmethods[لاره_nr].جمع_کړه_len;
    arg.cat_init_from_file = FALSE;

    *سرلیک_len = len;
    *سرلیک = alloc(len);
    إذا (*سرلیک == عدم)
	أرجع عدم;

    mch_memmove(*سرلیک, cryptmethods[لاره_nr].magic, CRYPT_MAGIC_LEN);
    إذا (arg.cat_salt_len > 0 || arg.cat_تخم_len > 0 || arg.cat_جمع_کړه_len > 0)
    {
	إذا (arg.cat_salt_len > 0)
	    arg.cat_salt = *سرلیک + CRYPT_MAGIC_LEN;
	إذا (arg.cat_تخم_len > 0)
	    arg.cat_تخم = *سرلیک + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	إذا (arg.cat_جمع_کړه_len > 0)
	    arg.cat_جمع_کړه = *سرلیک + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_تخم_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#که_تعریف_شوی FEAT_SODIUM
	إذا (sodium_init() >= 0)
	{
	    إذا (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    إذا (arg.cat_تخم_len > 0)
		randombytes_buf(arg.cat_تخم, arg.cat_تخم_len);
	}
	وإلا
#د_که_پای
	    sha2_تخم(arg.cat_salt, arg.cat_salt_len, arg.cat_تخم, arg.cat_تخم_len);
    }
    state = crypt_create(لاره_nr, کلی, &arg);
    إذا (state == عدم)
	VIM_CLEAR(*سرلیک);
    أرجع state;
}

/*
 * Free the crypt state.
 */
    فراغ
crypt_free_state(cryptstate_T *state)
{
#که_تعریف_شوی FEAT_SODIUM
    إذا (crypt_لاره_is_sodium(state->لاره_nr))
    {
	sodium_munlock(((sodium_state_T *)state->لاره_state)->کلی,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->لاره_state, حجم(sodium_state_T));
	sodium_free(state->لاره_state);
    }
    وإلا
#د_که_پای
	vim_free(state->لاره_state);
    vim_free(state);
}

#که_تعریف_شوی CRYPT_NOT_INPLACE
/*
 * Encode "from[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    اوږد
crypt_encode_alloc(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	**newptr,
    صحيح		وروستی)
{
    cryptmethod_T *لاره = &cryptmethods[state->لاره_nr];

    إذا (لاره->encode_بفر_fn != عدم)
	// Has buffer function, pass through.
	أرجع لاره->encode_بفر_fn(state, from, len, newptr, وروستی);
    إذا (len == 0)
	// Not buffering, just return EOF.
	أرجع (اوږد)len;

    *newptr = alloc(len + 50);
    إذا (*newptr == عدم)
	أرجع -1;
    لاره->encode_fn(state, from, len, *newptr, وروستی);
    أرجع (اوږد)len;
}

/*
 * Decrypt "ptr[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    اوږد
crypt_decode_alloc(
    cryptstate_T *state,
    char_u	*ptr,
    اوږد	len,
    char_u      **newptr,
    صحيح		وروستی)
{
    cryptmethod_T *لاره = &cryptmethods[state->لاره_nr];

    إذا (لاره->decode_بفر_fn != عدم)
	// Has buffer function, pass through.
	أرجع لاره->decode_بفر_fn(state, ptr, len, newptr, وروستی);

    إذا (len == 0)
	// Not buffering, just return EOF.
	أرجع len;

    *newptr = alloc(len);
    إذا (*newptr == عدم)
	أرجع -1;
    لاره->decode_fn(state, ptr, len, *newptr, وروستی);
    أرجع len;
}
#د_که_پای

/*
 * Encrypting "from[len]" into "to[len]".
 */
    فراغ
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*منزل,
    صحيح		وروستی)
{
    cryptmethods[state->لاره_nr].encode_fn(state, from, len, منزل, وروستی);
}

#إذا 0  //ناکارول_شوی
/*
 * decrypting "from[len]" into "to[len]".
 */
    فراغ
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*منزل,
    صحيح		وروستی)
{
    cryptmethods[state->لاره_nr].decode_fn(state, from, len, منزل, وروستی);
}
#د_که_پای

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    فراغ
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    صحيح		وروستی)
{
    cryptmethods[state->لاره_nr].encode_inplace_fn(state, buf, len,
								    buf, وروستی);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    فراغ
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    صحيح		وروستی)
{
    cryptmethods[state->لاره_nr].decode_inplace_fn(state, buf, len,
								    buf, وروستی);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    فراغ
crypt_free_کلی(char_u *کلی)
{
    // Create a safe memset which cannot be optimized away by compiler
    ساكن فراغ *(* متطاير vim_memset_خوندي)(فراغ *s, صحيح c, size_t n) =
	memset;
    إذا (کلی != عدم)
    {
#که_تعریف_شوی FEAT_SODIUM
	إذا (sodium_init() >= 0)
	    sodium_memzero(کلی, STRLEN(کلی));
	وإلا
#د_که_پای
	    vim_memset_خوندي(کلی, 0, STRLEN(کلی));
	vim_free(کلی);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    فراغ
crypt_check_لاره(صحيح لاره)
{
    إذا (لاره < CRYPT_M_BF2 || لاره == CRYPT_M_SOD)
    {
	پیغام_scroll = TRUE;
	پیغام(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    فراغ
crypt_check_swapfile_curbuf(فراغ)
{
#که_تعریف_شوی FEAT_SODIUM
    صحيح لاره = crypt_get_لاره_nr(curbuf);
    إذا (crypt_لاره_is_sodium(لاره))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	سېټ_option_ارزښت_give_err((char_u *)"swf", 0, عدم, OPT_LOCAL);
	پیغام_scroll = TRUE;
	پیغام(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#د_که_پای
}

    فراغ
crypt_check_current_لاره(فراغ)
{
    crypt_check_لاره(crypt_get_لاره_nr(curbuf));
}

/*
 * Ask the user for a crypt key.
 * When "store" is TRUE, the new key is stored in the 'key' option, and the
 * 'key' option value is returned: Don't free it.
 * When "store" is FALSE, the typed key is returned in allocated memory.
 * Returns NULL on failure.
 */
    char_u *
crypt_get_کلی(
    صحيح		store,
    صحيح		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = عدم;
    صحيح		پړاو;

    لكل (پړاو = 0; ; ++پړاو)
    {
	cmdline_ستوری = TRUE;
	cmdline_لیکه = پیغام_لیکه;
	p1 = getcmdline_prompt(NUL, پړاو == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		عدم);
	cmdline_ستوری = FALSE;

	إذا (p1 == عدم)
	    اكسر;

	إذا (پړاو == twice)
	{
	    إذا (p2 != عدم && STRCMP(p1, p2) != 0)
	    {
		پیغام(_("Keys don't match!"));
		crypt_free_کلی(p1);
		crypt_free_کلی(p2);
		p2 = عدم;
		پړاو = -1;		// do it again
		استمر;
	    }

	    إذا (store)
	    {
		سېټ_option_ارزښت_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_کلی(p1);
		p1 = curbuf->b_p_کلی;
		crypt_check_swapfile_curbuf();
	    }
	    اكسر;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    إذا (!crypt_لاره_is_sodium(crypt_get_لاره_nr(curbuf)))
    {
	إذا (پیغام_didout)
	    پیغام_putchar('\n');
	need_wait_return = FALSE;
	پیغام_didout = FALSE;
    }

    crypt_free_کلی(p2);
    أرجع p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    فراغ
crypt_append_پیغام(
    buf_T *buf)
{
    إذا (crypt_get_لاره_nr(buf) == 0)
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
    cryptstate_T	*state ناکارول_شوی,
    char_u		*کلی ناکارول_شوی,
    crypt_arg_T		*arg ناکارول_شوی)
{
#که_تعریف_شوی FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    بینښه حرف	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    صحيح			retval = 0;
    بینښه اوږد اوږد	opslimit;
    بینښه اوږد اوږد	memlimit;
    صحيح			alg;

    إذا (sodium_init() < 0)
	أرجع FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(حجم(sodium_state_T));
    sodium_memzero(sd_state, حجم(sodium_state_T));

    إذا ((state->لاره_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->لاره_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# إذا 0
	// For testing
	إذا (state->لاره_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	إذا (crypto_pwhash(dkey, حجم(dkey), (ثابت حرف *)کلی, STRLEN(کلی),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
	حافظه_نقل(sd_state->کلی, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->کلی, crypto_box_SEEDBYTES);
	retval += sodium_mlock(کلی, STRLEN(کلی));

	إذا (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	إذا (state->لاره_nr == CRYPT_M_SOD2 && arg->cat_جمع_کړه != عدم)
	{
	    char_u	بفر[20];
	    char_u	*p = بفر;
	    vim_memset(بفر, 0, 20);

	    crypt_long_long_منزل_char(opslimit, p);
	    p += حجم(opslimit);

	    crypt_long_long_منزل_char(memlimit, p);
	    p += حجم(memlimit);

	    crypt_int_منزل_char(alg, p);
	    حافظه_نقل(arg->cat_جمع_کړه, بفر, حجم(opslimit) + حجم(memlimit) + حجم(alg));
	}
    }
    وإلا
    {
	char_u	بفر[20];
	char_u	*p = بفر;
	vim_memset(بفر, 0, 20);
	صحيح	اندازه = حجم(opslimit) +
	    حجم(memlimit) + حجم(alg);

	// Reading parameters from file
	إذا (arg->cat_جمع_کړه_len < اندازه)
	{
	    sodium_free(sd_state);
	    أرجع FAIL;
	}

	// derive the key from the file header
	حافظه_نقل(p, arg->cat_جمع_کړه, اندازه);
	arg->cat_جمع_کړه += اندازه;

	opslimit = crypt_char_منزل_long_long(p);
	p += حجم(opslimit);
	memlimit = crypt_char_منزل_long_long(p);
	p += حجم(memlimit);
	alg = crypt_char_منزل_int(p);
	p += حجم(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	إذا (crypto_pwhash(dkey, حجم(dkey), (ثابت حرف *)کلی, STRLEN(کلی),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
	حافظه_نقل(sd_state->کلی, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->کلی, crypto_box_SEEDBYTES);
	retval += sodium_mlock(کلی, STRLEN(کلی));

	إذا (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
    }
    sd_state->شمېر = 0;
    state->لاره_state = sd_state;

    أرجع OK;
#وإلا
    emsg(_(e_libsodium_not_built_دننه));
    أرجع FAIL;
#د_که_پای
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#إذا 0  // Currently unused
    فراغ
crypt_sodium_encode(
    cryptstate_T *state ناکارول_شوی,
    char_u	*from ناکارول_شوی,
    size_t	len ناکارول_شوی,
    char_u	*منزل ناکارول_شوی,
    صحيح		وروستی ناکارول_شوی)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->لاره_state;
    بینښه حرف  نښان = وروستی
			? crypto_secretstream_xchacha20poly1305_نښان_FINAL  : 0;

    إذا (sod_st->شمېر == 0)
    {
	إذا (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_سرلیک));
	    أرجع;
	}
	crypto_secretstream_xchacha20poly1305_init_ټېل(&sod_st->state,
							      منزل, sod_st->کلی);
	منزل += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    إذا (sod_st->شمېر && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_بفر));
	أرجع;
    }

    crypto_secretstream_xchacha20poly1305_ټېل(&sod_st->state, منزل, عدم,
						      from, len, عدم, 0, نښان);

    sod_st->شمېر++;
# endif
}
#د_که_پای

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#إذا 0  // Currently unused
    فراغ
crypt_sodium_decode(
    cryptstate_T *state ناکارول_شوی,
    char_u	*from ناکارول_شوی,
    size_t	len ناکارول_شوی,
    char_u	*منزل ناکارول_شوی,
    صحيح		وروستی ناکارول_شوی)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->لاره_state;
    بینښه حرف  نښان;
    بینښه اوږد اوږد buf_len;
    char_u *p1 = from;
    char_u *p2 = منزل;
    char_u *buf_بهر;

    إذا (sod_st->شمېر == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_سرلیک));
	أرجع;
    }

    buf_بهر = (char_u *)alloc(len);

    إذا (buf_بهر == عدم)
    {
	emsg(_(e_libsodium_cannot_allocate_بفر));
	أرجع;
    }
    إذا (sod_st->شمېر == 0)
    {
	إذا (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->کلی) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_سرلیک_incomplete));
	    اذهب fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	إذا (p1 == p2)
	    منزل += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    إذا (sod_st->شمېر && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_بفر));
	اذهب fail;
    }
    إذا (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_بهر, &buf_len, &نښان, from, len, عدم, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	اذهب fail;
    }
    sod_st->شمېر++;

    إذا (نښان == crypto_secretstream_xchacha20poly1305_نښان_FINAL && !وروستی)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	اذهب fail;
    }
    إذا (p1 == p2)
	mch_memmove(p2, buf_بهر, buf_len);

fail:
    vim_free(buf_بهر);
# endif
}
#د_که_پای

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ساكن اوږد
crypt_sodium_بفر_encode(
    cryptstate_T *state ناکارول_شوی,
    char_u	*from ناکارول_شوی,
    size_t	len ناکارول_شوی,
    char_u	**buf_بهر ناکارول_شوی,
    صحيح		وروستی ناکارول_شوی)
{
#که_تعریف_شوی FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    بینښه اوږد اوږد	بهر_len;
    char_u		*ptr;
    بینښه حرف	نښان = وروستی
			? crypto_secretstream_xchacha20poly1305_نښان_FINAL  : 0;
    صحيح			اوږدتیا;
    sodium_state_T	*sod_st = state->لاره_state;
    صحيح			لومړی = (sod_st->شمېر == 0);

    اوږدتیا = (صحيح)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (لومړی ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_بهر = alloc_clear(اوږدتیا);
    إذا (*buf_بهر == عدم)
    {
	emsg(_(e_libsodium_cannot_allocate_بفر));
	أرجع -1;
    }
    ptr = *buf_بهر;

    إذا (لومړی)
    {
	crypto_secretstream_xchacha20poly1305_init_ټېل(&sod_st->state,
		ptr, sod_st->کلی);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_ټېل(&sod_st->state, ptr,
	    &بهر_len, from, len, عدم, 0, نښان);

    sod_st->شمېر++;
    أرجع بهر_len + (لومړی
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#وإلا
    أرجع -1;
#د_که_پای
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ساكن اوږد
crypt_sodium_بفر_decode(
    cryptstate_T *state ناکارول_شوی,
    char_u	*from ناکارول_شوی,
    size_t	len ناکارول_شوی,
    char_u	**buf_بهر ناکارول_شوی,
    صحيح		وروستی ناکارول_شوی)
{
#که_تعریف_شوی FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->لاره_state;
    بینښه حرف  نښان;
    بینښه اوږد اوږد بهر_len;

    إذا (sod_st->شمېر == 0
	    && state->لاره_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].جمع_کړه_len;

    *buf_بهر = alloc_clear(len);
    إذا (*buf_بهر == عدم)
    {
	emsg(_(e_libsodium_cannot_allocate_بفر));
	أرجع -1;
    }

    إذا (sod_st->شمېر == 0)
    {
	إذا (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->کلی) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_سرلیک_incomplete));
	    أرجع -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->شمېر++;
    }
    إذا (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_بهر, &بهر_len, &نښان, from, len, عدم, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	أرجع -1;
    }

    إذا (نښان == crypto_secretstream_xchacha20poly1305_نښان_FINAL && !وروستی)
	emsg(_(e_libsodium_decryption_failed_premature));
    أرجع (اوږد) بهر_len;
#وإلا
    أرجع -1;
#د_که_پای
}

#إذا defined(FEAT_SODIUM)
    فراغ
crypt_sodium_قلف_کلی(char_u *کلی)
{
    إذا (sodium_init() >= 0)
	sodium_mlock(کلی, STRLEN(کلی));
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
	بینښه اوږد اوږد opslimit,
	بینښه اوږد اوږد ops_def,
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
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (بینښه اوږد)memlimit);
	وإلا
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (بینښه اوږد)memlimit);
	إذا (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	وإلا
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    ساكن فراغ
crypt_long_long_منزل_char(اوږد اوږد n, char_u *s)
{
    صحيح i;
    لكل (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (بینښه اوږد اوږد)n >> 8;
    }
}

    ساكن فراغ
crypt_int_منزل_char(صحيح n, char_u *s)
{
    صحيح i;
    لكل (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (بینښه اوږد اوږد)n >> 8;
    }
}

    ساكن اوږد اوږد
crypt_char_منزل_long_long(char_u *s)
{
    بینښه اوږد اوږد    retval = 0;
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
crypt_char_منزل_int(char_u *s)
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
#د_که_پای

#د_که_پای // FEAT_CRYPT
