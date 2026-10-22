#include "vterm_internal.h"

#define UNICODE_INVALID 0xFFFD

#නම් defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#endif

sa_nracanaa UTF8DecoderData {
  // number of bytes remaining in this codepoint
  පූර්ණ_සංඛ්‍යා bytes_remaining;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  පූර්ණ_සංඛ්‍යා bytes_total;

  පූර්ණ_සංඛ්‍යා this_cp;
};

static void init_utf8(VTermEncoding *enc UNUSED, void *data_)
{
  sa_nracanaa UTF8DecoderData *data = data_;

  data->bytes_remaining = 0;
  data->bytes_total     = 0;
}

static void decode_utf8(VTermEncoding *enc UNUSED, void *data_,
                        uint32_t cp[], පූර්ණ_සංඛ්‍යා *cpi, පූර්ණ_සංඛ්‍යා cplen,
                        const var_na bytes[], size_t *pos, size_t bytelen)
{
  sa_nracanaa UTF8DecoderData *data = data_;

#ifdef DEBUG_PRINT_UTF8
  printf("BEGIN UTF-8\n");
#endif

  සඳහා(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned var_na c = bytes[*pos];

#ifdef DEBUG_PRINT_UTF8
    printf(" pos=%zd c=%02x rem=%d\n", *pos, c, data->bytes_remaining);
#endif

    නම්(c < 0x20) // C0
      ආපසු;

    නැතහොත් නම්(c >= 0x20 && c < 0x7f) {
      නම්(data->bytes_remaining) {
        data->bytes_remaining = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	නම් (*cpi >= cplen)
	  bha_nga;
      }
      cp[(*cpi)++] = c;
#ifdef DEBUG_PRINT_UTF8
      printf(" UTF-8 char: U+%04x\n", c);
#endif
    }

    නැතහොත් නම්(c == 0x7f) // DEL
      ආපසු;

    නැතහොත් නම්(c >= 0x80 && c < 0xc0) {
      නම්(!data->bytes_remaining) {
        cp[(*cpi)++] = UNICODE_INVALID;
        continue;
      }

      data->this_cp <<= 6;
      data->this_cp |= c & 0x3f;
      data->bytes_remaining--;

      නම්(!data->bytes_remaining) {
#ifdef DEBUG_PRINT_UTF8
        printf(" UTF-8 raw char U+%04x bytelen=%d ", data->this_cp, data->bytes_total);
#endif
        // Check for overlong sequences
        switch(data->bytes_total) {
        pak_sa 2:
          නම්(data->this_cp <  0x0080) data->this_cp = UNICODE_INVALID;
          bha_nga;
        pak_sa 3:
          නම්(data->this_cp <  0x0800) data->this_cp = UNICODE_INVALID;
          bha_nga;
        pak_sa 4:
          නම්(data->this_cp < 0x10000) data->this_cp = UNICODE_INVALID;
          bha_nga;
        pak_sa 5:
          නම්(data->this_cp < 0x200000) data->this_cp = UNICODE_INVALID;
          bha_nga;
        pak_sa 6:
          නම්(data->this_cp < 0x4000000) data->this_cp = UNICODE_INVALID;
          bha_nga;
        }
        // Now look for plain invalid ones
        නම්((data->this_cp >= 0xD800 && data->this_cp <= 0xDFFF) ||
           data->this_cp == 0xFFFE ||
           data->this_cp == 0xFFFF)
          data->this_cp = UNICODE_INVALID;
#ifdef DEBUG_PRINT_UTF8
        printf(" char: U+%04x\n", data->this_cp);
#endif
        cp[(*cpi)++] = data->this_cp;
      }
    }

    නැතහොත් නම්(c >= 0xc0 && c < 0xe0) {
      නම්(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x1f;
      data->bytes_total = 2;
      data->bytes_remaining = 1;
    }

    නැතහොත් නම්(c >= 0xe0 && c < 0xf0) {
      නම්(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x0f;
      data->bytes_total = 3;
      data->bytes_remaining = 2;
    }

    නැතහොත් නම්(c >= 0xf0 && c < 0xf8) {
      නම්(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x07;
      data->bytes_total = 4;
      data->bytes_remaining = 3;
    }

    නැතහොත් නම්(c >= 0xf8 && c < 0xfc) {
      නම්(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x03;
      data->bytes_total = 5;
      data->bytes_remaining = 4;
    }

    නැතහොත් නම්(c >= 0xfc && c < 0xfe) {
      නම්(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x01;
      data->bytes_total = 6;
      data->bytes_remaining = 5;
    }

    නැතහොත් {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

static VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

static void decode_usascii(VTermEncoding *enc UNUSED, void *data UNUSED,
                           uint32_t cp[], පූර්ණ_සංඛ්‍යා *cpi, පූර්ණ_සංඛ්‍යා cplen,
                           const var_na bytes[], size_t *pos, size_t bytelen)
{
  පූර්ණ_සංඛ්‍යා is_gr = bytes[*pos] & 0x80;

  සඳහා(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned var_na c = bytes[*pos] ^ is_gr;

    නම්(c < 0x20 || c == 0x7f || c >= 0x80)
      ආපසු;

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
                         uint32_t cp[], පූර්ණ_සංඛ්‍යා *cpi, පූර්ණ_සංඛ්‍යා cplen,
                         const var_na bytes[], size_t *pos, size_t bytelen)
{
  sa_nracanaa StaticTableEncoding *table = (sa_nracanaa StaticTableEncoding *)enc;
  පූර්ණ_සංඛ්‍යා is_gr = bytes[*pos] & 0x80;

  සඳහා(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned var_na c = bytes[*pos] ^ is_gr;

    නම්(c < 0x20 || c == 0x7f || c >= 0x80)
      ආපසු;

    නම්(table->chars[c])
      cp[(*cpi)++] = table->chars[c];
    නැතහොත්
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
  සඳහා(පූර්ණ_සංඛ්‍යා i = 0; encodings[i].designation; i++)
    නම්(encodings[i].type == type && encodings[i].designation == designation)
      ආපසු encodings[i].enc;
  ආපසු NULL;
}
