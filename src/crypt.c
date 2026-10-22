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
    صحيح		ٻج_len;	// length of seed, or 0 when not using seed
    صحيح		جوڙيو_len;	// additional length in the header needed for storing
				// custom data
#جيڪڏهن_وصف_ٿيل CRYPT_NOT_INPLACE
    صحيح		هلي_inplace;	// encryption/decryption can be done in-place
#جيڪڏهن_پورو
    صحيح		سڄو_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    صحيح (*self_test_fn)(فراغ);

    // Function pointer for initializing encryption/decryption.
    صحيح (* init_fn)(cryptstate_T *state, char_u *ڪنجي, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    فراغ (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *منزل, صحيح آخري);
    فراغ (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *منزل, صحيح آخري);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    ڊگهو (*encode_بفر_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, صحيح آخري);
    ڊگهو (*decode_بفر_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, صحيح آخري);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    فراغ (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, صحيح آخري);
    فراغ (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, صحيح آخري);
} cryptmethod_T;

ساكن صحيح crypt_sodium_init_(cryptstate_T *state, char_u *ڪنجي, crypt_arg_T *arg);
ساكن ڊگهو crypt_sodium_بفر_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_ٻاهريون, صحيح آخري);
ساكن ڊگهو crypt_sodium_بفر_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_ٻاهريون, صحيح آخري);
#إذا defined(FEAT_SODIUM)
ساكن فراغ crypt_long_long_منزل_char(ڊگهو ڊگهو n, char_u *s);
ساكن فراغ crypt_int_منزل_char(صحيح n, char_u *s);
ساكن ڊگهو ڊگهو crypt_char_منزل_long_long(char_u *s);
ساكن صحيح crypt_char_منزل_int(char_u *s);
#جيڪڏهن_پورو
#إذا defined(FEAT_EVAL) && defined(FEAT_SODIUM)
ساكن فراغ crypt_sodium_report_hash_params(بنانشان ڊگهو ڊگهو opslimit, بنانشان ڊگهو ڊگهو ops_def, size_t memlimit, size_t mem_def, صحيح alg, صحيح alg_def);
#جيڪڏهن_پورو

// index is method_nr of cryptstate_T, CRYPT_M_*
ساكن cryptmethod_T cryptmethods[CRYPT_M_ڳڻپ] = {
    // PK_Zip; very weak
    {
	"zip",
	"VimCrypt~01!",
	0,
	0,
	0,
#جيڪڏهن_وصف_ٿيل CRYPT_NOT_INPLACE
	TRUE,
#جيڪڏهن_پورو
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
#جيڪڏهن_وصف_ٿيل CRYPT_NOT_INPLACE
	TRUE,
#جيڪڏهن_پورو
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
#جيڪڏهن_وصف_ٿيل CRYPT_NOT_INPLACE
	TRUE,
#جيڪڏهن_پورو
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
#جيڪڏهن_وصف_ٿيل FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#وإلا
	16,
#جيڪڏهن_پورو
	8,
	0,
#جيڪڏهن_وصف_ٿيل CRYPT_NOT_INPLACE
	FALSE,
#جيڪڏهن_پورو
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
#جيڪڏهن_وصف_ٿيل FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#وإلا
	16,
#جيڪڏهن_پورو
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#جيڪڏهن_وصف_ٿيل CRYPT_NOT_INPLACE
	FALSE,
#جيڪڏهن_پورو
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
    size_t	    ڳڻپ;
    بنانشان حرف   ڪنجي[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define ڀريو_dll vim_ڀريو_Lib
#   define نشان_from_dll Get_Proc_پتو
#   define close_dll FreeLibrary
#   define ڀريو_dll_error GetWin32Error
#  وإلا
#   error Dynamic loading جو libsodium is not supported لكل now.
//#   define HINSTANCE void*
//#   define SODIUM_PROC void*
//#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
//#   define symbol_from_dll dlsym
//#   define close_dll dlclose
//#   define load_dll_error dlerror
#  endif

#  define sodium_init	    ڀريو_sodium
#  define sodium_free	    dll_sodium_free
#  define sodium_malloc	    dll_sodium_malloc
#  define sodium_memzero    dll_sodium_memzero
#  define sodium_mlock	    dll_sodium_mlock
#  define sodium_munlock    dll_sodium_munlock
#  define crypto_secretstream_xchacha20poly1305_init_ڌڪو \
    dll_crypto_secretstream_xchacha20poly1305_init_ڌڪو
#  define crypto_secretstream_xchacha20poly1305_ڌڪو \
    dll_crypto_secretstream_xchacha20poly1305_ڌڪو
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
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_init_ڌڪو)
   (crypto_secretstream_xchacha20poly1305_state *state,
    بنانشان حرف [],
    ثابت بنانشان حرف []) = عدم;
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_ڌڪو)
   (crypto_secretstream_xchacha20poly1305_state *state,
    بنانشان حرف *c, بنانشان ڊگهو ڊگهو *clen_p,
    ثابت بنانشان حرف *m, بنانشان ڊگهو ڊگهو mlen,
    ثابت بنانشان حرف *ad, بنانشان ڊگهو ڊگهو adlen, بنانشان حرف ٽيگ)
    = عدم;
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ثابت بنانشان حرف [],
    ثابت بنانشان حرف []) = عدم;
