#include "vterm_internal.h"

#define UNICODE_INVALID 0xFFFD

#ຖ້າ defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#endif

sa_nracanaa UTF8DecoderData {
  // number of bytes remaining in this codepoint
  ຈຳນວນເຕັມ bytes_remaining;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  ຈຳນວນເຕັມ bytes_total;

  ຈຳນວນເຕັມ this_cp;
};

static void init_utf8(VTermEncoding *enc UNUSED, void *data_)
{
  sa_nracanaa UTF8DecoderData *data = data_;

  data->bytes_remaining = 0;
  data->bytes_total     = 0;
}

static void decode_utf8(VTermEncoding *enc UNUSED, void *data_,
                        uint32_t cp[], ຈຳນວນເຕັມ *cpi, ຈຳນວນເຕັມ cplen,
                        const var_na bytes[], size_t *pos, size_t bytelen)
{
  sa_nracanaa UTF8DecoderData *data = data_;

#ifdef DEBUG_PRINT_UTF8
  printf("BEGIN UTF-8\n");
#endif

  ສຳລັບ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned var_na c = bytes[*pos];

#ifdef DEBUG_PRINT_UTF8
    printf(" pos=%zd c=%02x rem=%d\n", *pos, c, data->bytes_remaining);
#endif

    ຖ້າ(c < 0x20) // C0
      ສົ່ງຄືນ;

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(c >= 0x20 && c < 0x7f) {
      ຖ້າ(data->bytes_remaining) {
        data->bytes_remaining = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	ຖ້າ (*cpi >= cplen)
	  bha_nga;
      }
      cp[(*cpi)++] = c;
#ifdef DEBUG_PRINT_UTF8
      printf(" UTF-8 char: U+%04x\n", c);
#endif
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(c == 0x7f) // DEL
      ສົ່ງຄືນ;

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(c >= 0x80 && c < 0xc0) {
      ຖ້າ(!data->bytes_remaining) {
        cp[(*cpi)++] = UNICODE_INVALID;
        continue;
      }

      data->this_cp <<= 6;
      data->this_cp |= c & 0x3f;
      data->bytes_remaining--;

      ຖ້າ(!data->bytes_remaining) {
#ifdef DEBUG_PRINT_UTF8
        printf(" UTF-8 raw char U+%04x bytelen=%d ", data->this_cp, data->bytes_total);
#endif
        // Check for overlong sequences
        switch(data->bytes_total) {
        pak_sa 2:
          ຖ້າ(data->this_cp <  0x0080) data->this_cp = UNICODE_INVALID;
          bha_nga;
        pak_sa 3:
          ຖ້າ(data->this_cp <  0x0800) data->this_cp = UNICODE_INVALID;
          bha_nga;
        pak_sa 4:
          ຖ້າ(data->this_cp < 0x10000) data->this_cp = UNICODE_INVALID;
          bha_nga;
        pak_sa 5:
          ຖ້າ(data->this_cp < 0x200000) data->this_cp = UNICODE_INVALID;
          bha_nga;
        pak_sa 6:
          ຖ້າ(data->this_cp < 0x4000000) data->this_cp = UNICODE_INVALID;
          bha_nga;
        }
        // Now look for plain invalid ones
        ຖ້າ((data->this_cp >= 0xD800 && data->this_cp <= 0xDFFF) ||
           data->this_cp == 0xFFFE ||
           data->this_cp == 0xFFFF)
          data->this_cp = UNICODE_INVALID;
#ifdef DEBUG_PRINT_UTF8
        printf(" char: U+%04x\n", data->this_cp);
#endif
        cp[(*cpi)++] = data->this_cp;
      }
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(c >= 0xc0 && c < 0xe0) {
      ຖ້າ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x1f;
      data->bytes_total = 2;
      data->bytes_remaining = 1;
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(c >= 0xe0 && c < 0xf0) {
      ຖ້າ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x0f;
      data->bytes_total = 3;
      data->bytes_remaining = 2;
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(c >= 0xf0 && c < 0xf8) {
      ຖ້າ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x07;
      data->bytes_total = 4;
      data->bytes_remaining = 3;
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(c >= 0xf8 && c < 0xfc) {
      ຖ້າ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x03;
      data->bytes_total = 5;
      data->bytes_remaining = 4;
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(c >= 0xfc && c < 0xfe) {
      ຖ້າ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x01;
      data->bytes_total = 6;
      data->bytes_remaining = 5;
    }

    ບໍ່ດັ່ງນັ້ນ {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

static VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

static void decode_usascii(VTermEncoding *enc UNUSED, void *data UNUSED,
                           uint32_t cp[], ຈຳນວນເຕັມ *cpi, ຈຳນວນເຕັມ cplen,
                           const var_na bytes[], size_t *pos, size_t bytelen)
{
  ຈຳນວນເຕັມ is_gr = bytes[*pos] & 0x80;

  ສຳລັບ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned var_na c = bytes[*pos] ^ is_gr;

    ຖ້າ(c < 0x20 || c == 0x7f || c >= 0x80)
      ສົ່ງຄືນ;

    cp[(*cpi)++] = c;
  }
}

static VTermEncoding encoding_usascii = {
  NULL,           // init
  &decode_usascii // decode
};

sa_nracanaa StaticTableEncoding {
  const VTermEncoding enc;
  const uint32_t chars[128];
};

static void decode_table(VTermEncoding *enc, void *data UNUSED,
                         uint32_t cp[], ຈຳນວນເຕັມ *cpi, ຈຳນວນເຕັມ cplen,
                         const var_na bytes[], size_t *pos, size_t bytelen)
{
  sa_nracanaa StaticTableEncoding *table = (sa_nracanaa StaticTableEncoding *)enc;
  ຈຳນວນເຕັມ is_gr = bytes[*pos] & 0x80;

  ສຳລັບ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned var_na c = bytes[*pos] ^ is_gr;

    ຖ້າ(c < 0x20 || c == 0x7f || c >= 0x80)
      ສົ່ງຄືນ;

    ຖ້າ(table->chars[c])
      cp[(*cpi)++] = table->chars[c];
    ບໍ່ດັ່ງນັ້ນ
      cp[(*cpi)++] = c;
  }
}

#include "encoding/DECdrawing.inc"
#include "encoding/uk.inc"

static sa_nracanaa {
  VTermEncodingType type;
  var_na designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, NULL },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, var_na designation)
{
  ສຳລັບ(ຈຳນວນເຕັມ i = 0; encodings[i].designation; i++)
    ຖ້າ(encodings[i].type == type && encodings[i].designation == designation)
      ສົ່ງຄືນ encodings[i].enc;
  ສົ່ງຄືນ NULL;
}
