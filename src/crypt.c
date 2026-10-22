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

قسمتعریف ساخت {
    حرف	*name;		// encryption name as used in 'cryptmethod'
    حرف	*magic;		// magic bytes stored in file header
    صحیح		salt_len;	// length of salt, or 0 when not using salt
    صحیح		بیج_len;	// length of seed, or 0 when not using seed
    صحیح		جوڑو_len;	// additional length in the header needed for storing
				// custom data
#اگر_تعریف_شدہ CRYPT_NOT_INPLACE
    صحیح		چلتا_inplace;	// encryption/decryption can be done in-place
#اگر_ختم
    صحیح		پورا_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    صحیح (*self_test_fn)(خالی);

    // Function pointer for initializing encryption/decryption.
    صحیح (* init_fn)(cryptstate_T *state, char_u *کلید, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    خالی (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *مقصد, صحیح آخری);
    خالی (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *مقصد, صحیح آخری);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    طویل (*encode_بفر_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, صحیح آخری);
    طویل (*decode_بفر_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, صحیح آخری);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    خالی (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, صحیح آخری);
    خالی (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, صحیح آخری);
} cryptmethod_T;

ساکن صحیح crypt_sodium_init_(cryptstate_T *state, char_u *کلید, crypt_arg_T *arg);
ساکن طویل crypt_sodium_بفر_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_برآمد, صحیح آخری);
ساکن طویل crypt_sodium_بفر_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_برآمد, صحیح آخری);
#اگر defined(FEAT_SODIUM)
ساکن خالی crypt_long_long_مقصد_char(طویل طویل n, char_u *s);
ساکن خالی crypt_int_مقصد_char(صحیح n, char_u *s);
ساکن طویل طویل crypt_char_مقصد_long_long(char_u *s);
ساکن صحیح crypt_char_مقصد_int(char_u *s);
#اگر_ختم
#اگر defined(FEAT_EVAL) && defined(FEAT_SODIUM)
ساکن خالی crypt_sodium_report_hash_params(غیرعلامتی طویل طویل opslimit, غیرعلامتی طویل طویل ops_def, size_t memlimit, size_t mem_def, صحیح alg, صحیح alg_def);
#اگر_ختم

