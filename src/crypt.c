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
#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_CRYPT)
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

પ્રકારવ્યાખ્યા સંરચના {
    અક્ષર	*name;		// encryption name as used in 'cryptmethod'
    અક્ષર	*magic;		// magic bytes stored in file header
    પૂર્ણાંક		salt_len;	// length of salt, or 0 when not using salt
    પૂર્ણાંક		seed_len;	// length of seed, or 0 when not using seed
    પૂર્ણાંક		add_len;	// additional length in the header needed for storing
				// custom data
#ifdef CRYPT_NOT_INPLACE
    પૂર્ણાંક		works_inplace;	// encryption/decryption can be done in-place
#endif
    પૂર્ણાંક		whole_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    પૂર્ણાંક (*self_test_fn)(શૂન્ય);

    // Function pointer for initializing encryption/decryption.
    પૂર્ણાંક (* init_fn)(cryptstate_T *state, char_u *ચાવી, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    શૂન્ય (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, પૂર્ણાંક last);
    શૂન્ય (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, પૂર્ણાંક last);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    long (*encode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, પૂર્ણાંક last);
    long (*decode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, પૂર્ણાંક last);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    શૂન્ય (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, પૂર્ણાંક last);
    શૂન્ય (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, પૂર્ણાંક last);
} cryptmethod_T;

સ્થિર પૂર્ણાંક crypt_sodium_init_(cryptstate_T *state, char_u *ચાવી, crypt_arg_T *arg);
સ્થિર long crypt_sodium_buffer_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, પૂર્ણાંક last);
સ્થિર long crypt_sodium_buffer_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, પૂર્ણાંક last);
#જો defined(FEAT_SODIUM)
સ્થિર શૂન્ય crypt_long_long_to_char(long long n, char_u *s);
સ્થિર શૂન્ય crypt_int_to_char(પૂર્ણાંક n, char_u *s);
સ્થિર long long crypt_char_to_long_long(char_u *s);
સ્થિર પૂર્ણાંક crypt_char_to_int(char_u *s);
#endif
#જો defined(FEAT_EVAL) && defined(FEAT_SODIUM)
સ્થિર શૂન્ય crypt_sodium_report_hash_params(unsigned long long opslimit, unsigned long long ops_def, size_t memlimit, size_t mem_def, પૂર્ણાંક alg, પૂર્ણાંક alg_def);
#endif

