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
#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_CRYPT)
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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ଅକ୍ଷର	*name;		// encryption name as used in 'cryptmethod'
    ଅକ୍ଷର	*magic;		// magic bytes stored in file header
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		salt_len;	// length of salt, or 0 when not using salt
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		seed_len;	// length of seed, or 0 when not using seed
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_len;	// additional length in the header needed for storing
				// custom data
#ifdef CRYPT_NOT_INPLACE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		works_inplace;	// encryption/decryption can be done in-place
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		whole_undofile;	// whole undo file is encrypted

    // Optional function pointer for a self-test.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*self_test_fn)(ଶୂନ୍ୟ);

    // Function pointer for initializing encryption/decryption.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା (* init_fn)(cryptstate_T *state, char_u *ଚାବି, crypt_arg_T *arg);

    // Function pointers for encoding/decoding from one buffer into another.
    // Optional, however, these or the _buffer ones should be configured.
    ଶୂନ୍ୟ (*encode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last);
    ଶୂନ୍ୟ (*decode_fn)(cryptstate_T *state, char_u *from, size_t len,
							char_u *to, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last);

    // Function pointers for encoding and decoding, can buffer data if needed.
    // Optional (however, these or the above should be configured).
    long (*encode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last);
    long (*decode_buffer_fn)(cryptstate_T *state, char_u *from, size_t len,
						    char_u **newptr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last);

    // Function pointers for in-place encoding and decoding, used for
    // crypt_*_inplace(). "from" and "to" arguments will be equal.
    // These may be the same as decode_fn and encode_fn above, however an
    // algorithm may implement them in a way that is not interchangeable with
    // the crypt_(en|de)code() interface (for example because it wishes to add
    // padding to files).
    // This method is used for swap and undo files which have a rigid format.
    ଶୂନ୍ୟ (*encode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last);
    ଶୂନ୍ୟ (*decode_inplace_fn)(cryptstate_T *state, char_u *p1, size_t len,
							char_u *p2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last);
} cryptmethod_T;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା crypt_sodium_init_(cryptstate_T *state, char_u *ଚାବି, crypt_arg_T *arg);
ସ୍ଥିର long crypt_sodium_buffer_decode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last);
ସ୍ଥିର long crypt_sodium_buffer_encode(cryptstate_T *state, char_u *from, size_t len, char_u **buf_out, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last);
#ଯଦି defined(FEAT_SODIUM)
ସ୍ଥିର ଶୂନ୍ୟ crypt_long_long_to_char(long long n, char_u *s);
ସ୍ଥିର ଶୂନ୍ୟ crypt_int_to_char(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, char_u *s);
ସ୍ଥିର long long crypt_char_to_long_long(char_u *s);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା crypt_char_to_int(char_u *s);
#endif
#ଯଦି defined(FEAT_EVAL) && defined(FEAT_SODIUM)
ସ୍ଥିର ଶୂନ୍ୟ crypt_sodium_report_hash_params(unsigned long long opslimit, unsigned long long ops_def, size_t memlimit, size_t mem_def, ପୂର୍ଣ୍ଣସଂଖ୍ୟା alg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା alg_def);
#endif

// index is method_nr of cryptstate_T, CRYPT_M_*
ସ୍ଥିର cryptmethod_T cryptmethods[CRYPT_M_ଗଣନା] = {
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
	ଖାଲି,
	crypt_zip_init,
	crypt_zip_encode, crypt_zip_decode,
	ଖାଲି, ଖାଲି,
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
	ଖାଲି, ଖାଲି,
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
	ଖାଲି, ଖାଲି,
	crypt_blowfish_encode, crypt_blowfish_decode,
    },

    // XChaCha20 using libsodium; implementation issues
    {
	"xchacha20",
	"VimCrypt~04!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#ଅନ୍ୟଥା
	16,
#endif
	8,
	0,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	ଖାଲି,
	crypt_sodium_init_,
	ଖାଲି, ଖାଲି,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	ଖାଲି, ଖାଲି,
    },
    // XChaCha20 using libsodium; stores parameters in header
    {
	"xchacha20v2",
	"VimCrypt~05!",
#ifdef FEAT_SODIUM
	crypto_pwhash_argon2id_SALTBYTES, // 16
#ଅନ୍ୟଥା
	16,
#endif
	8,
	// sizeof(crypto_pwhash_OPSLIMIT_INTERACTIVE + crypto_pwhash_MEMLIMIT_INTERACTIVE + crypto_pwhash_ALG_DEFAULT)
	20,
#ifdef CRYPT_NOT_INPLACE
	FALSE,
#endif
	FALSE,
	ଖାଲି,
	crypt_sodium_init_,
	ଖାଲି, ଖାଲି,
	crypt_sodium_buffer_encode, crypt_sodium_buffer_decode,
	ଖାଲି, ଖାଲି,
    },

    // NOTE: when adding a new method, use some random bytes for the magic key,
    // to avoid that a text file is recognized as encrypted.
};

#ଯଦି defined(FEAT_SODIUM)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    size_t	    ଗଣନା_2;
    unsigned ଅକ୍ଷର   ଚାବି[crypto_box_SEEDBYTES];
		  // 32, same as crypto_secretstream_xchacha20poly1305_KEYBYTES
    crypto_secretstream_xchacha20poly1305_state
		    state;
} sodium_state_T;