// index is method_nr of cryptstate_T, CRYPT_M_*
ساکن cryptmethod_T cryptmethods[CRYPT_M_گنتی] = {
    // PK_Zip; very weak
    {
	"zip",
	"VimCrypt~01!",
	0,
	0,
	0,
#اگر_تعریف_شدہ CRYPT_NOT_INPLACE
	TRUE,
#اگر_ختم
	FALSE,
	تہی,
	crypt_zip_init,
	crypt_zip_encode, crypt_zip_decode,
	تہی, تہی,
	crypt_zip_encode, crypt_zip_decode,
    },

    // Blowfish/CFB + SHA-256 custom key derivation; implementation issues.
    {
	"blowfish",
	"VimCrypt~02!",
	8,
	8,
	0,
#اگر_تعریف_شدہ CRYPT_NOT_INPLACE
	TRUE,
#اگر_ختم
	FALSE,
	blowfish_self_test,
	crypt_blowfish_init,
	crypt_blowfish_encode, crypt_blowfish_decode,
	تہی, تہی,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // Blowfish/CFB + SHA-256 custom key derivation; fixed.
    {
	"blowfish2",
	"VimCrypt~03!",
	8,
	8,
	0,
#اگر_تعریف_شدہ CRYPT_NOT_INPLACE
	TRUE,
#اگر_ختم
	TRUE,
	blowfish_self_test,
	crypt_blowfish_init,
	crypt_blowfish_encode, crypt_blowfish_decode,
	تہی, تہی,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // XChaCha20 using libsodium; implementation issues
    {
	"xchacha20",
	"VimCrypt~04!",
#اگر_تعریف_شدہ FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#ورنہ
	16,
#اگر_ختم
	8,
	0,
#اگر_تعریف_شدہ CRYPT_NOT_INPLACE
	FALSE,
#اگر_ختم
	FALSE,
	تہی,
	crypt_sodium_init_,
	تہی, تہی,
	crypt_sodium_بفر_encode, crypt_sodium_بفر_decode,
	تہی, تہی,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#اگر_تعریف_شدہ FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#ورنہ
	16,
#اگر_ختم
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#اگر_تعریف_شدہ CRYPT_NOT_INPLACE
	FALSE,
#اگر_ختم
	FALSE,
	تہی,
	crypt_sodium_init_,
	تہی, تہی,
	crypt_sodium_بفر_encode, crypt_sodium_بفر_decode,
	تہی, تہی,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#اگر defined(FEAT_SODIUM)
قسمتعریف ساخت {
    size_t	    گنتی;
    غیرعلامتی حرف   کلید[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define بھرو_dll vim_بھرو_Lib
#   define علامت_from_dll Get_Proc_پتہ
#   define close_dll FreeLibrary
#   define بھرو_dll_error GetWin32Error
#  ورنہ
#   error Dynamic loading کا libsodium is not supported برائے now.
//#   define HINSTANCE void*
//#   define SODIUM_PROC void*
//#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
//#   define symbol_from_dll dlsym
//#   define close_dll dlclose
//#   define load_dll_error dlerror
#  endif

#  define sodium_init	    بھرو_sodium
#  define sodium_free	    dll_sodium_free
#  define sodium_malloc	    dll_sodium_malloc
#  define sodium_memzero    dll_sodium_memzero
#  define sodium_mlock	    dll_sodium_mlock
#  define sodium_munlock    dll_sodium_munlock
#  define crypto_secretstream_xchacha20poly1305_init_ڈالو \
    dll_crypto_secretstream_xchacha20poly1305_init_ڈالو
#  define crypto_secretstream_xchacha20poly1305_ڈالو \
    dll_crypto_secretstream_xchacha20poly1305_ڈالو
#  define crypto_secretstream_xchacha20poly1305_init_pull \
    dll_crypto_secretstream_xchacha20poly1305_init_pull
#  define crypto_secretstream_xchacha20poly1305_pull \
    dll_crypto_secretstream_xchacha20poly1305_pull
#  define crypto_pwhash	    dll_crypto_pwhash
#  define randombytes_buf   dll_randombytes_buf
#  define randombytes_random dll_randombytes_random

ساکن صحیح (*dll_sodium_init)(خالی) = تہی;
ساکن خالی (*dll_sodium_free)(خالی *) = تہی;
ساکن خالی *(*dll_sodium_malloc)(ثابت size_t) = تہی;
ساکن خالی (*dll_sodium_memzero)(خالی * ثابت, ثابت size_t) = تہی;
ساکن صحیح (*dll_sodium_mlock)(خالی * ثابت, ثابت size_t) = تہی;
ساکن صحیح (*dll_sodium_munlock)(خالی * ثابت, ثابت size_t) = تہی;
ساکن صحیح (*dll_crypto_secretstream_xchacha20poly1305_init_ڈالو)
   (crypto_secretstream_xchacha20poly1305_state *state,
    غیرعلامتی حرف [],
    ثابت غیرعلامتی حرف []) = تہی;
ساکن صحیح (*dll_crypto_secretstream_xchacha20poly1305_ڈالو)
   (crypto_secretstream_xchacha20poly1305_state *state,
    غیرعلامتی حرف *c, غیرعلامتی طویل طویل *clen_p,
    ثابت غیرعلامتی حرف *m, غیرعلامتی طویل طویل mlen,
    ثابت غیرعلامتی حرف *ad, غیرعلامتی طویل طویل adlen, غیرعلامتی حرف لیبل)
    = تہی;
ساکن صحیح (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ثابت غیرعلامتی حرف [],
    ثابت غیرعلامتی حرف []) = تہی;
ساکن صحیح (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    غیرعلامتی حرف *m, غیرعلامتی طویل طویل *mlen_p, غیرعلامتی حرف *لیبل_p,
    ثابت غیرعلامتی حرف *c, غیرعلامتی طویل طویل clen,
    ثابت غیرعلامتی حرف *ad, غیرعلامتی طویل طویل adlen) = تہی;
ساکن صحیح (*dll_crypto_pwhash)(غیرعلامتی حرف * ثابت برآمد,
    غیرعلامتی طویل طویل outlen,
    ثابت حرف * ثابت passwd, غیرعلامتی طویل طویل passwdlen,
    ثابت غیرعلامتی حرف * ثابت salt,
    غیرعلامتی طویل طویل opslimit, size_t memlimit, صحیح alg)
    = تہی;
ساکن خالی (*dll_randombytes_buf)(خالی * ثابت buf, ثابت size_t سائز);
ساکن uint32_t (*dll_randombytes_random)(خالی);

ساکن ساخت {
    ثابت حرف *name;
    SODIUM_PROC *ptr;
} sodium_funcname_جدول[] = {
    {"sodium_init", (SODIUM_PROC*)&dll_sodium_init},
    {"sodium_free", (SODIUM_PROC*)&dll_sodium_free},
    {"sodium_malloc", (SODIUM_PROC*)&dll_sodium_malloc},
    {"sodium_memzero", (SODIUM_PROC*)&dll_sodium_memzero},
    {"sodium_mlock", (SODIUM_PROC*)&dll_sodium_mlock},
    {"sodium_munlock", (SODIUM_PROC*)&dll_sodium_munlock},
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_ڈالو},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_ڈالو},
    {"crypto_secretstream_xchacha20poly1305_init_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_pull},
    {"crypto_secretstream_xchacha20poly1305_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_pull},
    {"crypto_pwhash", (SODIUM_PROC*)&dll_crypto_pwhash},
    {"randombytes_buf", (SODIUM_PROC*)&dll_randombytes_buf},
    {"randombytes_random", (SODIUM_PROC*)&dll_randombytes_random},
    {تہی, تہی}
};

    ساکن صحیح
sodium_runtime_link_init(صحیح verbose)
{
    ساکن HINSTANCE hsodium = تہی;
    ثابت حرف *libname = DYNAMIC_SODIUM_DLL;
    صحیح i;

    اگر (hsodium != تہی)
	واپس OK;

    hsodium = بھرو_dll(libname);
    اگر (hsodium == تہی)
    {
	اگر (verbose)
	    semsg(_(e_could_not_بھرو_library_str_str), libname, بھرو_dll_error());
	واپس FAIL;
    }

    برائے (i = 0; sodium_funcname_جدول[i].ptr; ++i)
    {
	اگر ((*sodium_funcname_جدول[i].ptr = علامت_from_dll(hsodium,
			sodium_funcname_جدول[i].name)) == تہی)
	{
	    close_dll(hsodium);
	    hsodium = تہی;
	    اگر (verbose)
		semsg(_(e_could_not_بھرو_library_function_str), sodium_funcname_جدول[i].name);
	    واپس FAIL;
	}
    }
    واپس OK;
}

    ساکن صحیح
بھرو_sodium(خالی)
{
    اگر (sodium_runtime_link_init(TRUE) == FAIL)
	واپس -1;
    واپس dll_sodium_init();
}
# endif

# اگر defined(DYNAMIC_SODIUM)
    صحیح
sodium_enabled(صحیح verbose)
{
    واپس sodium_runtime_link_init(verbose) == OK;
}
# endif
#اگر_ختم

#تعریف_کرو CRYPT_MAGIC_LEN	12	// cannot change
ساکن ثابت حرف crypt_magic_سر[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    صحیح
crypt_طریقہ_nr_from_name(char_u *name)
{
    صحیح i;

    برائے (i = 0; i < CRYPT_M_گنتی; ++i)
	اگر (STRCMP(name, cryptmethods[i].name) == 0)
	    واپس i;
    واپس 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    صحیح
crypt_طریقہ_nr_from_magic(حرف *ptr, صحیح len)
{
    صحیح i;

    اگر (len < CRYPT_MAGIC_LEN)
	واپس -1;

    برائے (i = 0; i < CRYPT_M_گنتی; i++)
	اگر (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    واپس i;

    i = (صحیح)STRLEN(crypt_magic_سر);
    اگر (len >= i && memcmp(ptr, crypt_magic_سر, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_طریقہ));

    واپس -1;
}

#اگر_تعریف_شدہ CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    صحیح
crypt_چلتا_inplace(cryptstate_T *state)
{
    واپس cryptmethods[state->طریقہ_nr].چلتا_inplace;
}
#اگر_ختم

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    صحیح
crypt_get_طریقہ_nr(buf_T *buf)
{
    واپس crypt_طریقہ_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    صحیح
crypt_طریقہ_is_sodium(صحیح طریقہ)
{
    واپس طریقہ == CRYPT_M_SOD || طریقہ == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    صحیح
crypt_پورا_undofile(صحیح طریقہ_nr)
{
    واپس cryptmethods[طریقہ_nr].پورا_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    صحیح
crypt_get_سرخی_len(صحیح طریقہ_nr)
{
    واپس CRYPT_MAGIC_LEN
	+ cryptmethods[طریقہ_nr].salt_len
	+ cryptmethods[طریقہ_nr].بیج_len
	+ cryptmethods[طریقہ_nr].جوڑو_len;
}


#اگر defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    صحیح
crypt_get_max_سرخی_len(خالی)
{
    صحیح i;
    صحیح max = 0;
    صحیح عارضی = 0;

    برائے (i = 0; i < CRYPT_M_گنتی; ++i)
    {
	عارضی = crypt_get_سرخی_len(i);
	اگر (عارضی > max)
	    max = عارضی;
    }
    واپس max;
}
#اگر_ختم

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    خالی
crypt_سیٹ_cm_option(buf_T *buf, صحیح طریقہ_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[طریقہ_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    صحیح
crypt_self_test(خالی)
{
    صحیح طریقہ_nr = crypt_get_طریقہ_nr(curbuf);

    اگر (cryptmethods[طریقہ_nr].self_test_fn == تہی)
	واپس OK;
    واپس cryptmethods[طریقہ_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    صحیح		طریقہ_nr,
    char_u	*کلید,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    اگر (state == تہی)
	واپس state;

    state->طریقہ_nr = طریقہ_nr;
    اگر (cryptmethods[طریقہ_nr].init_fn(state, کلید, crypt_arg) == FAIL)
    {
	vim_free(state);
	واپس تہی;
    }
    واپس state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_سرخی(
    صحیح		طریقہ_nr,
    char_u	*کلید,
    char_u	*سرخی)
{
    crypt_arg_T arg;

    CLEAR_میدان(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[طریقہ_nr].salt_len;
    arg.cat_بیج_len = cryptmethods[طریقہ_nr].بیج_len;
    arg.cat_جوڑو_len = cryptmethods[طریقہ_nr].جوڑو_len;
    اگر (arg.cat_salt_len > 0)
	arg.cat_salt = سرخی + CRYPT_MAGIC_LEN;
    اگر (arg.cat_بیج_len > 0)
	arg.cat_بیج = سرخی + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    اگر (arg.cat_جوڑو_len > 0)
	arg.cat_جوڑو = سرخی + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_بیج_len;

    واپس crypt_create(طریقہ_nr, کلید, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *کلید)
{
    صحیح		طریقہ_nr;
    صحیح		سرخی_len;
    حرف	magic_بفر[CRYPT_MAGIC_LEN];
    char_u	*بفر;
    cryptstate_T *state;

    اگر (fread(magic_بفر, CRYPT_MAGIC_LEN, 1, fp) != 1)
	واپس تہی;
    طریقہ_nr = crypt_طریقہ_nr_from_magic(magic_بفر, CRYPT_MAGIC_LEN);
    اگر (طریقہ_nr < 0)
	واپس تہی;

    سرخی_len = crypt_get_سرخی_len(طریقہ_nr);
    اگر ((بفر = alloc(سرخی_len)) == تہی)
	واپس تہی;
    mch_memmove(بفر, magic_بفر, CRYPT_MAGIC_LEN);
    اگر (سرخی_len > CRYPT_MAGIC_LEN
	    && fread(بفر + CRYPT_MAGIC_LEN,
				    سرخی_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(بفر);
	واپس تہی;
    }

    state = crypt_create_from_سرخی(طریقہ_nr, کلید, بفر);
    vim_free(بفر);
    واپس state;
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
    صحیح	    طریقہ_nr,
    char_u  *کلید,
    char_u  **سرخی,
    صحیح	    *سرخی_len)
{
    صحیح	    len = crypt_get_سرخی_len(طریقہ_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_میدان(arg);
    arg.cat_salt_len = cryptmethods[طریقہ_nr].salt_len;
    arg.cat_بیج_len = cryptmethods[طریقہ_nr].بیج_len;
    arg.cat_جوڑو_len  = cryptmethods[طریقہ_nr].جوڑو_len;
    arg.cat_init_from_file = FALSE;

    *سرخی_len = len;
    *سرخی = alloc(len);
    اگر (*سرخی == تہی)
	واپس تہی;

    mch_memmove(*سرخی, cryptmethods[طریقہ_nr].magic, CRYPT_MAGIC_LEN);
    اگر (arg.cat_salt_len > 0 || arg.cat_بیج_len > 0 || arg.cat_جوڑو_len > 0)
    {
	اگر (arg.cat_salt_len > 0)
	    arg.cat_salt = *سرخی + CRYPT_MAGIC_LEN;
	اگر (arg.cat_بیج_len > 0)
	    arg.cat_بیج = *سرخی + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	اگر (arg.cat_جوڑو_len > 0)
	    arg.cat_جوڑو = *سرخی + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_بیج_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#اگر_تعریف_شدہ FEAT_SODIUM
	اگر (sodium_init() >= 0)
	{
	    اگر (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    اگر (arg.cat_بیج_len > 0)
		randombytes_buf(arg.cat_بیج, arg.cat_بیج_len);
	}
	ورنہ
#اگر_ختم
	    sha2_بیج(arg.cat_salt, arg.cat_salt_len, arg.cat_بیج, arg.cat_بیج_len);
    }
    state = crypt_create(طریقہ_nr, کلید, &arg);
    اگر (state == تہی)
	VIM_CLEAR(*سرخی);
    واپس state;
}

/*
 * Free the crypt state.
 */
    خالی
crypt_free_state(cryptstate_T *state)
{
#اگر_تعریف_شدہ FEAT_SODIUM
    اگر (crypt_طریقہ_is_sodium(state->طریقہ_nr))
    {
	sodium_munlock(((sodium_state_T *)state->طریقہ_state)->کلید,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->طریقہ_state, حجم(sodium_state_T));
	sodium_free(state->طریقہ_state);
    }
    ورنہ
#اگر_ختم
	vim_free(state->طریقہ_state);
    vim_free(state);
}

#اگر_تعریف_شدہ CRYPT_NOT_INPLACE
/*
 * Encode "from[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    طویل
crypt_encode_alloc(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	**newptr,
    صحیح		آخری)
{
    cryptmethod_T *طریقہ = &cryptmethods[state->طریقہ_nr];

    اگر (طریقہ->encode_بفر_fn != تہی)
	// Has buffer function, pass through.
	واپس طریقہ->encode_بفر_fn(state, from, len, newptr, آخری);
    اگر (len == 0)
	// Not buffering, just return EOF.
	واپس (طویل)len;

    *newptr = alloc(len + 50);
    اگر (*newptr == تہی)
	واپس -1;
    طریقہ->encode_fn(state, from, len, *newptr, آخری);
    واپس (طویل)len;
}

/*
 * Decrypt "ptr[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    طویل
crypt_decode_alloc(
    cryptstate_T *state,
    char_u	*ptr,
    طویل	len,
    char_u      **newptr,
    صحیح		آخری)
{
    cryptmethod_T *طریقہ = &cryptmethods[state->طریقہ_nr];

    اگر (طریقہ->decode_بفر_fn != تہی)
	// Has buffer function, pass through.
	واپس طریقہ->decode_بفر_fn(state, ptr, len, newptr, آخری);

    اگر (len == 0)
	// Not buffering, just return EOF.
	واپس len;

    *newptr = alloc(len);
    اگر (*newptr == تہی)
	واپس -1;
    طریقہ->decode_fn(state, ptr, len, *newptr, آخری);
    واپس len;
}
#اگر_ختم

/*
 * Encrypting "from[len]" into "to[len]".
 */
    خالی
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*مقصد,
    صحیح		آخری)
{
    cryptmethods[state->طریقہ_nr].encode_fn(state, from, len, مقصد, آخری);
}

#اگر 0  //غیر_مستعمل
/*
 * decrypting "from[len]" into "to[len]".
 */
    خالی
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*مقصد,
    صحیح		آخری)
{
    cryptmethods[state->طریقہ_nr].decode_fn(state, from, len, مقصد, آخری);
}
#اگر_ختم

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    خالی
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    صحیح		آخری)
{
    cryptmethods[state->طریقہ_nr].encode_inplace_fn(state, buf, len,
								    buf, آخری);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    خالی
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    صحیح		آخری)
{
    cryptmethods[state->طریقہ_nr].decode_inplace_fn(state, buf, len,
								    buf, آخری);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    خالی
crypt_free_کلید(char_u *کلید)
{
    // Create a safe memset which cannot be optimized away by compiler
    ساکن خالی *(* متلون vim_memset_محفوظ)(خالی *s, صحیح c, size_t n) =
	memset;
    اگر (کلید != تہی)
    {
#اگر_تعریف_شدہ FEAT_SODIUM
	اگر (sodium_init() >= 0)
	    sodium_memzero(کلید, STRLEN(کلید));
	ورنہ
#اگر_ختم
	    vim_memset_محفوظ(کلید, 0, STRLEN(کلید));
	vim_free(کلید);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    خالی
crypt_check_طریقہ(صحیح طریقہ)
{
    اگر (طریقہ < CRYPT_M_BF2 || طریقہ == CRYPT_M_SOD)
    {
	پیغام_scroll = TRUE;
	پیغام(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    خالی
crypt_check_swapfile_curbuf(خالی)
{
#اگر_تعریف_شدہ FEAT_SODIUM
    صحیح طریقہ = crypt_get_طریقہ_nr(curbuf);
    اگر (crypt_طریقہ_is_sodium(طریقہ))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	سیٹ_option_قدر_give_err((char_u *)"swf", 0, تہی, OPT_LOCAL);
	پیغام_scroll = TRUE;
	پیغام(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#اگر_ختم
}

    خالی
crypt_check_current_طریقہ(خالی)
{
    crypt_check_طریقہ(crypt_get_طریقہ_nr(curbuf));
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
    char_u	*p1, *p2 = تہی;
    صحیح		باری;

    برائے (باری = 0; ; ++باری)
    {
	cmdline_ستارہ = TRUE;
	cmdline_صف = پیغام_صف;
	p1 = getcmdline_prompt(NUL, باری == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		تہی);
	cmdline_ستارہ = FALSE;

	اگر (p1 == تہی)
	    توڑو;

	اگر (باری == twice)
	{
	    اگر (p2 != تہی && STRCMP(p1, p2) != 0)
	    {
		پیغام(_("Keys don't match!"));
		crypt_free_کلید(p1);
		crypt_free_کلید(p2);
		p2 = تہی;
		باری = -1;		// do it again
		جاری;
	    }

	    اگر (store)
	    {
		سیٹ_option_قدر_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_کلید(p1);
		p1 = curbuf->b_p_کلید;
		crypt_check_swapfile_curbuf();
	    }
	    توڑو;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    اگر (!crypt_طریقہ_is_sodium(crypt_get_طریقہ_nr(curbuf)))
    {
	اگر (پیغام_didout)
	    پیغام_putchar('\n');
	need_wait_return = FALSE;
	پیغام_didout = FALSE;
    }

    crypt_free_کلید(p2);
    واپس p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    خالی
crypt_append_پیغام(
    buf_T *buf)
{
    اگر (crypt_get_طریقہ_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    ورنہ
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    ساکن صحیح
crypt_sodium_init_(
    cryptstate_T	*state غیر_مستعمل,
    char_u		*کلید غیر_مستعمل,
    crypt_arg_T		*arg غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    غیرعلامتی حرف	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    صحیح			retval = 0;
    غیرعلامتی طویل طویل	opslimit;
    غیرعلامتی طویل طویل	memlimit;
    صحیح			alg;

    اگر (sodium_init() < 0)
	واپس FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(حجم(sodium_state_T));
    sodium_memzero(sd_state, حجم(sodium_state_T));

    اگر ((state->طریقہ_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->طریقہ_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# اگر 0
	// For testing
	اگر (state->طریقہ_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	اگر (crypto_pwhash(dkey, حجم(dkey), (ثابت حرف *)کلید, STRLEN(کلید),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    واپس FAIL;
	}
	حافظہ_نقل(sd_state->کلید, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->کلید, crypto_box_SEEDBYTES);
	retval += sodium_mlock(کلید, STRLEN(کلید));

	اگر (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    واپس FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	اگر (state->طریقہ_nr == CRYPT_M_SOD2 && arg->cat_جوڑو != تہی)
	{
	    char_u	بفر[20];
	    char_u	*p = بفر;
	    vim_memset(بفر, 0, 20);

	    crypt_long_long_مقصد_char(opslimit, p);
	    p += حجم(opslimit);

	    crypt_long_long_مقصد_char(memlimit, p);
	    p += حجم(memlimit);

	    crypt_int_مقصد_char(alg, p);
	    حافظہ_نقل(arg->cat_جوڑو, بفر, حجم(opslimit) + حجم(memlimit) + حجم(alg));
	}
    }
    ورنہ
    {
	char_u	بفر[20];
	char_u	*p = بفر;
	vim_memset(بفر, 0, 20);
	صحیح	سائز = حجم(opslimit) +
	    حجم(memlimit) + حجم(alg);

	// Reading parameters from file
	اگر (arg->cat_جوڑو_len < سائز)
	{
	    sodium_free(sd_state);
	    واپس FAIL;
	}

	// derive the key from the file header
	حافظہ_نقل(p, arg->cat_جوڑو, سائز);
	arg->cat_جوڑو += سائز;

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

	اگر (crypto_pwhash(dkey, حجم(dkey), (ثابت حرف *)کلید, STRLEN(کلید),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    واپس FAIL;
	}
	حافظہ_نقل(sd_state->کلید, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->کلید, crypto_box_SEEDBYTES);
	retval += sodium_mlock(کلید, STRLEN(کلید));

	اگر (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    واپس FAIL;
	}
    }
    sd_state->گنتی = 0;
    state->طریقہ_state = sd_state;

    واپس OK;
#ورنہ
    emsg(_(e_libsodium_not_built_اندر));
    واپس FAIL;
#اگر_ختم
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#اگر 0  // Currently unused
    خالی
crypt_sodium_encode(
    cryptstate_T *state غیر_مستعمل,
    char_u	*from غیر_مستعمل,
    size_t	len غیر_مستعمل,
    char_u	*مقصد غیر_مستعمل,
    صحیح		آخری غیر_مستعمل)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->طریقہ_state;
    غیرعلامتی حرف  لیبل = آخری
			? crypto_secretstream_xchacha20poly1305_لیبل_FINAL  : 0;

    اگر (sod_st->گنتی == 0)
    {
	اگر (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_سرخی));
	    واپس;
	}
	crypto_secretstream_xchacha20poly1305_init_ڈالو(&sod_st->state,
							      مقصد, sod_st->کلید);
	مقصد += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    اگر (sod_st->گنتی && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_بفر));
	واپس;
    }

    crypto_secretstream_xchacha20poly1305_ڈالو(&sod_st->state, مقصد, تہی,
						      from, len, تہی, 0, لیبل);

    sod_st->گنتی++;
# endif
}
#اگر_ختم

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#اگر 0  // Currently unused
    خالی
crypt_sodium_decode(
    cryptstate_T *state غیر_مستعمل,
    char_u	*from غیر_مستعمل,
    size_t	len غیر_مستعمل,
    char_u	*مقصد غیر_مستعمل,
    صحیح		آخری غیر_مستعمل)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->طریقہ_state;
    غیرعلامتی حرف  لیبل;
    غیرعلامتی طویل طویل buf_len;
    char_u *p1 = from;
    char_u *p2 = مقصد;
    char_u *buf_برآمد;

    اگر (sod_st->گنتی == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_سرخی));
	واپس;
    }

    buf_برآمد = (char_u *)alloc(len);

    اگر (buf_برآمد == تہی)
    {
	emsg(_(e_libsodium_cannot_allocate_بفر));
	واپس;
    }
    اگر (sod_st->گنتی == 0)
    {
	اگر (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->کلید) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_سرخی_incomplete));
	    جاؤ fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	اگر (p1 == p2)
	    مقصد += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    اگر (sod_st->گنتی && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_بفر));
	جاؤ fail;
    }
    اگر (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_برآمد, &buf_len, &لیبل, from, len, تہی, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	جاؤ fail;
    }
    sod_st->گنتی++;

    اگر (لیبل == crypto_secretstream_xchacha20poly1305_لیبل_FINAL && !آخری)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	جاؤ fail;
    }
    اگر (p1 == p2)
	mch_memmove(p2, buf_برآمد, buf_len);

fail:
    vim_free(buf_برآمد);
# endif
}
#اگر_ختم

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ساکن طویل
crypt_sodium_بفر_encode(
    cryptstate_T *state غیر_مستعمل,
    char_u	*from غیر_مستعمل,
    size_t	len غیر_مستعمل,
    char_u	**buf_برآمد غیر_مستعمل,
    صحیح		آخری غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    غیرعلامتی طویل طویل	برآمد_len;
    char_u		*ptr;
    غیرعلامتی حرف	لیبل = آخری
			? crypto_secretstream_xchacha20poly1305_لیبل_FINAL  : 0;
    صحیح			لمبائی;
    sodium_state_T	*sod_st = state->طریقہ_state;
    صحیح			پہلا = (sod_st->گنتی == 0);

    لمبائی = (صحیح)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (پہلا ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_برآمد = alloc_clear(لمبائی);
    اگر (*buf_برآمد == تہی)
    {
	emsg(_(e_libsodium_cannot_allocate_بفر));
	واپس -1;
    }
    ptr = *buf_برآمد;

    اگر (پہلا)
    {
	crypto_secretstream_xchacha20poly1305_init_ڈالو(&sod_st->state,
		ptr, sod_st->کلید);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_ڈالو(&sod_st->state, ptr,
	    &برآمد_len, from, len, تہی, 0, لیبل);

    sod_st->گنتی++;
    واپس برآمد_len + (پہلا
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#ورنہ
    واپس -1;
#اگر_ختم
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ساکن طویل
crypt_sodium_بفر_decode(
    cryptstate_T *state غیر_مستعمل,
    char_u	*from غیر_مستعمل,
    size_t	len غیر_مستعمل,
    char_u	**buf_برآمد غیر_مستعمل,
    صحیح		آخری غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->طریقہ_state;
    غیرعلامتی حرف  لیبل;
    غیرعلامتی طویل طویل برآمد_len;

    اگر (sod_st->گنتی == 0
	    && state->طریقہ_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].جوڑو_len;

    *buf_برآمد = alloc_clear(len);
    اگر (*buf_برآمد == تہی)
    {
	emsg(_(e_libsodium_cannot_allocate_بفر));
	واپس -1;
    }

    اگر (sod_st->گنتی == 0)
    {
	اگر (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->کلید) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_سرخی_incomplete));
	    واپس -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->گنتی++;
    }
    اگر (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_برآمد, &برآمد_len, &لیبل, from, len, تہی, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	واپس -1;
    }

    اگر (لیبل == crypto_secretstream_xchacha20poly1305_لیبل_FINAL && !آخری)
	emsg(_(e_libsodium_decryption_failed_premature));
    واپس (طویل) برآمد_len;
#ورنہ
    واپس -1;
#اگر_ختم
}

#اگر defined(FEAT_SODIUM)
    خالی
crypt_sodium_تالا_کلید(char_u *کلید)
{
    اگر (sodium_init() >= 0)
	sodium_mlock(کلید, STRLEN(کلید));
}

    صحیح
crypt_sodium_munlock(خالی *ثابت addr, ثابت size_t len)
{
    واپس sodium_munlock(addr, len);
}

    خالی
crypt_sodium_randombytes_buf(خالی *ثابت buf, ثابت size_t سائز)
{
    randombytes_buf(buf, سائز);
}

    صحیح
crypt_sodium_init(خالی)
{
    واپس sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(خالی)
{
    واپس randombytes_random();
}

# اگر defined(FEAT_EVAL)
    ساکن خالی
crypt_sodium_report_hash_params(
	غیرعلامتی طویل طویل opslimit,
	غیرعلامتی طویل طویل ops_def,
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
	ورنہ
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	اگر (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (غیرعلامتی طویل)memlimit);
	ورنہ
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (غیرعلامتی طویل)memlimit);
	اگر (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	ورنہ
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    ساکن خالی
crypt_long_long_مقصد_char(طویل طویل n, char_u *s)
{
    صحیح i;
    برائے (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (غیرعلامتی طویل طویل)n >> 8;
    }
}

    ساکن خالی
crypt_int_مقصد_char(صحیح n, char_u *s)
{
    صحیح i;
    برائے (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (غیرعلامتی طویل طویل)n >> 8;
    }
}

    ساکن طویل طویل
crypt_char_مقصد_long_long(char_u *s)
{
    غیرعلامتی طویل طویل    retval = 0;
    صحیح i;
    برائے (i = 7; i >= 0; i--)
    {
	اگر (i == 7)
	    retval = s[i];
	ورنہ
	    retval |= s[i];
	اگر (i > 0)
	    retval <<= 8;
    }
    واپس retval;
}

    ساکن صحیح
crypt_char_مقصد_int(char_u *s)
{
    صحیح retval = 0;
    صحیح i;

    برائے (i = 3; i >= 0; i--)
    {
	اگر (i == 3)
	    retval = s[i];
	ورنہ
	    retval |= s[i];
	اگر (i > 0)
	    retval <<= 8;
    }
    واپس retval;
}
#اگر_ختم

#اگر_ختم // FEAT_CRYPT
