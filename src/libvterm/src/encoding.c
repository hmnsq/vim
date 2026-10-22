#include "vterm_internal.h"

#define UNICODE_INVALID 0xFFFD

#nếu defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#endif

struct UTF8DecoderData {
  // number of bytes remaining in this codepoint
  số_nguyên bytes_remaining;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  số_nguyên bytes_total;

  số_nguyên this_cp;
};

static void init_utf8(VTermEncoding *enc UNUSED, void *data_)
{
  struct UTF8DecoderData *data = data_;

  data->bytes_remaining = 0;
  data->bytes_total     = 0;
}

static void decode_utf8(VTermEncoding *enc UNUSED, void *data_,
                        uint32_t cp[], số_nguyên *cpi, số_nguyên cplen,
                        const char bytes[], size_t *pos, size_t bytelen)
{
  struct UTF8DecoderData *data = data_;

#ifdef DEBUG_PRINT_UTF8
  printf("BEGIN UTF-8\n");
#endif

  cho(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned char c = bytes[*pos];

#ifdef DEBUG_PRINT_UTF8
    printf(" pos=%zd c=%02x rem=%d\n", *pos, c, data->bytes_remaining);
#endif

    nếu(c < 0x20) // C0
      trả_về;

    không_thì nếu(c >= 0x20 && c < 0x7f) {
      nếu(data->bytes_remaining) {
        data->bytes_remaining = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	nếu (*cpi >= cplen)
	  break;
      }
      cp[(*cpi)++] = c;
#ifdef DEBUG_PRINT_UTF8
      printf(" UTF-8 char: U+%04x\n", c);
#endif
    }

    không_thì nếu(c == 0x7f) // DEL
      trả_về;

    không_thì nếu(c >= 0x80 && c < 0xc0) {
      nếu(!data->bytes_remaining) {
        cp[(*cpi)++] = UNICODE_INVALID;
        continue;
      }

      data->this_cp <<= 6;
      data->this_cp |= c & 0x3f;
      data->bytes_remaining--;

      nếu(!data->bytes_remaining) {
#ifdef DEBUG_PRINT_UTF8
        printf(" UTF-8 raw char U+%04x bytelen=%d ", data->this_cp, data->bytes_total);
#endif
        // Check for overlong sequences
        switch(data->bytes_total) {
        case 2:
          nếu(data->this_cp <  0x0080) data->this_cp = UNICODE_INVALID;
          break;
        case 3:
          nếu(data->this_cp <  0x0800) data->this_cp = UNICODE_INVALID;
          break;
        case 4:
          nếu(data->this_cp < 0x10000) data->this_cp = UNICODE_INVALID;
          break;
        case 5:
          nếu(data->this_cp < 0x200000) data->this_cp = UNICODE_INVALID;
          break;
        case 6:
          nếu(data->this_cp < 0x4000000) data->this_cp = UNICODE_INVALID;
          break;
        }
        // Now look for plain invalid ones
        nếu((data->this_cp >= 0xD800 && data->this_cp <= 0xDFFF) ||
           data->this_cp == 0xFFFE ||
           data->this_cp == 0xFFFF)
          data->this_cp = UNICODE_INVALID;
#ifdef DEBUG_PRINT_UTF8
        printf(" char: U+%04x\n", data->this_cp);
#endif
        cp[(*cpi)++] = data->this_cp;
      }
    }

    không_thì nếu(c >= 0xc0 && c < 0xe0) {
      nếu(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x1f;
      data->bytes_total = 2;
      data->bytes_remaining = 1;
    }

    không_thì nếu(c >= 0xe0 && c < 0xf0) {
      nếu(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x0f;
      data->bytes_total = 3;
      data->bytes_remaining = 2;
    }

    không_thì nếu(c >= 0xf0 && c < 0xf8) {
      nếu(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x07;
      data->bytes_total = 4;
      data->bytes_remaining = 3;
    }

    không_thì nếu(c >= 0xf8 && c < 0xfc) {
      nếu(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x03;
      data->bytes_total = 5;
      data->bytes_remaining = 4;
    }

    không_thì nếu(c >= 0xfc && c < 0xfe) {
      nếu(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x01;
      data->bytes_total = 6;
      data->bytes_remaining = 5;
    }

    không_thì {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

static VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

static void decode_usascii(VTermEncoding *enc UNUSED, void *data UNUSED,
                           uint32_t cp[], số_nguyên *cpi, số_nguyên cplen,
                           const char bytes[], size_t *pos, size_t bytelen)
{
  số_nguyên is_gr = bytes[*pos] & 0x80;

  cho(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned char c = bytes[*pos] ^ is_gr;

    nếu(c < 0x20 || c == 0x7f || c >= 0x80)
      trả_về;

    cp[(*cpi)++] = c;
  }
}

static VTermEncoding encoding_usascii = {
  NULL,           // init
  &decode_usascii // decode
};

struct StaticTableEncoding {
  const VTermEncoding enc;
  const uint32_t chars[128];
};

static void decode_table(VTermEncoding *enc, void *data UNUSED,
                         uint32_t cp[], số_nguyên *cpi, số_nguyên cplen,
                         const char bytes[], size_t *pos, size_t bytelen)
{
  struct StaticTableEncoding *table = (struct StaticTableEncoding *)enc;
  số_nguyên is_gr = bytes[*pos] & 0x80;

  cho(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned char c = bytes[*pos] ^ is_gr;

    nếu(c < 0x20 || c == 0x7f || c >= 0x80)
      trả_về;

    nếu(table->chars[c])
      cp[(*cpi)++] = table->chars[c];
    không_thì
      cp[(*cpi)++] = c;
  }
}

#include "encoding/DECdrawing.inc"
#include "encoding/uk.inc"

static struct {
  VTermEncodingType type;
  char designation;
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
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, char designation)
{
  cho(số_nguyên i = 0; encodings[i].designation; i++)
    nếu(encodings[i].type == type && encodings[i].designation == designation)
      trả_về encodings[i].enc;
  trả_về NULL;
}