ساكن صحيح (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    بنانشان حرف *m, بنانشان ڊگهو ڊگهو *mlen_p, بنانشان حرف *ٽيگ_p,
    ثابت بنانشان حرف *c, بنانشان ڊگهو ڊگهو clen,
    ثابت بنانشان حرف *ad, بنانشان ڊگهو ڊگهو adlen) = عدم;
ساكن صحيح (*dll_crypto_pwhash)(بنانشان حرف * ثابت ٻاهريون,
    بنانشان ڊگهو ڊگهو outlen,
    ثابت حرف * ثابت passwd, بنانشان ڊگهو ڊگهو passwdlen,
    ثابت بنانشان حرف * ثابت salt,
    بنانشان ڊگهو ڊگهو opslimit, size_t memlimit, صحيح alg)
    = عدم;
ساكن فراغ (*dll_randombytes_buf)(فراغ * ثابت buf, ثابت size_t سائيز);
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
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_ڌڪو},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_ڌڪو},
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

    hsodium = ڀريو_dll(libname);
    إذا (hsodium == عدم)
    {
	إذا (verbose)
	    semsg(_(e_could_not_ڀريو_library_str_str), libname, ڀريو_dll_error());
	أرجع FAIL;
    }

    لكل (i = 0; sodium_funcname_جدول[i].ptr; ++i)
    {
	إذا ((*sodium_funcname_جدول[i].ptr = نشان_from_dll(hsodium,
			sodium_funcname_جدول[i].name)) == عدم)
	{
	    close_dll(hsodium);
	    hsodium = عدم;
	    إذا (verbose)
		semsg(_(e_could_not_ڀريو_library_function_str), sodium_funcname_جدول[i].name);
	    أرجع FAIL;
	}
    }
    أرجع OK;
}

    ساكن صحيح
ڀريو_sodium(فراغ)
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
#جيڪڏهن_پورو