# ifdef DYNAMIC_SODIUM
#  ifdef MSWIN
#   define SODIUM_PROC FARPROC
#   define load_dll vimLoadLib
#   define ପ୍ରତୀକ_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  ଅନ୍ୟଥା
#   error Dynamic loading of libsodium is not supported ପାଇଁ now.
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
#  define crypto_secretstream_xchacha20poly1305_init_ଠେଲ \
    dll_crypto_secretstream_xchacha20poly1305_init_ଠେଲ
#  define crypto_secretstream_xchacha20poly1305_ଠେଲ \
    dll_crypto_secretstream_xchacha20poly1305_ଠେଲ
#  define crypto_secretstream_xchacha20poly1305_init_pull \
    dll_crypto_secretstream_xchacha20poly1305_init_pull
#  define crypto_secretstream_xchacha20poly1305_pull \
    dll_crypto_secretstream_xchacha20poly1305_pull
#  define crypto_pwhash	    dll_crypto_pwhash
#  define randombytes_buf   dll_randombytes_buf
#  define randombytes_random dll_randombytes_random

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_sodium_init)(ଶୂନ୍ୟ) = ଖାଲି;
ସ୍ଥିର ଶୂନ୍ୟ (*dll_sodium_free)(ଶୂନ୍ୟ *) = ଖାଲି;
ସ୍ଥିର ଶୂନ୍ୟ *(*dll_sodium_malloc)(ଅଚଳ size_t) = ଖାଲି;
ସ୍ଥିର ଶୂନ୍ୟ (*dll_sodium_memzero)(ଶୂନ୍ୟ * ଅଚଳ, ଅଚଳ size_t) = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_sodium_mlock)(ଶୂନ୍ୟ * ଅଚଳ, ଅଚଳ size_t) = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_sodium_munlock)(ଶୂନ୍ୟ * ଅଚଳ, ଅଚଳ size_t) = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_crypto_secretstream_xchacha20poly1305_init_ଠେଲ)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned ଅକ୍ଷର [],
    ଅଚଳ unsigned ଅକ୍ଷର []) = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_crypto_secretstream_xchacha20poly1305_ଠେଲ)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned ଅକ୍ଷର *c, unsigned long long *clen_p,
    ଅଚଳ unsigned ଅକ୍ଷର *m, unsigned long long mlen,
    ଅଚଳ unsigned ଅକ୍ଷର *ad, unsigned long long adlen, unsigned ଅକ୍ଷର tag)
    = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_crypto_secretstream_xchacha20poly1305_init_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    ଅଚଳ unsigned ଅକ୍ଷର [],
    ଅଚଳ unsigned ଅକ୍ଷର []) = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_crypto_secretstream_xchacha20poly1305_pull)
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned ଅକ୍ଷର *m, unsigned long long *mlen_p, unsigned ଅକ୍ଷର *tag_p,
    ଅଚଳ unsigned ଅକ୍ଷର *c, unsigned long long clen,
    ଅଚଳ unsigned ଅକ୍ଷର *ad, unsigned long long adlen) = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_crypto_pwhash)(unsigned ଅକ୍ଷର * ଅଚଳ out,
    unsigned long long outlen,
    ଅଚଳ ଅକ୍ଷର * ଅଚଳ passwd, unsigned long long passwdlen,
    ଅଚଳ unsigned ଅକ୍ଷର * ଅଚଳ salt,
    unsigned long long opslimit, size_t memlimit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା alg)
    = ଖାଲି;
ସ୍ଥିର ଶୂନ୍ୟ (*dll_randombytes_buf)(ଶୂନ୍ୟ * ଅଚଳ buf, ଅଚଳ size_t size);
ସ୍ଥିର uint32_t (*dll_randombytes_random)(ଶୂନ୍ୟ);

