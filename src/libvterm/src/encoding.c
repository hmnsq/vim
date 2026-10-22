#include "vterm_internal.h"

#define UNICODE_INVALID 0xFFFD

#만약 defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#endif

struct UTF8DecoderData {
  // number of bytes remaining in this codepoint
  정수 bytes_remaining;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  정수 bytes_total;

  정수 this_cp;
};

static void init_utf8(VTermEncoding *enc UNUSED, void *data_)
{
  struct UTF8DecoderData *data = data_;

  data->bytes_remaining = 0;
  data->bytes_total     = 0;
}

static void decode_utf8(VTermEncoding *enc UNUSED, void *data_,
                        uint32_t cp[], 정수 *cpi, 정수 cplen,
                        const char bytes[], size_t *pos, size_t bytelen)
{
  struct UTF8DecoderData *data = data_;

#ifdef DEBUG_PRINT_UTF8
  printf("BEGIN UTF-8\n");
#endif

  위해(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned char c = bytes[*pos];

#ifdef DEBUG_PRINT_UTF8
    printf(" pos=%zd c=%02x rem=%d\n", *pos, c, data->bytes_remaining);
#endif

    만약(c < 0x20) // C0
      반환;

    아니면 만약(c >= 0x20 && c < 0x7f) {
      만약(data->bytes_remaining) {
        data->bytes_remaining = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	만약 (*cpi >= cplen)
	  break;
      }
      cp[(*cpi)++] = c;
#ifdef DEBUG_PRINT_UTF8
      printf(" UTF-8 char: U+%04x\n", c);
#endif
    }

    아니면 만약(c == 0x7f) // DEL
      반환;

    아니면 만약(c >= 0x80 && c < 0xc0) {
      만약(!data->bytes_remaining) {
        cp[(*cpi)++] = UNICODE_INVALID;
        continue;
      }

      data->this_cp <<= 6;
      data->this_cp |= c & 0x3f;
      data->bytes_remaining--;

      만약(!data->bytes_remaining) {
#ifdef DEBUG_PRINT_UTF8
        printf(" UTF-8 raw char U+%04x bytelen=%d ", data->this_cp, data->bytes_total);
#endif
        // Check for overlong sequences
        switch(data->bytes_total) {
        case 2:
          만약(data->this_cp <  0x0080) data->this_cp = UNICODE_INVALID;
          break;
        case 3:
          만약(data->this_cp <  0x0800) data->this_cp = UNICODE_INVALID;
          break;
        case 4:
          만약(data->this_cp < 0x10000) data->this_cp = UNICODE_INVALID;
          break;
        case 5:
          만약(data->this_cp < 0x200000) data->this_cp = UNICODE_INVALID;
          break;
        case 6:
          만약(data->this_cp < 0x4000000) data->this_cp = UNICODE_INVALID;
          break;
        }
        // Now look for plain invalid ones
        만약((data->this_cp >= 0xD800 && data->this_cp <= 0xDFFF) ||
           data->this_cp == 0xFFFE ||
           data->this_cp == 0xFFFF)
          data->this_cp = UNICODE_INVALID;
#ifdef DEBUG_PRINT_UTF8
        printf(" char: U+%04x\n", data->this_cp);
#endif
        cp[(*cpi)++] = data->this_cp;
      }
    }

    아니면 만약(c >= 0xc0 && c < 0xe0) {
      만약(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x1f;
      data->bytes_total = 2;
      data->bytes_remaining = 1;
    }

    아니면 만약(c >= 0xe0 && c < 0xf0) {
      만약(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x0f;
      data->bytes_total = 3;
      data->bytes_remaining = 2;
    }

    아니면 만약(c >= 0xf0 && c < 0xf8) {
      만약(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x07;
      data->bytes_total = 4;
      data->bytes_remaining = 3;
    }

    아니면 만약(c >= 0xf8 && c < 0xfc) {
      만약(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x03;
      data->bytes_total = 5;
      data->bytes_remaining = 4;
    }

    아니면 만약(c >= 0xfc && c < 0xfe) {
      만약(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x01;
      data->bytes_total = 6;
      data->bytes_remaining = 5;
    }

    아니면 {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

static VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

static void decode_usascii(VTermEncoding *enc UNUSED, void *data UNUSED,
                           uint32_t cp[], 정수 *cpi, 정수 cplen,
                           const char bytes[], size_t *pos, size_t bytelen)
{
  정수 is_gr = bytes[*pos] & 0x80;

  위해(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned char c = bytes[*pos] ^ is_gr;

    만약(c < 0x20 || c == 0x7f || c >= 0x80)
      반환;

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
                         uint32_t cp[], 정수 *cpi, 정수 cplen,
                         const char bytes[], size_t *pos, size_t bytelen)
{
  struct StaticTableEncoding *table = (struct StaticTableEncoding *)enc;
  정수 is_gr = bytes[*pos] & 0x80;

  위해(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned char c = bytes[*pos] ^ is_gr;

    만약(c < 0x20 || c == 0x7f || c >= 0x80)
      반환;

    만약(table->chars[c])
      cp[(*cpi)++] = table->chars[c];
    아니면
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
  위해(정수 i = 0; encodings[i].designation; i++)
    만약(encodings[i].type == type && encodings[i].designation == designation)
      반환 encodings[i].enc;
  반환 NULL;
}