// index is method_nr of cryptstate_T, CRYPT_M_*
સ્થિર cryptmethod_T cryptmethods[CRYPT_M_ગણતરી] = {
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
	ખાલી,
	crypt_zip_init,
	crypt_zip_encode, crypt_zip_decode,
	ખાલી, ખાલી,
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
	ખાલી, ખાલી,
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
	ખાલી, ખાલી,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // XChaCha20 using libsodium; implementation issues
    {
	"xchacha20",
	"VimCrypt~04!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#નહીંતર
	16,
#endif
	8,
	0,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	ખાલી,
	crypt_sodium_init_,
	ખાલી, ખાલી,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	ખાલી, ખાલી,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#નહીંતર
	16,
#endif
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	ખાલી,
	crypt_sodium_init_,
	ખાલી, ખાલી,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	ખાલી, ખાલી,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#જો defined(FEAT_SODIUM)
પ્રકારવ્યાખ્યા સંરચના {
    size_t	    ગણતરી;
    unsigned અક્ષર   ચાવી[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define load_dll vimLoadLib
#   define પ્રતીક_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  નહીંતર
#   error Dynamic loading of libsodium is not supported માટે now.
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
#  define crypto_secretstream_xchacha20poly1305_init_ધકેલો \
    dll_crypto_secretstream_xchacha20poly1305_init_ધકેલો
#  define crypto_secretstream_xchacha20poly1305_ધકેલો \
    dll_crypto_secretstream_xchacha20poly1305_ધકેલો
#  define crypto_secretstream_xchacha20poly1305_init_pull \
    dll_crypto_secretstream_xchacha20poly1305_init_pull
#  define crypto_secretstream_xchacha20poly1305_pull \
    dll_crypto_secretstream_xchacha20poly1305_pull
#  define crypto_pwhash	    dll_crypto_pwhash
#  define randombytes_buf   dll_randombytes_buf
#  define randombytes_random dll_randombytes_random

સ્થિર પૂર્ણાંક (*dll_sodium_init)(શૂન્ય) = ખાલી;
સ્થિર શૂન્ય (*dll_sodium_free)(શૂન્ય *) = ખાલી;
સ્થિર શૂન્ય *(*dll_sodium_malloc)(અચલ size_t) = ખાલી;
સ્થિર શૂન્ય (*dll_sodium_memzero)(શૂન્ય * અચલ, અચલ size_t) = ખાલી;
સ્થિર પૂર્ણાંક (*dll_sodium_mlock)(શૂન્ય * અચલ, અચલ size_t) = ખાલી;
સ્થિર પૂર્ણાંક (*dll_sodium_munlock)(શૂન્ય * અચલ, અચલ size_t) = ખાલી;
સ્થિર પૂર્ણાંક (*dll_crypto_secretstream_xchacha20poly1305_init_ધકેલો)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned અક્ષર [],
    અચલ unsigned અક્ષર []) = ખાલી;
સ્થિર પૂર્ણાંક (*dll_crypto_secretstream_xchacha20poly1305_ધકેલો)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned અક્ષર *c, unsigned long long *clen_p,
    અચલ unsigned અક્ષર *m, unsigned long long mlen,
    અચલ unsigned અક્ષર *ad, unsigned long long adlen, unsigned અક્ષર tag)
    = ખાલી;
સ્થિર પૂર્ણાંક (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    અચલ unsigned અક્ષર [],
    અચલ unsigned અક્ષર []) = ખાલી;
સ્થિર પૂર્ણાંક (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned અક્ષર *m, unsigned long long *mlen_p, unsigned અક્ષર *tag_p,
    અચલ unsigned અક્ષર *c, unsigned long long clen,
    અચલ unsigned અક્ષર *ad, unsigned long long adlen) = ખાલી;
સ્થિર પૂર્ણાંક (*dll_crypto_pwhash)(unsigned અક્ષર * અચલ out,
    unsigned long long outlen,
    અચલ અક્ષર * અચલ passwd, unsigned long long passwdlen,
    અચલ unsigned અક્ષર * અચલ salt,
    unsigned long long opslimit, size_t memlimit, પૂર્ણાંક alg)
    = ખાલી;
સ્થિર શૂન્ય (*dll_randombytes_buf)(શૂન્ય * અચલ buf, અચલ size_t size);
સ્થિર uint32_t (*dll_randombytes_random)(શૂન્ય);

સ્થિર સંરચના {
    અચલ અક્ષર *name;
    SODIUM_PROC *ptr;
} sodium_funcname_table[] = {
    {"sodium_init", (SODIUM_PROC*)&dll_sodium_init},
    {"sodium_free", (SODIUM_PROC*)&dll_sodium_free},
    {"sodium_malloc", (SODIUM_PROC*)&dll_sodium_malloc},
    {"sodium_memzero", (SODIUM_PROC*)&dll_sodium_memzero},
    {"sodium_mlock", (SODIUM_PROC*)&dll_sodium_mlock},
    {"sodium_munlock", (SODIUM_PROC*)&dll_sodium_munlock},
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_ધકેલો},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_ધકેલો},
    {"crypto_secretstream_xchacha20poly1305_init_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_pull},
    {"crypto_secretstream_xchacha20poly1305_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_pull},
    {"crypto_pwhash", (SODIUM_PROC*)&dll_crypto_pwhash},
    {"randombytes_buf", (SODIUM_PROC*)&dll_randombytes_buf},
    {"randombytes_random", (SODIUM_PROC*)&dll_randombytes_random},
    {ખાલી, ખાલી}
};

    સ્થિર પૂર્ણાંક
sodium_runtime_link_init(પૂર્ણાંક verbose)
{
    સ્થિર HINSTANCE hsodium = ખાલી;
    અચલ અક્ષર *libname = DYNAMIC_SODIUM_DLL;
    પૂર્ણાંક i;

    જો (hsodium != ખાલી)
	પાછા OK;

    hsodium = load_dll(libname);
    જો (hsodium == ખાલી)
    {
	જો (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	પાછા FAIL;
    }

    માટે (i = 0; sodium_funcname_table[i].ptr; ++i)
    {
	જો ((*sodium_funcname_table[i].ptr = પ્રતીક_from_dll(hsodium,
			sodium_funcname_table[i].name)) == ખાલી)
	{
	    close_dll(hsodium);
	    hsodium = ખાલી;
	    જો (verbose)
		semsg(_(e_could_not_load_library_function_str), sodium_funcname_table[i].name);
	    પાછા FAIL;
	}
    }
    પાછા OK;
}

    સ્થિર પૂર્ણાંક
load_sodium(શૂન્ય)
{
    જો (sodium_runtime_link_init(TRUE) == FAIL)
	પાછા -1;
    પાછા dll_sodium_init();
}
# endif

# જો defined(DYNAMIC_SODIUM)
    પૂર્ણાંક
sodium_enabled(પૂર્ણાંક verbose)
{
    પાછા sodium_runtime_link_init(verbose) == OK;
}
# endif
#endif

#define CRYPT_MAGIC_LEN	12	// cannot change
સ્થિર અચલ અક્ષર crypt_magic_head[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    પૂર્ણાંક
crypt_method_nr_from_name(char_u *name)
{
    પૂર્ણાંક i;

    માટે (i = 0; i < CRYPT_M_ગણતરી; ++i)
	જો (STRCMP(name, cryptmethods[i].name) == 0)
	    પાછા i;
    પાછા 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    પૂર્ણાંક
crypt_method_nr_from_magic(અક્ષર *ptr, પૂર્ણાંક len)
{
    પૂર્ણાંક i;

    જો (len < CRYPT_MAGIC_LEN)
	પાછા -1;

    માટે (i = 0; i < CRYPT_M_ગણતરી; i++)
	જો (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    પાછા i;

    i = (પૂર્ણાંક)STRLEN(crypt_magic_head);
    જો (len >= i && memcmp(ptr, crypt_magic_head, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_method));

    પાછા -1;
}

#ifdef CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    પૂર્ણાંક
crypt_works_inplace(cryptstate_T *state)
{
    પાછા cryptmethods[state->method_nr].works_inplace;
}
#endif

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    પૂર્ણાંક
crypt_get_method_nr(buf_T *buf)
{
    પાછા crypt_method_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    પૂર્ણાંક
crypt_method_is_sodium(પૂર્ણાંક method)
{
    પાછા method == CRYPT_M_SOD || method == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    પૂર્ણાંક
crypt_whole_undofile(પૂર્ણાંક method_nr)
{
    પાછા cryptmethods[method_nr].whole_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    પૂર્ણાંક
crypt_get_header_len(પૂર્ણાંક method_nr)
{
    પાછા CRYPT_MAGIC_LEN
	+ cryptmethods[method_nr].salt_len
	+ cryptmethods[method_nr].seed_len
	+ cryptmethods[method_nr].add_len;
}


#જો defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    પૂર્ણાંક
crypt_get_max_header_len(શૂન્ય)
{
    પૂર્ણાંક i;
    પૂર્ણાંક max = 0;
    પૂર્ણાંક temp = 0;

    માટે (i = 0; i < CRYPT_M_ગણતરી; ++i)
    {
	temp = crypt_get_header_len(i);
	જો (temp > max)
	    max = temp;
    }
    પાછા max;
}
#endif

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    શૂન્ય
crypt_set_cm_option(buf_T *buf, પૂર્ણાંક method_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[method_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    પૂર્ણાંક
crypt_self_test(શૂન્ય)
{
    પૂર્ણાંક method_nr = crypt_get_method_nr(curbuf);

    જો (cryptmethods[method_nr].self_test_fn == ખાલી)
	પાછા OK;
    પાછા cryptmethods[method_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    પૂર્ણાંક		method_nr,
    char_u	*ચાવી,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    જો (state == ખાલી)
	પાછા state;

    state->method_nr = method_nr;
    જો (cryptmethods[method_nr].init_fn(state, ચાવી, crypt_arg) == FAIL)
    {
	vim_free(state);
	પાછા ખાલી;
    }
    પાછા state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_header(
    પૂર્ણાંક		method_nr,
    char_u	*ચાવી,
    char_u	*header)
{
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len = cryptmethods[method_nr].add_len;
    જો (arg.cat_salt_len > 0)
	arg.cat_salt = header + CRYPT_MAGIC_LEN;
    જો (arg.cat_seed_len > 0)
	arg.cat_seed = header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    જો (arg.cat_add_len > 0)
	arg.cat_add = header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

    પાછા crypt_create(method_nr, ચાવી, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *ચાવી)
{
    પૂર્ણાંક		method_nr;
    પૂર્ણાંક		header_len;
    અક્ષર	magic_buffer[CRYPT_MAGIC_LEN];
    char_u	*buffer;
    cryptstate_T *state;

    જો (fread(magic_buffer, CRYPT_MAGIC_LEN, 1, fp) != 1)
	પાછા ખાલી;
    method_nr = crypt_method_nr_from_magic(magic_buffer, CRYPT_MAGIC_LEN);
    જો (method_nr < 0)
	પાછા ખાલી;

    header_len = crypt_get_header_len(method_nr);
    જો ((buffer = alloc(header_len)) == ખાલી)
	પાછા ખાલી;
    mch_memmove(buffer, magic_buffer, CRYPT_MAGIC_LEN);
    જો (header_len > CRYPT_MAGIC_LEN
	    && fread(buffer + CRYPT_MAGIC_LEN,
				    header_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(buffer);
	પાછા ખાલી;
    }

    state = crypt_create_from_header(method_nr, ચાવી, buffer);
    vim_free(buffer);
    પાછા state;
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
    પૂર્ણાંક	    method_nr,
    char_u  *ચાવી,
    char_u  **header,
    પૂર્ણાંક	    *header_len)
{
    પૂર્ણાંક	    len = crypt_get_header_len(method_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_FIELD(arg);
    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len  = cryptmethods[method_nr].add_len;
    arg.cat_init_from_file = FALSE;

    *header_len = len;
    *header = alloc(len);
    જો (*header == ખાલી)
	પાછા ખાલી;

    mch_memmove(*header, cryptmethods[method_nr].magic, CRYPT_MAGIC_LEN);
    જો (arg.cat_salt_len > 0 || arg.cat_seed_len > 0 || arg.cat_add_len > 0)
    {
	જો (arg.cat_salt_len > 0)
	    arg.cat_salt = *header + CRYPT_MAGIC_LEN;
	જો (arg.cat_seed_len > 0)
	    arg.cat_seed = *header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	જો (arg.cat_add_len > 0)
	    arg.cat_add = *header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#ifdef FEAT_SODIUM
	જો (sodium_init() >= 0)
	{
	    જો (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    જો (arg.cat_seed_len > 0)
		randombytes_buf(arg.cat_seed, arg.cat_seed_len);
	}
	નહીંતર
#endif
	    sha2_seed(arg.cat_salt, arg.cat_salt_len, arg.cat_seed, arg.cat_seed_len);
    }
    state = crypt_create(method_nr, ચાવી, &arg);
    જો (state == ખાલી)
	VIM_CLEAR(*header);
    પાછા state;
}

/*
 * Free the crypt state.
 */
    શૂન્ય
crypt_free_state(cryptstate_T *state)
{
#ifdef FEAT_SODIUM
    જો (crypt_method_is_sodium(state->method_nr))
    {
	sodium_munlock(((sodium_state_T *)state->method_state)->ચાવી,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->method_state, કદ(sodium_state_T));
	sodium_free(state->method_state);
    }
    નહીંતર
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
    પૂર્ણાંક		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    જો (method->encode_buffer_fn != ખાલી)
	// Has buffer function, pass through.
	પાછા method->encode_buffer_fn(state, from, len, newptr, last);
    જો (len == 0)
	// Not buffering, just return EOF.
	પાછા (long)len;

    *newptr = alloc(len + 50);
    જો (*newptr == ખાલી)
	પાછા -1;
    method->encode_fn(state, from, len, *newptr, last);
    પાછા (long)len;
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
    પૂર્ણાંક		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    જો (method->decode_buffer_fn != ખાલી)
	// Has buffer function, pass through.
	પાછા method->decode_buffer_fn(state, ptr, len, newptr, last);

    જો (len == 0)
	// Not buffering, just return EOF.
	પાછા len;

    *newptr = alloc(len);
    જો (*newptr == ખાલી)
	પાછા -1;
    method->decode_fn(state, ptr, len, *newptr, last);
    પાછા len;
}
#endif

/*
 * Encrypting "from[len]" into "to[len]".
 */
    શૂન્ય
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    પૂર્ણાંક		last)
{
    cryptmethods[state->method_nr].encode_fn(state, from, len, to, last);
}

#જો 0  // unused
/*
 * decrypting "from[len]" into "to[len]".
 */
    શૂન્ય
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    પૂર્ણાંક		last)
{
    cryptmethods[state->method_nr].decode_fn(state, from, len, to, last);
}
#endif

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    શૂન્ય
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    પૂર્ણાંક		last)
{
    cryptmethods[state->method_nr].encode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    શૂન્ય
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    પૂર્ણાંક		last)
{
    cryptmethods[state->method_nr].decode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    શૂન્ય
crypt_free_ચાવી(char_u *ચાવી)
{
    // Create a safe memset which cannot be optimized away by compiler
    સ્થિર શૂન્ય *(* અસ્થિર vim_memset_safe)(શૂન્ય *s, પૂર્ણાંક c, size_t n) =
	memset;
    જો (ચાવી != ખાલી)
    {
#ifdef FEAT_SODIUM
	જો (sodium_init() >= 0)
	    sodium_memzero(ચાવી, STRLEN(ચાવી));
	નહીંતર
#endif
	    vim_memset_safe(ચાવી, 0, STRLEN(ચાવી));
	vim_free(ચાવી);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    શૂન્ય
crypt_check_method(પૂર્ણાંક method)
{
    જો (method < CRYPT_M_BF2 || method == CRYPT_M_SOD)
    {
	સંદેશ_scroll = TRUE;
	સંદેશ(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    શૂન્ય
crypt_check_swapfile_curbuf(શૂન્ય)
{
#ifdef FEAT_SODIUM
    પૂર્ણાંક method = crypt_get_method_nr(curbuf);
    જો (crypt_method_is_sodium(method))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	set_option_કિંમત_give_err((char_u *)"swf", 0, ખાલી, OPT_LOCAL);
	સંદેશ_scroll = TRUE;
	સંદેશ(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#endif
}

    શૂન્ય
crypt_check_current_method(શૂન્ય)
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
crypt_get_ચાવી(
    પૂર્ણાંક		store,
    પૂર્ણાંક		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = ખાલી;
    પૂર્ણાંક		round;

    માટે (round = 0; ; ++round)
    {
	cmdline_star = TRUE;
	cmdline_row = સંદેશ_row;
	p1 = getcmdline_prompt(NUL, round == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		ખાલી);
	cmdline_star = FALSE;

	જો (p1 == ખાલી)
	    તોડો;

	જો (round == twice)
	{
	    જો (p2 != ખાલી && STRCMP(p1, p2) != 0)
	    {
		સંદેશ(_("Keys don't match!"));
		crypt_free_ચાવી(p1);
		crypt_free_ચાવી(p2);
		p2 = ખાલી;
		round = -1;		// do it again
		ચાલુ;
	    }

	    જો (store)
	    {
		set_option_કિંમત_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_ચાવી(p1);
		p1 = curbuf->b_p_ચાવી;
		crypt_check_swapfile_curbuf();
	    }
	    તોડો;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    જો (!crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
    {
	જો (સંદેશ_didout)
	    સંદેશ_putchar('\n');
	need_wait_return = FALSE;
	સંદેશ_didout = FALSE;
    }

    crypt_free_ચાવી(p2);
    પાછા p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    શૂન્ય
crypt_append_સંદેશ(
    buf_T *buf)
{
    જો (crypt_get_method_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    નહીંતર
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    સ્થિર પૂર્ણાંક
crypt_sodium_init_(
    cryptstate_T	*state UNUSED,
    char_u		*ચાવી UNUSED,
    crypt_arg_T		*arg UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned અક્ષર	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    પૂર્ણાંક			retval = 0;
    unsigned long long	opslimit;
    unsigned long long	memlimit;
    પૂર્ણાંક			alg;

    જો (sodium_init() < 0)
	પાછા FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(કદ(sodium_state_T));
    sodium_memzero(sd_state, કદ(sodium_state_T));

    જો ((state->method_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->method_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# જો 0
	// For testing
	જો (state->method_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	જો (crypto_pwhash(dkey, કદ(dkey), (અચલ અક્ષર *)ચાવી, STRLEN(ચાવી),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    પાછા FAIL;
	}
	memcpy(sd_state->ચાવી, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->ચાવી, crypto_box_SEEDBYTES);
	retval += sodium_mlock(ચાવી, STRLEN(ચાવી));

	જો (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    પાછા FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	જો (state->method_nr == CRYPT_M_SOD2 && arg->cat_add != ખાલી)
	{
	    char_u	buffer[20];
	    char_u	*p = buffer;
	    vim_memset(buffer, 0, 20);

	    crypt_long_long_to_char(opslimit, p);
	    p += કદ(opslimit);

	    crypt_long_long_to_char(memlimit, p);
	    p += કદ(memlimit);

	    crypt_int_to_char(alg, p);
	    memcpy(arg->cat_add, buffer, કદ(opslimit) + કદ(memlimit) + કદ(alg));
	}
    }
    નહીંતર
    {
	char_u	buffer[20];
	char_u	*p = buffer;
	vim_memset(buffer, 0, 20);
	પૂર્ણાંક	size = કદ(opslimit) +
	    કદ(memlimit) + કદ(alg);

	// Reading parameters from file
	જો (arg->cat_add_len < size)
	{
	    sodium_free(sd_state);
	    પાછા FAIL;
	}

	// derive the key from the file header
	memcpy(p, arg->cat_add, size);
	arg->cat_add += size;

	opslimit = crypt_char_to_long_long(p);
	p += કદ(opslimit);
	memlimit = crypt_char_to_long_long(p);
	p += કદ(memlimit);
	alg = crypt_char_to_int(p);
	p += કદ(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	જો (crypto_pwhash(dkey, કદ(dkey), (અચલ અક્ષર *)ચાવી, STRLEN(ચાવી),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    પાછા FAIL;
	}
	memcpy(sd_state->ચાવી, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->ચાવી, crypto_box_SEEDBYTES);
	retval += sodium_mlock(ચાવી, STRLEN(ચાવી));

	જો (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    પાછા FAIL;
	}
    }
    sd_state->ગણતરી = 0;
    state->method_state = sd_state;

    પાછા OK;
#નહીંતર
    emsg(_(e_libsodium_not_built_in));
    પાછા FAIL;
#endif
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#જો 0  // Currently unused
    શૂન્ય
crypt_sodium_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    પૂર્ણાંક		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned અક્ષર  tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;

    જો (sod_st->ગણતરી == 0)
    {
	જો (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_header));
	    પાછા;
	}
	crypto_secretstream_xchacha20poly1305_init_ધકેલો(&sod_st->state,
							      to, sod_st->ચાવી);
	to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    જો (sod_st->ગણતરી && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_buffer));
	પાછા;
    }

    crypto_secretstream_xchacha20poly1305_ધકેલો(&sod_st->state, to, ખાલી,
						      from, len, ખાલી, 0, tag);

    sod_st->ગણતરી++;
# endif
}
#endif

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#જો 0  // Currently unused
    શૂન્ય
crypt_sodium_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    પૂર્ણાંક		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned અક્ષર  tag;
    unsigned long long buf_len;
    char_u *p1 = from;
    char_u *p2 = to;
    char_u *buf_out;

    જો (sod_st->ગણતરી == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_header));
	પાછા;
    }

    buf_out = (char_u *)alloc(len);

    જો (buf_out == ખાલી)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	પાછા;
    }
    જો (sod_st->ગણતરી == 0)
    {
	જો (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->ચાવી) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    જાઓ fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	જો (p1 == p2)
	    to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    જો (sod_st->ગણતરી && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_buffer));
	જાઓ fail;
    }
    જો (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_out, &buf_len, &tag, from, len, ખાલી, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	જાઓ fail;
    }
    sod_st->ગણતરી++;

    જો (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	જાઓ fail;
    }
    જો (p1 == p2)
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
    સ્થિર long
crypt_sodium_buffer_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    પૂર્ણાંક		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned long long	out_len;
    char_u		*ptr;
    unsigned અક્ષર	tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;
    પૂર્ણાંક			length;
    sodium_state_T	*sod_st = state->method_state;
    પૂર્ણાંક			first = (sod_st->ગણતરી == 0);

    length = (પૂર્ણાંક)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (first ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_out = alloc_clear(length);
    જો (*buf_out == ખાલી)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	પાછા -1;
    }
    ptr = *buf_out;

    જો (first)
    {
	crypto_secretstream_xchacha20poly1305_init_ધકેલો(&sod_st->state,
		ptr, sod_st->ચાવી);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_ધકેલો(&sod_st->state, ptr,
	    &out_len, from, len, ખાલી, 0, tag);

    sod_st->ગણતરી++;
    પાછા out_len + (first
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#નહીંતર
    પાછા -1;
#endif
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    સ્થિર long
crypt_sodium_buffer_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    પૂર્ણાંક		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned અક્ષર  tag;
    unsigned long long out_len;

    જો (sod_st->ગણતરી == 0
	    && state->method_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].add_len;

    *buf_out = alloc_clear(len);
    જો (*buf_out == ખાલી)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	પાછા -1;
    }

    જો (sod_st->ગણતરી == 0)
    {
	જો (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->ચાવી) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    પાછા -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->ગણતરી++;
    }
    જો (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_out, &out_len, &tag, from, len, ખાલી, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	પાછા -1;
    }

    જો (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
	emsg(_(e_libsodium_decryption_failed_premature));
    પાછા (long) out_len;
#નહીંતર
    પાછા -1;
#endif
}

#જો defined(FEAT_SODIUM)
    શૂન્ય
crypt_sodium_lock_ચાવી(char_u *ચાવી)
{
    જો (sodium_init() >= 0)
	sodium_mlock(ચાવી, STRLEN(ચાવી));
}

    પૂર્ણાંક
crypt_sodium_munlock(શૂન્ય *અચલ addr, અચલ size_t len)
{
    પાછા sodium_munlock(addr, len);
}

    શૂન્ય
crypt_sodium_randombytes_buf(શૂન્ય *અચલ buf, અચલ size_t size)
{
    randombytes_buf(buf, size);
}

    પૂર્ણાંક
crypt_sodium_init(શૂન્ય)
{
    પાછા sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(શૂન્ય)
{
    પાછા randombytes_random();
}

# જો defined(FEAT_EVAL)
    સ્થિર શૂન્ય
crypt_sodium_report_hash_params(
	unsigned long long opslimit,
	unsigned long long ops_def,
	size_t memlimit,
	size_t mem_def,
	પૂર્ણાંક alg,
	પૂર્ણાંક alg_def)
{
    જો (p_verbose > 0)
    {
	verbose_enter();
	જો (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	નહીંતર
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	જો (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	નહીંતર
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	જો (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	નહીંતર
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    સ્થિર શૂન્ય
crypt_long_long_to_char(long long n, char_u *s)
{
    પૂર્ણાંક i;
    માટે (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    સ્થિર શૂન્ય
crypt_int_to_char(પૂર્ણાંક n, char_u *s)
{
    પૂર્ણાંક i;
    માટે (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    સ્થિર long long
crypt_char_to_long_long(char_u *s)
{
    unsigned long long    retval = 0;
    પૂર્ણાંક i;
    માટે (i = 7; i >= 0; i--)
    {
	જો (i == 7)
	    retval = s[i];
	નહીંતર
	    retval |= s[i];
	જો (i > 0)
	    retval <<= 8;
    }
    પાછા retval;
}

    સ્થિર પૂર્ણાંક
crypt_char_to_int(char_u *s)
{
    પૂર્ણાંક retval = 0;
    પૂર્ણાંક i;

    માટે (i = 3; i >= 0; i--)
    {
	જો (i == 3)
	    retval = s[i];
	નહીંતર
	    retval |= s[i];
	જો (i > 0)
	    retval <<= 8;
    }
    પાછા retval;
}
#endif

#endif // FEAT_CRYPT