ସ୍ଥିର ସଂରଚନା {
    ଅଚଳ ଅକ୍ଷର *name;
    SODIUM_PROC *ptr;
} sodium_funcname_table[] = {
    {"sodium_init", (SODIUM_PROC*)&dll_sodium_init},
    {"sodium_free", (SODIUM_PROC*)&dll_sodium_free},
    {"sodium_malloc", (SODIUM_PROC*)&dll_sodium_malloc},
    {"sodium_memzero", (SODIUM_PROC*)&dll_sodium_memzero},
    {"sodium_mlock", (SODIUM_PROC*)&dll_sodium_mlock},
    {"sodium_munlock", (SODIUM_PROC*)&dll_sodium_munlock},
    {"crypto_secretstream_xchacha20poly1305_init_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_ଠେଲ},
    {"crypto_secretstream_xchacha20poly1305_push", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_ଠେଲ},
    {"crypto_secretstream_xchacha20poly1305_init_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_init_pull},
    {"crypto_secretstream_xchacha20poly1305_pull", (SODIUM_PROC*)&dll_crypto_secretstream_xchacha20poly1305_pull},
    {"crypto_pwhash", (SODIUM_PROC*)&dll_crypto_pwhash},
    {"randombytes_buf", (SODIUM_PROC*)&dll_randombytes_buf},
    {"randombytes_random", (SODIUM_PROC*)&dll_randombytes_random},
    {ଖାଲି, ଖାଲି}
};

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sodium_runtime_link_init(ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ସ୍ଥିର HINSTANCE hsodium = ଖାଲି;
    ଅଚଳ ଅକ୍ଷର *libname = DYNAMIC_SODIUM_DLL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (hsodium != ଖାଲି)
	ଫେରନ୍ତୁ OK;

    hsodium = load_dll(libname);
    ଯଦି (hsodium == ଖାଲି)
    {
	ଯଦି (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ଫେରନ୍ତୁ FAIL;
    }

    ପାଇଁ (i = 0; sodium_funcname_table[i].ptr; ++i)
    {
	ଯଦି ((*sodium_funcname_table[i].ptr = ପ୍ରତୀକ_from_dll(hsodium,
			sodium_funcname_table[i].name)) == ଖାଲି)
	{
	    close_dll(hsodium);
	    hsodium = ଖାଲି;
	    ଯଦି (verbose)
		semsg(_(e_could_not_load_library_function_str), sodium_funcname_table[i].name);
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
load_sodium(ଶୂନ୍ୟ)
{
    ଯଦି (sodium_runtime_link_init(TRUE) == FAIL)
	ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ dll_sodium_init();
}
# endif

# ଯଦି defined(DYNAMIC_SODIUM)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sodium_enabled(ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ଫେରନ୍ତୁ sodium_runtime_link_init(verbose) == OK;
}
# endif
#endif

#define CRYPT_MAGIC_LEN	12	// cannot change
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର crypt_magic_head[] = "VimCrypt~";

/*
 * Return int value for crypt method name.
 * 0 for "zip", the old method.  Also for any non-valid value.
 * 1 for "blowfish".
 * 2 for "blowfish2".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
crypt_method_nr_from_name(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < CRYPT_M_ଗଣନା; ++i)
	ଯଦି (STRCMP(name, cryptmethods[i].name) == 0)
	    ଫେରନ୍ତୁ i;
    ଫେରନ୍ତୁ 0;
}

/*
 * Get the crypt method used for a file from "ptr[len]", the magic text at the
 * start of the file.
 * Returns -1 when no encryption used.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
crypt_method_nr_from_magic(ଅକ୍ଷର *ptr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (len < CRYPT_MAGIC_LEN)
	ଫେରନ୍ତୁ -1;

    ପାଇଁ (i = 0; i < CRYPT_M_ଗଣନା; i++)
	ଯଦି (memcmp(ptr, cryptmethods[i].magic, CRYPT_MAGIC_LEN) == 0)
	    ଫେରନ୍ତୁ i;

    i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(crypt_magic_head);
    ଯଦି (len >= i && memcmp(ptr, crypt_magic_head, i) == 0)
	emsg(_(e_file_is_encrypted_with_unknown_method));

    ଫେରନ୍ତୁ -1;
}

#ifdef CRYPT_NOT_INPLACE
/*
 * Return TRUE if the crypt method for "method_nr" can be done in-place.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
crypt_works_inplace(cryptstate_T *state)
{
    ଫେରନ୍ତୁ cryptmethods[state->method_nr].works_inplace;
}
#endif

/*
 * Get the crypt method for buffer "buf" as a number.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
crypt_get_method_nr(buf_T *buf)
{
    ଫେରନ୍ତୁ crypt_method_nr_from_name(*buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
}

/*
 * Returns True for Sodium Encryption.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
crypt_method_is_sodium(ପୂର୍ଣ୍ଣସଂଖ୍ୟା method)
{
    ଫେରନ୍ତୁ method == CRYPT_M_SOD || method == CRYPT_M_SOD2;
}

/*
 * Return TRUE when the buffer uses an encryption method that encrypts the
 * whole undo file, not only the text.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
crypt_whole_undofile(ପୂର୍ଣ୍ଣସଂଖ୍ୟା method_nr)
{
    ଫେରନ୍ତୁ cryptmethods[method_nr].whole_undofile;
}

/*
 * Get crypt method specific length of the file header in bytes.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
crypt_get_header_len(ପୂର୍ଣ୍ଣସଂଖ୍ୟା method_nr)
{
    ଫେରନ୍ତୁ CRYPT_MAGIC_LEN
	+ cryptmethods[method_nr].salt_len
	+ cryptmethods[method_nr].seed_len
	+ cryptmethods[method_nr].add_len;
}


#ଯଦି defined(FEAT_SODIUM)
/*
 * Get maximum crypt method specific length of the file header in bytes.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
crypt_get_max_header_len(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା max = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା temp = 0;

    ପାଇଁ (i = 0; i < CRYPT_M_ଗଣନା; ++i)
    {
	temp = crypt_get_header_len(i);
	ଯଦି (temp > max)
	    max = temp;
    }
    ଫେରନ୍ତୁ max;
}
#endif

/*
 * Set the crypt method for buffer "buf" to "method_nr" using the int value as
 * returned by crypt_method_nr_from_name().
 */
    ଶୂନ୍ୟ
crypt_set_cm_option(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା method_nr)
{
    free_string_option(buf->b_p_cm);
    buf->b_p_cm = vim_strsave((char_u *)cryptmethods[method_nr].name);
}

/*
 * If the crypt method for the current buffer has a self-test, run it and
 * return OK/FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
crypt_self_test(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା method_nr = crypt_get_method_nr(curbuf);

    ଯଦି (cryptmethods[method_nr].self_test_fn == ଖାଲି)
	ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ cryptmethods[method_nr].self_test_fn();
}

/*
 * Allocate a crypt state and initialize it.
 * Return NULL for failure.
 */
    cryptstate_T *
crypt_create(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		method_nr,
    char_u	*ଚାବି,
    crypt_arg_T *crypt_arg)
{
    cryptstate_T *state = ALLOC_ONE(cryptstate_T);

    ଯଦି (state == ଖାଲି)
	ଫେରନ୍ତୁ state;

    state->method_nr = method_nr;
    ଯଦି (cryptmethods[method_nr].init_fn(state, ଚାବି, crypt_arg) == FAIL)
    {
	vim_free(state);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ state;
}

/*
 * Allocate a crypt state from a file header and initialize it.
 * Assumes that header contains at least the number of bytes that
 * crypt_get_header_len() returns for "method_nr".
 */
    cryptstate_T *
crypt_create_from_header(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		method_nr,
    char_u	*ଚାବି,
    char_u	*header)
{
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    arg.cat_init_from_file = TRUE;

    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len = cryptmethods[method_nr].add_len;
    ଯଦି (arg.cat_salt_len > 0)
	arg.cat_salt = header + CRYPT_MAGIC_LEN;
    ଯଦି (arg.cat_seed_len > 0)
	arg.cat_seed = header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
    ଯଦି (arg.cat_add_len > 0)
	arg.cat_add = header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

    ଫେରନ୍ତୁ crypt_create(method_nr, ଚାବି, &arg);
}

/*
 * Read the crypt method specific header data from "fp".
 * Return an allocated cryptstate_T or NULL on error.
 */
    cryptstate_T *
crypt_create_from_file(FILE *fp, char_u *ଚାବି)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		method_nr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		header_len;
    ଅକ୍ଷର	magic_buffer[CRYPT_MAGIC_LEN];
    char_u	*buffer;
    cryptstate_T *state;

    ଯଦି (fread(magic_buffer, CRYPT_MAGIC_LEN, 1, fp) != 1)
	ଫେରନ୍ତୁ ଖାଲି;
    method_nr = crypt_method_nr_from_magic(magic_buffer, CRYPT_MAGIC_LEN);
    ଯଦି (method_nr < 0)
	ଫେରନ୍ତୁ ଖାଲି;

    header_len = crypt_get_header_len(method_nr);
    ଯଦି ((buffer = alloc(header_len)) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    mch_memmove(buffer, magic_buffer, CRYPT_MAGIC_LEN);
    ଯଦି (header_len > CRYPT_MAGIC_LEN
	    && fread(buffer + CRYPT_MAGIC_LEN,
				    header_len - CRYPT_MAGIC_LEN, 1, fp) != 1)
    {
	vim_free(buffer);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    state = crypt_create_from_header(method_nr, ଚାବି, buffer);
    vim_free(buffer);
    ଫେରନ୍ତୁ state;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    method_nr,
    char_u  *ଚାବି,
    char_u  **header,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *header_len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = crypt_get_header_len(method_nr);
    crypt_arg_T arg;
    cryptstate_T *state;

    CLEAR_FIELD(arg);
    arg.cat_salt_len = cryptmethods[method_nr].salt_len;
    arg.cat_seed_len = cryptmethods[method_nr].seed_len;
    arg.cat_add_len  = cryptmethods[method_nr].add_len;
    arg.cat_init_from_file = FALSE;

    *header_len = len;
    *header = alloc(len);
    ଯଦି (*header == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    mch_memmove(*header, cryptmethods[method_nr].magic, CRYPT_MAGIC_LEN);
    ଯଦି (arg.cat_salt_len > 0 || arg.cat_seed_len > 0 || arg.cat_add_len > 0)
    {
	ଯଦି (arg.cat_salt_len > 0)
	    arg.cat_salt = *header + CRYPT_MAGIC_LEN;
	ଯଦି (arg.cat_seed_len > 0)
	    arg.cat_seed = *header + CRYPT_MAGIC_LEN + arg.cat_salt_len;
	ଯଦି (arg.cat_add_len > 0)
	    arg.cat_add = *header + CRYPT_MAGIC_LEN
					 + arg.cat_salt_len + arg.cat_seed_len;

	// TODO: Should this be crypt method specific? (Probably not worth
	// it).  sha2_seed is pretty bad for large amounts of entropy, so make
	// that into something which is suitable for anything.
#ifdef FEAT_SODIUM
	ଯଦି (sodium_init() >= 0)
	{
	    ଯଦି (arg.cat_salt_len > 0)
		randombytes_buf(arg.cat_salt, arg.cat_salt_len);
	    ଯଦି (arg.cat_seed_len > 0)
		randombytes_buf(arg.cat_seed, arg.cat_seed_len);
	}
	ଅନ୍ୟଥା
#endif
	    sha2_seed(arg.cat_salt, arg.cat_salt_len, arg.cat_seed, arg.cat_seed_len);
    }
    state = crypt_create(method_nr, ଚାବି, &arg);
    ଯଦି (state == ଖାଲି)
	VIM_CLEAR(*header);
    ଫେରନ୍ତୁ state;
}

/*
 * Free the crypt state.
 */
    ଶୂନ୍ୟ
crypt_free_state(cryptstate_T *state)
{
#ifdef FEAT_SODIUM
    ଯଦି (crypt_method_is_sodium(state->method_nr))
    {
	sodium_munlock(((sodium_state_T *)state->method_state)->ଚାବି,
							 crypto_box_SEEDBYTES);
	sodium_memzero(state->method_state, ଆକାର(sodium_state_T));
	sodium_free(state->method_state);
    }
    ଅନ୍ୟଥା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    ଯଦି (method->encode_buffer_fn != ଖାଲି)
	// Has buffer function, pass through.
	ଫେରନ୍ତୁ method->encode_buffer_fn(state, from, len, newptr, last);
    ଯଦି (len == 0)
	// Not buffering, just return EOF.
	ଫେରନ୍ତୁ (long)len;

    *newptr = alloc(len + 50);
    ଯଦି (*newptr == ଖାଲି)
	ଫେରନ୍ତୁ -1;
    method->encode_fn(state, from, len, *newptr, last);
    ଫେରନ୍ତୁ (long)len;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last)
{
    cryptmethod_T *method = &cryptmethods[state->method_nr];

    ଯଦି (method->decode_buffer_fn != ଖାଲି)
	// Has buffer function, pass through.
	ଫେରନ୍ତୁ method->decode_buffer_fn(state, ptr, len, newptr, last);

    ଯଦି (len == 0)
	// Not buffering, just return EOF.
	ଫେରନ୍ତୁ len;

    *newptr = alloc(len);
    ଯଦି (*newptr == ଖାଲି)
	ଫେରନ୍ତୁ -1;
    method->decode_fn(state, ptr, len, *newptr, last);
    ଫେରନ୍ତୁ len;
}
#endif

/*
 * Encrypting "from[len]" into "to[len]".
 */
    ଶୂନ୍ୟ
crypt_encode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last)
{
    cryptmethods[state->method_nr].encode_fn(state, from, len, to, last);
}

#ଯଦି 0  // unused
/*
 * decrypting "from[len]" into "to[len]".
 */
    ଶୂନ୍ୟ
crypt_decode(
    cryptstate_T *state,
    char_u	*from,
    size_t	len,
    char_u	*to,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last)
{
    cryptmethods[state->method_nr].decode_fn(state, from, len, to, last);
}
#endif

/*
 * Simple inplace encryption, modifies "buf[len]" in place.
 */
    ଶୂନ୍ୟ
crypt_encode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last)
{
    cryptmethods[state->method_nr].encode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Simple inplace decryption, modifies "buf[len]" in place.
 */
    ଶୂନ୍ୟ
crypt_decode_inplace(
    cryptstate_T *state,
    char_u	*buf,
    size_t	len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last)
{
    cryptmethods[state->method_nr].decode_inplace_fn(state, buf, len,
								    buf, last);
}

/*
 * Free an allocated crypt key.  Clear the text to make sure it doesn't stay
 * in memory anywhere.
 */
    ଶୂନ୍ୟ
crypt_free_ଚାବି(char_u *ଚାବି)
{
    // Create a safe memset which cannot be optimized away by compiler
    ସ୍ଥିର ଶୂନ୍ୟ *(* ଅସ୍ଥିର vim_memset_safe)(ଶୂନ୍ୟ *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, size_t n) =
	memset;
    ଯଦି (ଚାବି != ଖାଲି)
    {
#ifdef FEAT_SODIUM
	ଯଦି (sodium_init() >= 0)
	    sodium_memzero(ଚାବି, STRLEN(ଚାବି));
	ଅନ୍ୟଥା
#endif
	    vim_memset_safe(ଚାବି, 0, STRLEN(ଚାବି));
	vim_free(ଚାବି);
    }
}

/*
 * Check the crypt method and give a warning if it's outdated.
 */
    ଶୂନ୍ୟ
crypt_check_method(ପୂର୍ଣ୍ଣସଂଖ୍ୟା method)
{
    ଯଦି (method < CRYPT_M_BF2 || method == CRYPT_M_SOD)
    {
	ସନ୍ଦେଶ_scroll = TRUE;
	ସନ୍ଦେଶ(_("Warning: Using a weak encryption method; see :help 'cm'"));
    }
}

/*
 * If the crypt method for "curbuf" does not support encrypting the swap file
 * then disable the swap file.
 */
    ଶୂନ୍ୟ
crypt_check_swapfile_curbuf(ଶୂନ୍ୟ)
{
#ifdef FEAT_SODIUM
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା method = crypt_get_method_nr(curbuf);
    ଯଦି (crypt_method_is_sodium(method))
    {
	// encryption uses padding and MAC, that does not work very well with
	// swap and undo files, so disable them
	mf_close_file(curbuf, TRUE);	// remove the swap file
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"swf", 0, ଖାଲି, OPT_LOCAL);
	ସନ୍ଦେଶ_scroll = TRUE;
	ସନ୍ଦେଶ(_("Note: Encryption of swapfile not supported, disabling swap file"));
    }
#endif
}

    ଶୂନ୍ୟ
crypt_check_current_method(ଶୂନ୍ୟ)
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
crypt_get_ଚାବି(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		store,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		twice)	    // Ask for the key twice.
{
    char_u	*p1, *p2 = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;

    ପାଇଁ (round = 0; ; ++round)
    {
	cmdline_star = TRUE;
	cmdline_row = ସନ୍ଦେଶ_row;
	p1 = getcmdline_prompt(NUL, round == 0
		? (char_u *)_("Enter encryption key: ")
		: (char_u *)_("Enter same key again: "), 0, EXPAND_NOTHING,
		ଖାଲି);
	cmdline_star = FALSE;

	ଯଦି (p1 == ଖାଲି)
	    ଭାଙ୍ଗ;

	ଯଦି (round == twice)
	{
	    ଯଦି (p2 != ଖାଲି && STRCMP(p1, p2) != 0)
	    {
		ସନ୍ଦେଶ(_("Keys don't match!"));
		crypt_free_ଚାବି(p1);
		crypt_free_ଚାବି(p2);
		p2 = ଖାଲି;
		round = -1;		// do it again
		ଜାରି;
	    }

	    ଯଦି (store)
	    {
		set_option_ମୂଲ୍ୟ_give_err((char_u *)"key", 0L, p1, OPT_LOCAL);
		crypt_free_ଚାବି(p1);
		p1 = curbuf->b_p_ଚାବି;
		crypt_check_swapfile_curbuf();
	    }
	    ଭାଙ୍ଗ;
	}
	p2 = p1;
    }

    // since the user typed this, no need to wait for return
    ଯଦି (!crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
    {
	ଯଦି (ସନ୍ଦେଶ_didout)
	    ସନ୍ଦେଶ_putchar('\n');
	need_wait_return = FALSE;
	ସନ୍ଦେଶ_didout = FALSE;
    }

    crypt_free_ଚାବି(p2);
    ଫେରନ୍ତୁ p1;
}


/*
 * Append a message to IObuff for the encryption/decryption method being used.
 */
    ଶୂନ୍ୟ
crypt_append_ସନ୍ଦେଶ(
    buf_T *buf)
{
    ଯଦି (crypt_get_method_nr(buf) == 0)
	STRCAT(IObuff, _("[crypted]"));
    ଅନ୍ୟଥା
    {
	STRCAT(IObuff, "[");
	STRCAT(IObuff, *buf->b_p_cm == NUL ? p_cm : buf->b_p_cm);
	STRCAT(IObuff, "]");
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
crypt_sodium_init_(
    cryptstate_T	*state UNUSED,
    char_u		*ଚାବି UNUSED,
    crypt_arg_T		*arg UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned ଅକ୍ଷର	dkey[crypto_box_SEEDBYTES]; // 32
    sodium_state_T	*sd_state;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			retval = 0;
    unsigned long long	opslimit;
    unsigned long long	memlimit;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			alg;

    ଯଦି (sodium_init() < 0)
	ଫେରନ୍ତୁ FAIL;

    sd_state = (sodium_state_T *)sodium_malloc(ଆକାର(sodium_state_T));
    sodium_memzero(sd_state, ଆକାର(sodium_state_T));

    ଯଦି ((state->method_nr == CRYPT_M_SOD2 && !arg->cat_init_from_file)
	    || state->method_nr == CRYPT_M_SOD)
    {
	opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
	memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
	alg = crypto_pwhash_ALG_DEFAULT;

# ଯଦି 0
	// For testing
	ଯଦି (state->method_nr == CRYPT_M_SOD2)
	{
	    opslimit = crypto_pwhash_OPSLIMIT_MODERATE;
	    memlimit = crypto_pwhash_MEMLIMIT_MODERATE;
	}
# endif

	// derive a key from the password
	ଯଦି (crypto_pwhash(dkey, ଆକାର(dkey), (ଅଚଳ ଅକ୍ଷର *)ଚାବି, STRLEN(ଚାବି),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    ଫେରନ୍ତୁ FAIL;
	}
	memcpy(sd_state->ଚାବି, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->ଚାବି, crypto_box_SEEDBYTES);
	retval += sodium_mlock(ଚାବି, STRLEN(ଚାବି));

	ଯଦି (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    ଫେରନ୍ତୁ FAIL;
	}
	// "cat_add" should not be NULL, check anyway for safety
	ଯଦି (state->method_nr == CRYPT_M_SOD2 && arg->cat_add != ଖାଲି)
	{
	    char_u	buffer[20];
	    char_u	*p = buffer;
	    vim_memset(buffer, 0, 20);

	    crypt_long_long_to_char(opslimit, p);
	    p += ଆକାର(opslimit);

	    crypt_long_long_to_char(memlimit, p);
	    p += ଆକାର(memlimit);

	    crypt_int_to_char(alg, p);
	    memcpy(arg->cat_add, buffer, ଆକାର(opslimit) + ଆକାର(memlimit) + ଆକାର(alg));
	}
    }
    ଅନ୍ୟଥା
    {
	char_u	buffer[20];
	char_u	*p = buffer;
	vim_memset(buffer, 0, 20);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	size = ଆକାର(opslimit) +
	    ଆକାର(memlimit) + ଆକାର(alg);

	// Reading parameters from file
	ଯଦି (arg->cat_add_len < size)
	{
	    sodium_free(sd_state);
	    ଫେରନ୍ତୁ FAIL;
	}

	// derive the key from the file header
	memcpy(p, arg->cat_add, size);
	arg->cat_add += size;

	opslimit = crypt_char_to_long_long(p);
	p += ଆକାର(opslimit);
	memlimit = crypt_char_to_long_long(p);
	p += ଆକାର(memlimit);
	alg = crypt_char_to_int(p);
	p += ଆକାର(alg);

# ifdef FEAT_EVAL
	crypt_sodium_report_hash_params(opslimit,
					    crypto_pwhash_OPSLIMIT_INTERACTIVE,
		(size_t)memlimit, crypto_pwhash_MEMLIMIT_INTERACTIVE,
		alg, crypto_pwhash_ALG_DEFAULT);
# endif

	ଯଦି (crypto_pwhash(dkey, ଆକାର(dkey), (ଅଚଳ ଅକ୍ଷର *)ଚାବି, STRLEN(ଚାବି),
			  arg->cat_salt, opslimit, (size_t)memlimit, alg) != 0)
	{
	    // out of memory
	    sodium_free(sd_state);
	    ଫେରନ୍ତୁ FAIL;
	}
	memcpy(sd_state->ଚାବି, dkey, crypto_box_SEEDBYTES);

	retval += sodium_mlock(sd_state->ଚାବି, crypto_box_SEEDBYTES);
	retval += sodium_mlock(ଚାବି, STRLEN(ଚାବି));

	ଯଦି (retval < 0)
	{
	    emsg(_(e_encryption_sodium_mlock_failed));
	    sodium_free(sd_state);
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    sd_state->ଗଣନା_2 = 0;
    state->method_state = sd_state;

    ଫେରନ୍ତୁ OK;
#ଅନ୍ୟଥା
    emsg(_(e_libsodium_not_built_in));
    ଫେରନ୍ତୁ FAIL;
#endif
}

/*
 * Encrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 * Call needs to ensure that there is enough space in to (for the header)
 */
#ଯଦି 0  // Currently unused
    ଶୂନ୍ୟ
crypt_sodium_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES == crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned ଅକ୍ଷର  tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;

    ଯଦି (sod_st->ଗଣନା_2 == 0)
    {
	ଯଦି (len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
	{
	    emsg(_(e_libsodium_cannot_encrypt_header));
	    ଫେରନ୍ତୁ;
	}
	crypto_secretstream_xchacha20poly1305_init_ଠେଲ(&sod_st->state,
							      to, sod_st->ଚାବି);
	to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    ଯଦି (sod_st->ଗଣନା_2 && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_encrypt_buffer));
	ଫେରନ୍ତୁ;
    }

    crypto_secretstream_xchacha20poly1305_ଠେଲ(&sod_st->state, to, ଖାଲି,
						      from, len, ଖାଲି, 0, tag);

    sod_st->ଗଣନା_2++;
# endif
}
#endif

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
#ଯଦି 0  // Currently unused
    ଶୂନ୍ୟ
crypt_sodium_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	*to UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last UNUSED)
{
# ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned ଅକ୍ଷର  tag;
    unsigned long long buf_len;
    char_u *p1 = from;
    char_u *p2 = to;
    char_u *buf_out;

    ଯଦି (sod_st->ଗଣନା_2 == 0
		   && len <= crypto_secretstream_xchacha20poly1305_HEADERBYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_header));
	ଫେରନ୍ତୁ;
    }

    buf_out = (char_u *)alloc(len);

    ଯଦି (buf_out == ଖାଲି)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (sod_st->ଗଣନା_2 == 0)
    {
	ଯଦି (crypto_secretstream_xchacha20poly1305_init_pull(
				       &sod_st->state, from, sod_st->ଚାବି) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    ଯାଅ fail;
	}

	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;

	ଯଦି (p1 == p2)
	    to += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    ଯଦି (sod_st->ଗଣନା_2 && len <= crypto_secretstream_xchacha20poly1305_ABYTES)
    {
	emsg(_(e_libsodium_cannot_decrypt_buffer));
	ଯାଅ fail;
    }
    ଯଦି (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			     buf_out, &buf_len, &tag, from, len, ଖାଲି, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	ଯାଅ fail;
    }
    sod_st->ଗଣନା_2++;

    ଯଦି (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
    {
	emsg(_(e_libsodium_decryption_failed_premature));
	ଯାଅ fail;
    }
    ଯଦି (p1 == p2)
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
    ସ୍ଥିର long
crypt_sodium_buffer_encode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    unsigned long long	out_len;
    char_u		*ptr;
    unsigned ଅକ୍ଷର	tag = last
			? crypto_secretstream_xchacha20poly1305_TAG_FINAL  : 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			length;
    sodium_state_T	*sod_st = state->method_state;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			first = (sod_st->ଗଣନା_2 == 0);

    length = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len + crypto_secretstream_xchacha20poly1305_ABYTES
	     + (first ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
    *buf_out = alloc_clear(length);
    ଯଦି (*buf_out == ଖାଲି)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	ଫେରନ୍ତୁ -1;
    }
    ptr = *buf_out;

    ଯଦି (first)
    {
	crypto_secretstream_xchacha20poly1305_init_ଠେଲ(&sod_st->state,
		ptr, sod_st->ଚାବି);
	ptr += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    }

    crypto_secretstream_xchacha20poly1305_ଠେଲ(&sod_st->state, ptr,
	    &out_len, from, len, ଖାଲି, 0, tag);

    sod_st->ଗଣନା_2++;
    ଫେରନ୍ତୁ out_len + (first
		      ? crypto_secretstream_xchacha20poly1305_HEADERBYTES : 0);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ -1;
#endif
}

/*
 * Decrypt "from[len]" into "to[len]".
 * "from" and "to" can be equal to encrypt in place.
 */
    ସ୍ଥିର long
crypt_sodium_buffer_decode(
    cryptstate_T *state UNUSED,
    char_u	*from UNUSED,
    size_t	len UNUSED,
    char_u	**buf_out UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last UNUSED)
{
#ifdef FEAT_SODIUM
    // crypto_box_SEEDBYTES ==  crypto_secretstream_xchacha20poly1305_KEYBYTES
    sodium_state_T *sod_st = state->method_state;
    unsigned ଅକ୍ଷର  tag;
    unsigned long long out_len;

    ଯଦି (sod_st->ଗଣନା_2 == 0
	    && state->method_nr == CRYPT_M_SOD
	    && len > WRITEBUFSIZE
		+ crypto_secretstream_xchacha20poly1305_HEADERBYTES
		+ crypto_secretstream_xchacha20poly1305_ABYTES)
	len -= cryptmethods[CRYPT_M_SOD2].add_len;

    *buf_out = alloc_clear(len);
    ଯଦି (*buf_out == ଖାଲି)
    {
	emsg(_(e_libsodium_cannot_allocate_buffer));
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (sod_st->ଗଣନା_2 == 0)
    {
	ଯଦି (len < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
		crypto_secretstream_xchacha20poly1305_init_pull(&sod_st->state,
						       from, sod_st->ଚାବି) != 0)
	{
	    emsg(_(e_libsodium_decryption_failed_header_incomplete));
	    ଫେରନ୍ତୁ -1;
	}
	from += crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	len -= crypto_secretstream_xchacha20poly1305_HEADERBYTES;
	sod_st->ଗଣନା_2++;
    }
    ଯଦି (crypto_secretstream_xchacha20poly1305_pull(&sod_st->state,
			    *buf_out, &out_len, &tag, from, len, ଖାଲି, 0) != 0)
    {
	emsg(_(e_libsodium_decryption_failed));
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !last)
	emsg(_(e_libsodium_decryption_failed_premature));
    ଫେରନ୍ତୁ (long) out_len;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ -1;
#endif
}

#ଯଦି defined(FEAT_SODIUM)
    ଶୂନ୍ୟ
crypt_sodium_lock_ଚାବି(char_u *ଚାବି)
{
    ଯଦି (sodium_init() >= 0)
	sodium_mlock(ଚାବି, STRLEN(ଚାବି));
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
crypt_sodium_munlock(ଶୂନ୍ୟ *ଅଚଳ addr, ଅଚଳ size_t len)
{
    ଫେରନ୍ତୁ sodium_munlock(addr, len);
}

    ଶୂନ୍ୟ
crypt_sodium_randombytes_buf(ଶୂନ୍ୟ *ଅଚଳ buf, ଅଚଳ size_t size)
{
    randombytes_buf(buf, size);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
crypt_sodium_init(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ sodium_init();
}

    UINT32_T
crypt_sodium_randombytes_random(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ randombytes_random();
}

# ଯଦି defined(FEAT_EVAL)
    ସ୍ଥିର ଶୂନ୍ୟ
crypt_sodium_report_hash_params(
	unsigned long long opslimit,
	unsigned long long ops_def,
	size_t memlimit,
	size_t mem_def,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା alg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା alg_def)
{
    ଯଦି (p_verbose > 0)
    {
	verbose_enter();
	ଯଦି (opslimit != ops_def)
	    smsg(_("xchacha20v2: using custom opslimit \"%llu\" for Key derivation."), opslimit);
	ଅନ୍ୟଥା
	    smsg(_("xchacha20v2: using default opslimit \"%llu\" for Key derivation."), opslimit);
	ଯଦି (memlimit != mem_def)
	    smsg(_("xchacha20v2: using custom memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	ଅନ୍ୟଥା
	    smsg(_("xchacha20v2: using default memlimit \"%lu\" for Key derivation."), (unsigned long)memlimit);
	ଯଦି (alg != alg_def)
	    smsg(_("xchacha20v2: using custom algorithm \"%d\" for Key derivation."), alg);
	ଅନ୍ୟଥା
	    smsg(_("xchacha20v2: using default algorithm \"%d\" for Key derivation."), alg);
	verbose_leave();
    }
}
# endif

    ସ୍ଥିର ଶୂନ୍ୟ
crypt_long_long_to_char(long long n, char_u *s)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପାଇଁ (i = 0; i < 8; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
crypt_int_to_char(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, char_u *s)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପାଇଁ (i = 0; i < 4; i++)
    {
	s[i] = (char_u)(n & 0xff);
	n = (unsigned long long)n >> 8;
    }
}

    ସ୍ଥିର long long
crypt_char_to_long_long(char_u *s)
{
    unsigned long long    retval = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପାଇଁ (i = 7; i >= 0; i--)
    {
	ଯଦି (i == 7)
	    retval = s[i];
	ଅନ୍ୟଥା
	    retval |= s[i];
	ଯଦି (i > 0)
	    retval <<= 8;
    }
    ଫେରନ୍ତୁ retval;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
crypt_char_to_int(char_u *s)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 3; i >= 0; i--)
    {
	ଯଦି (i == 3)
	    retval = s[i];
	ଅନ୍ୟଥା
	    retval |= s[i];
	ଯଦି (i > 0)
	    retval <<= 8;
    }
    ଫେରନ୍ତୁ retval;
}
#endif

#endif // FEAT_CRYPT
