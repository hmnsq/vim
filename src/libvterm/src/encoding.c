#ضمن "vterm_internal.h"

#define UNICODE_INVALID 0xFFFD

#𐤀𐤌 defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#endif

بنية UTF8DecoderData {
  // number of bytes remaining in this codepoint
  صحيح bytes_remaining;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  صحيح bytes_total;

  صحيح this_cp;
};

ساكن فراغ init_utf8(VTermEncoding *enc UNUSED, فراغ *data_)
{
  بنية UTF8DecoderData *data = data_;

  data->bytes_remaining = 0;
  data->bytes_total     = 0;
}

ساكن فراغ decode_utf8(VTermEncoding *enc UNUSED, فراغ *data_,
                        uint32_t cp[], صحيح *cpi, صحيح cplen,
                        ثابت حرف bytes[], size_t *pos, size_t bytelen)
{
  بنية UTF8DecoderData *data = data_;

#ifdef DEBUG_PRINT_UTF8
  printf("BEGIN UTF-8\n");
#endif

  لكل(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned حرف c = bytes[*pos];

#ifdef DEBUG_PRINT_UTF8
    printf(" pos=%zd c=%02x rem=%d\n", *pos, c, data->bytes_remaining);
#endif

    𐤀𐤌(c < 0x20) // C0
      أرجع;

    وإلا 𐤀𐤌(c >= 0x20 && c < 0x7f) {
      𐤀𐤌(data->bytes_remaining) {
        data->bytes_remaining = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	𐤀𐤌 (*cpi >= cplen)
	  اكسر;
      }
      cp[(*cpi)++] = c;
#ifdef DEBUG_PRINT_UTF8
      printf(" UTF-8 char: U+%04x\n", c);
#endif
    }

    وإلا 𐤀𐤌(c == 0x7f) // DEL
      أرجع;

    وإلا 𐤀𐤌(c >= 0x80 && c < 0xc0) {
      𐤀𐤌(!data->bytes_remaining) {
        cp[(*cpi)++] = UNICODE_INVALID;
        استمر;
      }

      data->this_cp <<= 6;
      data->this_cp |= c & 0x3f;
      data->bytes_remaining--;

      𐤀𐤌(!data->bytes_remaining) {
#ifdef DEBUG_PRINT_UTF8
        printf(" UTF-8 raw char U+%04x bytelen=%d ", data->this_cp, data->bytes_total);
#endif
        // Check for overlong sequences
        بدل(data->bytes_total) {
        حالة 2:
          𐤀𐤌(data->this_cp <  0x0080) data->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 3:
          𐤀𐤌(data->this_cp <  0x0800) data->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 4:
          𐤀𐤌(data->this_cp < 0x10000) data->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 5:
          𐤀𐤌(data->this_cp < 0x200000) data->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 6:
          𐤀𐤌(data->this_cp < 0x4000000) data->this_cp = UNICODE_INVALID;
          اكسر;
        }
        // Now look for plain invalid ones
        𐤀𐤌((data->this_cp >= 0xD800 && data->this_cp <= 0xDFFF) ||
           data->this_cp == 0xFFFE ||
           data->this_cp == 0xFFFF)
          data->this_cp = UNICODE_INVALID;
#ifdef DEBUG_PRINT_UTF8
        printf(" char: U+%04x\n", data->this_cp);
#endif
        cp[(*cpi)++] = data->this_cp;
      }
    }

    وإلا 𐤀𐤌(c >= 0xc0 && c < 0xe0) {
      𐤀𐤌(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x1f;
      data->bytes_total = 2;
      data->bytes_remaining = 1;
    }

    وإلا 𐤀𐤌(c >= 0xe0 && c < 0xf0) {
      𐤀𐤌(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x0f;
      data->bytes_total = 3;
      data->bytes_remaining = 2;
    }

    وإلا 𐤀𐤌(c >= 0xf0 && c < 0xf8) {
      𐤀𐤌(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x07;
      data->bytes_total = 4;
      data->bytes_remaining = 3;
    }

    وإلا 𐤀𐤌(c >= 0xf8 && c < 0xfc) {
      𐤀𐤌(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x03;
      data->bytes_total = 5;
      data->bytes_remaining = 4;
    }

    وإلا 𐤀𐤌(c >= 0xfc && c < 0xfe) {
      𐤀𐤌(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x01;
      data->bytes_total = 6;
      data->bytes_remaining = 5;
    }

    وإلا {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

ساكن VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

ساكن فراغ decode_usascii(VTermEncoding *enc UNUSED, فراغ *data UNUSED,
                           uint32_t cp[], صحيح *cpi, صحيح cplen,
                           ثابت حرف bytes[], size_t *pos, size_t bytelen)
{
  صحيح is_gr = bytes[*pos] & 0x80;

  لكل(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned حرف c = bytes[*pos] ^ is_gr;

    𐤀𐤌(c < 0x20 || c == 0x7f || c >= 0x80)
      أرجع;

    cp[(*cpi)++] = c;
  }
}

ساكن VTermEncoding encoding_usascii = {
  عدم,           // init
  &decode_usascii // decode
};

بنية StaticTableEncoding {
  ثابت VTermEncoding enc;
  ثابت uint32_t chars[128];
};

ساكن فراغ decode_table(VTermEncoding *enc, فراغ *data UNUSED,
                         uint32_t cp[], صحيح *cpi, صحيح cplen,
                         ثابت حرف bytes[], size_t *pos, size_t bytelen)
{
  بنية StaticTableEncoding *table = (بنية StaticTableEncoding *)enc;
  صحيح is_gr = bytes[*pos] & 0x80;

  لكل(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned حرف c = bytes[*pos] ^ is_gr;

    𐤀𐤌(c < 0x20 || c == 0x7f || c >= 0x80)
      أرجع;

    𐤀𐤌(table->chars[c])
      cp[(*cpi)++] = table->chars[c];
    وإلا
      cp[(*cpi)++] = c;
  }
}

#ضمن "encoding/DECdrawing.inc"
#ضمن "encoding/uk.inc"

ساكن بنية {
  VTermEncodingType type;
  حرف designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, عدم },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, حرف designation)
{
  لكل(صحيح i = 0; encodings[i].designation; i++)
    𐤀𐤌(encodings[i].type == type && encodings[i].designation == designation)
      أرجع encodings[i].enc;
  أرجع عدم;
}