#وصف_ڪريو CRYPT_MAGIC_LEN	12	// cannot change
ساكن ثابت حرف crypt_magic_سر[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    صحيح
crypt_طريقو_nr_from_name(char_u *name)
{
    صحيح i;

    لكل (i = 0; i < CRYPT_M_ڳڻپ; ++i)
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
crypt_طريقو_nr_from_magic(حرف *ptr, صحيح len)
{
    صحيح i;

    إذا (len < CRYPT_MAGIC_LEN)
	أرجع -1;

    لكل (i = 0; i < CRYPT_M_ڳڻپ; i++)
	إذا (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    أرجع i;

    i = (صحيح)STRLEN(crypt_magic_سر);
    إذا (len >= i && memcmp(ptr, crypt_magic_سر, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_طريقو));

    أرجع -1;
}

#جيڪڏهن_وصف_ٿيل CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    صحيح
crypt_هلي_inplace(cryptstate_T *state)
{
    أرجع cryptmethods[state->طريقو_nr].هلي_inplace;
}
#جيڪڏهن_پورو

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    صحيح
crypt_get_طريقو_nr(buf_T *buf)
{
    أرجع crypt_طريقو_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    صحيح
crypt_طريقو_is_sodium(صحيح طريقو)
{
    أرجع طريقو == CRYPT_M_SOD || طريقو == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    صحيح
crypt_سڄو_undofile(صحيح طريقو_nr)
{
    أرجع cryptmethods[طريقو_nr].سڄو_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    صحيح
crypt_get_سرو_len(صحيح طريقو_nr)
{
    أرجع CRYPT_MAGIC_LEN
	+ cryptmethods[طريقو_nr].salt_len
	+ cryptmethods[طريقو_nr].ٻج_len
	+ cryptmethods[طريقو_nr].جوڙيو_len;
}


#إذا defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    صحيح
crypt_get_max_سرو_len(فراغ)
{
    صحيح i;
    صحيح max = 0;
    صحيح عارضي = 0;

    لكل (i = 0; i < CRYPT_M_ڳڻپ; ++i)
    {
	عارضي = crypt_get_سرو_len(i);
	إذا (عارضي > max)
	    max = عارضي;
    }
    أرجع max;
}
#جيڪڏهن_پورو

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    فراغ
crypt_سيٽ_cm_option(buf_T *buf, صحيح طريقو_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[طريقو_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    صحيح
crypt_self_test(فراغ)
{
    صحيح طريقو_nr = crypt_get_طريقو_nr(curbuf);

    إذا (cryptmethods[طريقو_nr].self_test_fn == عدم)
	أرجع OK;
    أرجع cryptmethods[طريقو_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    صحيح		طريقو_nr,
    char_u	*ڪنجي,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    إذا (state == عدم)
	أرجع state;

    state->طريقو_nr = طريقو_nr;
    إذا (cryptmethods[طريقو_nr].init_fn(state, ڪنجي, crypt_arg) == FAIL)
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
crypt_create_from_سرو(
    صحيح		طريقو_nr,
    char_u	*ڪنجي,
    char_u	*سرو)
{
    crypt_arg_T arg;

    CLEAR_ميدان(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[طريقو_nr].salt_len;
    arg.cat_ٻج_len = cryptmethods[طريقو_nr].ٻج_len;
    arg.cat_جوڙيو_len = cryptmethods[طريقو_nr].جوڙيو_len;
    إذا (arg.cat_salt_len > 0)
	arg.cat_salt = سرو + CRYPT_MAGIC_LEN;
    إذا (arg.cat_ٻج_len > 0)
	arg.cat_ٻج = سرو + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    إذا (arg.cat_جوڙيو_len > 0)
	arg.cat_جوڙيو = سرو + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_ٻج_len;

    أرجع crypt_create(طريقو_nr, ڪنجي, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *ڪنجي)
{
    صحيح		طريقو_nr;
    صحيح		سرو_len;
    حرف	magic_بفر[CRYPT_MAGIC_LEN];
    char_u	*بفر;
    cryptstate_T *state;

    إذا (fread(magic_بفر, CRYPT_MAGIC_LEN, 1, fp) != 1)
	أرجع عدم;
    طريقو_nr = crypt_طريقو_nr_from_magic(magic_بفر, CRYPT_MAGIC_LEN);
    إذا (طريقو_nr < 0)
	أرجع عدم;

    سرو_len = crypt_get_سرو_len(طريقو_nr);
    إذا ((بفر = alloc(سرو_len)) == عدم)
	أرجع عدم;
    mch_memmove(بفر, magic_بفر, CRYPT_MAGIC_LEN);
    إذا (سرو_len > CRYPT_MAGIC_LEN
	    && fread(بفر + CRYPT_MAGIC_LEN,
				    سرو_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(بفر);
	أرجع عدم;
    }

    state = crypt_create_from_سرو(طريقو_nr, ڪنجي, بفر);
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
    صحيح	    طريقو_nr,
    char_u  *ڪنجي,
    char_u  **سرو,
    صحيح	    *سرو_len)
{
    صحيح	    len = crypt_get_سرو_len(طريقو_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_ميدان(arg);
    arg.cat_salt_len = cryptmethods[طريقو_nr].salt_len;
    arg.cat_ٻج_len = cryptmethods[طريقو_nr].ٻج_len;
    arg.cat_جوڙيو_len  = cryptmethods[طريقو_nr].جوڙيو_len;
    arg.cat_init_from_file = FALSE;

    *سرو_len = len;
    *سرو = alloc(len);
    إذا (*سرو == عدم)
	أرجع عدم;

    mch_memmove(*سرو, cryptmethods[طريقو_nr].magic, CRYPT_MAGIC_LEN);
    إذا (arg.cat_salt_len > 0 || arg.cat_ٻج_len > 0 || arg.cat_جوڙيو_len > 0)
    {
	إذا (arg.cat_salt_len > 0)
	    arg.cat_salt = *سرو + CRYPT_MAGIC_LEN;
	إذا (arg.cat_ٻج_len > 0)
	    arg.cat_ٻج = *سرو + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	إذا (arg.cat_جوڙيو_len > 0)
	    arg.cat_جوڙيو = *سرو + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_ٻج_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#جيڪڏهن_وصف_ٿيل FEAT_SODIUM
	إذا (sodium_init() >= 0)
	{
	    إذا (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    إذا (arg.cat_ٻج_len > 0)
		randombytes_buf(arg.cat_ٻج, arg.cat_ٻج_len);
	}
	وإلا
#جيڪڏهن_پورو
	    sha2_ٻج(arg.cat_salt, arg.cat_salt_len, arg.cat_ٻج, arg.cat_ٻج_len);
    }
    state = crypt_create(طريقو_nr, ڪنجي, &arg);
    إذا (state == عدم)
	VIM_CLEAR(*سرو);
    أرجع state;
}

/*
 * Free the crypt state.
 */
    فراغ
crypt_free_state(cryptstate_T *state)
{
#جيڪڏهن_وصف_ٿيل FEAT_SODIUM
    إذا (crypt_طريقو_is_sodium(state->طريقو_nr))
    {
	sodium_munlock(((sodium_state_T *)state->طريقو_state)->ڪنجي,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->طريقو_state, حجم(sodium_state_T));
	sodium_free(state->طريقو_state);
    }
    وإلا
#جيڪڏهن_پورو
	vim_free(state->طريقو_state);
    vim_free(state);
}

#جيڪڏهن_وصف_ٿيل CRYPT_NOT_INPLACE
/*
 * Encode "from[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    ڊگهو
crypt_encode_alloc(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	**newptr,
    صحيح		آخري)
{
    cryptmethod_T *طريقو = &cryptmethods[state->طريقو_nr];

    إذا (طريقو->encode_بفر_fn != عدم)
	// Has buffer function, pass through.
	أرجع طريقو->encode_بفر_fn(state, from, len, newptr, آخري);
    إذا (len == 0)
	// Not buffering, just return EOF.
	أرجع (ڊگهو)len;

    *newptr = alloc(len + 50);
    إذا (*newptr == عدم)
	أرجع -1;
    طريقو->encode_fn(state, from, len, *newptr, آخري);
    أرجع (ڊگهو)len;
}

/*
 * Decrypt "ptr[len]" and store the result in a newly allocated buffer, which
 * is stored in "newptr".
 * Return number of bytes in "newptr", 0 for need more or -1 on error.
 */
    ڊگهو
crypt_decode_alloc(
    cryptstate_T *state,
    char_u	*ptr,
    ڊگهو	len,
    char_u      **newptr,
    صحيح		آخري)
{
    cryptmethod_T *طريقو = &cryptmethods[state->طريقو_nr];

    إذا (طريقو->decode_بفر_fn != عدم)
	// Has buffer function, pass through.
	أرجع طريقو->decode_بفر_fn(state, ptr, len, newptr, آخري);

    إذا (len == 0)
	// Not buffering, just return EOF.
	أرجع len;

    *newptr = alloc(len);
    إذا (*newptr == عدم)
	أرجع -1;
    طريقو->decode_fn(state, ptr, len, *newptr, آخري);
    أرجع len;
}
#جيڪڏهن_پورو

/*
 * Encrypting "from[len]" into "to[len]".
 */
    فراغ
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*منزل,
    صحيح		آخري)
{
    cryptmethods[state->طريقو_nr].encode_fn(state, from, len, منزل, آخري);
}

#إذا 0  //اڻ_استعمال_ٿيل
/*
 * decrypting "from[len]" into "to[len]".
 */
    فراغ
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*منزل,
    صحيح		آخري)
{
    cryptmethods[state->طريقو_nr].decode_fn(state, from, len, منزل, آخري);
}
#جيڪڏهن_پورو

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    فراغ
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    صحيح		آخري)
{
    cryptmethods[state->طريقو_nr].encode_inplace_fn(state, buf, len,
								    buf, آخري);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    فراغ
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    صحيح		آخري)
{
    cryptmethods[state->طريقو_nr].decode_inplace_fn(state, buf, len,
								    buf, آخري);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    فراغ
crypt_free_ڪنجي(char_u *ڪنجي)
{
    // Create a safe memset which cannot be optimized away by compiler
    ساكن فراغ *(* متطاير vim_memset_محفوظ)(فراغ *s, صحيح c, size_t n) =
	memset;
    إذا (ڪنجي != عدم)
    {
#جيڪڏهن_وصف_ٿيل FEAT_SODIUM
	إذا (sodium_init() >= 0)
	    sodium_memzero(ڪنجي, STRLEN(ڪنجي));
	وإلا
#جيڪڏهن_پورو
	    vim_memset_محفوظ(ڪنجي, 0, STRLEN(ڪنجي));
	vim_free(ڪنجي);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    فراغ
crypt_check_طريقو(صحيح طريقو)
{
    إذا (طريقو < CRYPT_M_BF2 || طريقو == CRYPT_M_SOD)
    {
	نياپو_scroll = TRUE;
	نياپو(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    فراغ
crypt_check_swapfile_curbuf(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_SODIUM
    صحيح طريقو = crypt_get_طريقو_nr(curbuf);
    إذا (crypt_طريقو_is_sodium(طريقو))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	سيٽ_option_قدر_give_err((char_u *)"swf", 0, عدم, OPT_LOCAL);
	نياپو_scroll = TRUE;
	نياپو(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#جيڪڏهن_پورو
}

    فراغ
crypt_check_current_طريقو(فراغ)
{
    crypt_check_طريقو(crypt_get_طريقو_nr(curbuf));
}

/*
 * Ask the user for a crypt key.
 * When "store" is TRUE, the new key is stored in the 'key' option, and the
 * 'key' option value is returned: Don't free it.
 * When "store" is FALSE, the typed key is returned in allocated memory.
 * Returns NULL on failure.
 */
    char_u *
crypt_get_ڪنجي(
    صحيح		store,
    صحيح		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = عدم;
    صحيح		ڦيرو;

    لكل (ڦيرو = 0; ; ++ڦيرو)
    {
	cmdline_تارو = TRUE;
	cmdline_صف = نياپو_صف;
	p1 = getcmdline_prompt(NUL, ڦيرو == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		عدم);
	cmdline_تارو = FALSE;

	إذا (p1 == عدم)
	    اكسر;

	إذا (ڦيرو == twice)
	{
	    إذا (p2 != عدم && STRCMP(p1, p2) != 0)
	    {
		نياپو(_("Keys don't match!"));
		crypt_free_ڪنجي(p1);
		crypt_free_ڪنجي(p2);
		p2 = عدم;
		ڦيرو = -1;		// do it again
		استمر;
	    }

	    إذا (store)
	    {
		سيٽ_option_قدر_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_ڪنجي(p1);
		p1 = curbuf->b_p_ڪنجي;
		crypt_check_swapfile_curbuf();
	    }
	    اكسر;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    إذا (!crypt_طريقو_is_sodium(crypt_get_طريقو_nr(curbuf)))
    {
	إذا (نياپو_didout)
	    نياپو_putchar('\n');
	need_wait_return = FALSE;
	نياپو_didout = FALSE;
    }

    crypt_free_ڪنجي(p2);
    أرجع p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    فراغ
crypt_append_نياپو(
    buf_T *buf)
{
    إذا (crypt_get_طريقو_nr(buf) == 0)
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
    cryptstate_T	*state اڻ_استعمال_ٿيل,
    char_u		*ڪنجي اڻ_استعمال_ٿيل,
    crypt_arg_T		*arg اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    بنانشان حرف	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    صحيح			retval = 0;
    بنانشان ڊگهو ڊگهو	opslimit;
    بنانشان ڊگهو ڊگهو	memlimit;
    صحيح			alg;

    إذا (sodium_init() < 0)
	أرجع FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(حجم(sodium_state_T));
    sodium_memzero(sd_state, حجم(sodium_state_T));

    إذا ((state->طريقو_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->طريقو_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# إذا 0
	// For testing
	إذا (state->طريقو_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	إذا (crypto_pwhash(dkey, حجم(dkey), (ثابت حرف *)ڪنجي, STRLEN(ڪنجي),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
	يادگيري_نقل(sd_state->ڪنجي, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->ڪنجي, crypto_box_SEEDBYTES);
	retval += sodium_mlock(ڪنجي, STRLEN(ڪنجي));

	إذا (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	إذا (state->طريقو_nr == CRYPT_M_SOD2 && arg->cat_جوڙيو != عدم)
	{
	    char_u	بفر[20];
	    char_u	*p = بفر;
	    vim_memset(بفر, 0, 20);

	    crypt_long_long_منزل_char(opslimit, p);
	    p += حجم(opslimit);

	    crypt_long_long_منزل_char(memlimit, p);
	    p += حجم(memlimit);

	    crypt_int_منزل_char(alg, p);
	    يادگيري_نقل(arg->cat_جوڙيو, بفر, حجم(opslimit) + حجم(memlimit) + حجم(alg));
	}
    }
    وإلا
    {
	char_u	بفر[20];
	char_u	*p = بفر;
	vim_memset(بفر, 0, 20);
	صحيح	سائيز = حجم(opslimit) +
	    حجم(memlimit) + حجم(alg);

	// Reading parameters from file
	إذا (arg->cat_جوڙيو_len < سائيز)
	{
	    sodium_free(sd_state);
	    أرجع FAIL;
	}

	// derive the key from the file header
	يادگيري_نقل(p, arg->cat_جوڙيو, سائيز);
	arg->cat_جوڙيو += سائيز;

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

	إذا (crypto_pwhash(dkey, حجم(dkey), (ثابت حرف *)ڪنجي, STRLEN(ڪنجي),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
	يادگيري_نقل(sd_state->ڪنجي, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->ڪنجي, crypto_box_SEEDBYTES);
	retval += sodium_mlock(ڪنجي, STRLEN(ڪنجي));

	إذا (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    أرجع FAIL;
	}
    }
    sd_state->ڳڻپ = 0;
    state->طريقو_state = sd_state;

    أرجع OK;
#وإلا
    emsg(_(e_libsodium_not_built_اندريون));
    أرجع FAIL;
#جيڪڏهن_پورو
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#إذا 0  // Currently unused
    فراغ
crypt_sodium_encode(
    cryptstate_T *state اڻ_استعمال_ٿيل,
    char_u	*from اڻ_استعمال_ٿيل,
    size_t	len اڻ_استعمال_ٿيل,
    char_u	*منزل اڻ_استعمال_ٿيل,
    صحيح		آخري اڻ_استعمال_ٿيل)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->طريقو_state;
    بنانشان حرف  ٽيگ = آخري
			? crypto_secretstream_xchacha20poly1305_ٽيگ_FINAL  : 0;

    إذا (sod_st->ڳڻپ == 0)
    {
	إذا (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_سرو));
	    أرجع;
	}
	crypto_secretstream_xchacha20poly1305_init_ڌڪو(&sod_st->state,
							      منزل, sod_st->ڪنجي);
	منزل += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    إذا (sod_st->ڳڻپ && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_بفر));
	أرجع;
    }

    crypto_secretstream_xchacha20poly1305_ڌڪو(&sod_st->state, منزل, عدم,
						      from, len, عدم, 0, ٽيگ);

    sod_st->ڳڻپ++;
# endif
}
#جيڪڏهن_پورو

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#إذا 0  // Currently unused
    فراغ
crypt_sodium_decode(
    cryptstate_T *state اڻ_استعمال_ٿيل,
    char_u	*from اڻ_استعمال_ٿيل,
    size_t	len اڻ_استعمال_ٿيل,
    char_u	*منزل اڻ_استعمال_ٿيل,
    صحيح		آخري اڻ_استعمال_ٿيل)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->طريقو_state;
    بنانشان حرف  ٽيگ;
    بنانشان ڊگهو ڊگهو buf_len;
    char_u *p1 = from;
    char_u *p2 = منزل;
    char_u *buf_ٻاهريون;

    إذا (sod_st->ڳڻپ == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_سرو));
	أرجع;
    }

    buf_ٻاهريون = (char_u *)alloc(len);

    إذا (buf_ٻاهريون == عدم)
    {
	emsg(_(e_libsodium_cannot_allocate_بفر));
	أرجع;
    }
    إذا (sod_st->ڳڻپ == 0)
    {
	إذا (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->ڪنجي) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_سرو_incomplete));
	    اذهب fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	إذا (p1 == p2)
	    منزل += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    إذا (sod_st->ڳڻپ && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_بفر));
	اذهب fail;
    }
    إذا (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_ٻاهريون, &buf_len, &ٽيگ, from, len, عدم, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	اذهب fail;
    }
    sod_st->ڳڻپ++;

    إذا (ٽيگ == crypto_secretstream_xchacha20poly1305_ٽيگ_FINAL && !آخري)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	اذهب fail;
    }
    إذا (p1 == p2)
	mch_memmove(p2, buf_ٻاهريون, buf_len);

fail:
    vim_free(buf_ٻاهريون);
# endif
}
#جيڪڏهن_پورو

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ساكن ڊگهو
crypt_sodium_بفر_encode(
    cryptstate_T *state اڻ_استعمال_ٿيل,
    char_u	*from اڻ_استعمال_ٿيل,
    size_t	len اڻ_استعمال_ٿيل,
    char_u	**buf_ٻاهريون اڻ_استعمال_ٿيل,
    صحيح		آخري اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    بنانشان ڊگهو ڊگهو	ٻاهريون_len;
    char_u		*ptr;
    بنانشان حرف	ٽيگ = آخري
			? crypto_secretstream_xchacha20poly1305_ٽيگ_FINAL  : 0;
    صحيح			ڊيگهائي;
    sodium_state_T	*sod_st = state->طريقو_state;
    صحيح			پهريون = (sod_st->ڳڻپ == 0);

    ڊيگهائي = (صحيح)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (پهريون ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_ٻاهريون = alloc_clear(ڊيگهائي);
    إذا (*buf_ٻاهريون == عدم)
    {
	emsg(_(e_libsodium_cannot_allocate_بفر));
	أرجع -1;
    }
    ptr = *buf_ٻاهريون;

    إذا (پهريون)
    {
	crypto_secretstream_xchacha20poly1305_init_ڌڪو(&sod_st->state,
		ptr, sod_st->ڪنجي);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_ڌڪو(&sod_st->state, ptr,
	    &ٻاهريون_len, from, len, عدم, 0, ٽيگ);

    sod_st->ڳڻپ++;
    أرجع ٻاهريون_len + (پهريون
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#وإلا
    أرجع -1;
#جيڪڏهن_پورو
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ساكن ڊگهو
crypt_sodium_بفر_decode(
    cryptstate_T *state اڻ_استعمال_ٿيل,
    char_u	*from اڻ_استعمال_ٿيل,
    size_t	len اڻ_استعمال_ٿيل,
    char_u	**buf_ٻاهريون اڻ_استعمال_ٿيل,
    صحيح		آخري اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->طريقو_state;
    بنانشان حرف  ٽيگ;
    بنانشان ڊگهو ڊگهو ٻاهريون_len;

    إذا (sod_st->ڳڻپ == 0
	    && state->طريقو_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].جوڙيو_len;

    *buf_ٻاهريون = alloc_clear(len);
    إذا (*buf_ٻاهريون == عدم)
    {
	emsg(_(e_libsodium_cannot_allocate_بفر));
	أرجع -1;
    }

    إذا (sod_st->ڳڻپ == 0)
    {
	إذا (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->ڪنجي) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_سرو_incomplete));
	    أرجع -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->ڳڻپ++;
    }
    إذا (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_ٻاهريون, &ٻاهريون_len, &ٽيگ, from, len, عدم, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	أرجع -1;
    }

    إذا (ٽيگ == crypto_secretstream_xchacha20poly1305_ٽيگ_FINAL && !آخري)
	emsg(_(e_libsodium_decryption_failed_premature));
    أرجع (ڊگهو) ٻاهريون_len;
#وإلا
    أرجع -1;
#جيڪڏهن_پورو
}

#إذا defined(FEAT_SODIUM)
    فراغ
crypt_sodium_تالو_ڪنجي(char_u *ڪنجي)
{
    إذا (sodium_init() >= 0)
	sodium_mlock(ڪنجي, STRLEN(ڪنجي));
}

    صحيح
crypt_sodium_munlock(فراغ *ثابت addr, ثابت size_t len)
{
    أرجع sodium_munlock(addr, len);
}

    فراغ
crypt_sodium_randombytes_buf(فراغ *ثابت buf, ثابت size_t سائيز)
{
    randombytes_buf(buf, سائيز);
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
	بنانشان ڊگهو ڊگهو opslimit,
	بنانشان ڊگهو ڊگهو ops_def,
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
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (بنانشان ڊگهو)memlimit);
	وإلا
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (بنانشان ڊگهو)memlimit);
	إذا (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	وإلا
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    ساكن فراغ
crypt_long_long_منزل_char(ڊگهو ڊگهو n, char_u *s)
{
    صحيح i;
    لكل (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (بنانشان ڊگهو ڊگهو)n >> 8;
    }
}

    ساكن فراغ
crypt_int_منزل_char(صحيح n, char_u *s)
{
    صحيح i;
    لكل (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (بنانشان ڊگهو ڊگهو)n >> 8;
    }
}

    ساكن ڊگهو ڊگهو
crypt_char_منزل_long_long(char_u *s)
{
    بنانشان ڊگهو ڊگهو    retval = 0;
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
#جيڪڏهن_پورو

#جيڪڏهن_پورو // FEAT_CRYPT
