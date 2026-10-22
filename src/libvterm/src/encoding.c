#অন্তর্ভাব "vterm_internal.h"

#define UNICODE_INVALID 0xFFFD

#যদি defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#endif

সংরচনা UTF8DecoderData {
  // number of bytes remaining in this codepoint
  পূর্ণাঙ্ক bytes_remaining;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  পূর্ণাঙ্ক bytes_total;

  পূর্ণাঙ্ক this_cp;
};

স্থির শূন্য init_utf8(VTermEncoding *enc UNUSED, শূন্য *data_)
{
  সংরচনা UTF8DecoderData *data = data_;

  data->bytes_remaining = 0;
  data->bytes_total     = 0;
}

স্থির শূন্য decode_utf8(VTermEncoding *enc UNUSED, শূন্য *data_,
                        uint32_t cp[], পূর্ণাঙ্ক *cpi, পূর্ণাঙ্ক cplen,
                        ধ্রুব বর্ণ bytes[], size_t *pos, size_t bytelen)
{
  সংরচনা UTF8DecoderData *data = data_;

#ifdef DEBUG_PRINT_UTF8
  printf("BEGIN UTF-8\n");
#endif

  প্রতি(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned বর্ণ c = bytes[*pos];

#ifdef DEBUG_PRINT_UTF8
    printf(" pos=%zd c=%02x rem=%d\n", *pos, c, data->bytes_remaining);
#endif

    যদি(c < 0x20) // C0
      প্রত্যাগম;

    অন্যথা যদি(c >= 0x20 && c < 0x7f) {
      যদি(data->bytes_remaining) {
        data->bytes_remaining = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	যদি (*cpi >= cplen)
	  ভঙ্গ;
      }
      cp[(*cpi)++] = c;
#ifdef DEBUG_PRINT_UTF8
      printf(" UTF-8 char: U+%04x\n", c);
#endif
    }

    অন্যথা যদি(c == 0x7f) // DEL
      প্রত্যাগম;

    অন্যথা যদি(c >= 0x80 && c < 0xc0) {
      যদি(!data->bytes_remaining) {
        cp[(*cpi)++] = UNICODE_INVALID;
        অনুবর্ত;
      }

      data->this_cp <<= 6;
      data->this_cp |= c & 0x3f;
      data->bytes_remaining--;

      যদি(!data->bytes_remaining) {
#ifdef DEBUG_PRINT_UTF8
        printf(" UTF-8 raw char U+%04x bytelen=%d ", data->this_cp, data->bytes_total);
#endif
        // Check for overlong sequences
        বিকল্প(data->bytes_total) {
        পক্ষ 2:
          যদি(data->this_cp <  0x0080) data->this_cp = UNICODE_INVALID;
          ভঙ্গ;
        পক্ষ 3:
          যদি(data->this_cp <  0x0800) data->this_cp = UNICODE_INVALID;
          ভঙ্গ;
        পক্ষ 4:
          যদি(data->this_cp < 0x10000) data->this_cp = UNICODE_INVALID;
          ভঙ্গ;
        পক্ষ 5:
          যদি(data->this_cp < 0x200000) data->this_cp = UNICODE_INVALID;
          ভঙ্গ;
        পক্ষ 6:
          যদি(data->this_cp < 0x4000000) data->this_cp = UNICODE_INVALID;
          ভঙ্গ;
        }
        // Now look for plain invalid ones
        যদি((data->this_cp >= 0xD800 && data->this_cp <= 0xDFFF) ||
           data->this_cp == 0xFFFE ||
           data->this_cp == 0xFFFF)
          data->this_cp = UNICODE_INVALID;
#ifdef DEBUG_PRINT_UTF8
        printf(" char: U+%04x\n", data->this_cp);
#endif
        cp[(*cpi)++] = data->this_cp;
      }
    }

    অন্যথা যদি(c >= 0xc0 && c < 0xe0) {
      যদি(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x1f;
      data->bytes_total = 2;
      data->bytes_remaining = 1;
    }

    অন্যথা যদি(c >= 0xe0 && c < 0xf0) {
      যদি(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x0f;
      data->bytes_total = 3;
      data->bytes_remaining = 2;
    }

    অন্যথা যদি(c >= 0xf0 && c < 0xf8) {
      যদি(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x07;
      data->bytes_total = 4;
      data->bytes_remaining = 3;
    }

    অন্যথা যদি(c >= 0xf8 && c < 0xfc) {
      যদি(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x03;
      data->bytes_total = 5;
      data->bytes_remaining = 4;
    }

    অন্যথা যদি(c >= 0xfc && c < 0xfe) {
      যদি(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x01;
      data->bytes_total = 6;
      data->bytes_remaining = 5;
    }

    অন্যথা {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

স্থির VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

স্থির শূন্য decode_usascii(VTermEncoding *enc UNUSED, শূন্য *data UNUSED,
                           uint32_t cp[], পূর্ণাঙ্ক *cpi, পূর্ণাঙ্ক cplen,
                           ধ্রুব বর্ণ bytes[], size_t *pos, size_t bytelen)
{
  পূর্ণাঙ্ক is_gr = bytes[*pos] & 0x80;

  প্রতি(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned বর্ণ c = bytes[*pos] ^ is_gr;

    যদি(c < 0x20 || c == 0x7f || c >= 0x80)
      প্রত্যাগম;

    cp[(*cpi)++] = c;
  }
}

স্থির VTermEncoding encoding_usascii = {
  রিক্ত,           // init
  &decode_usascii // decode
};

সংরচনা StaticTableEncoding {
  ধ্রুব VTermEncoding enc;
  ধ্রুব uint32_t chars[128];
};

স্থির শূন্য decode_table(VTermEncoding *enc, শূন্য *data UNUSED,
                         uint32_t cp[], পূর্ণাঙ্ক *cpi, পূর্ণাঙ্ক cplen,
                         ধ্রুব বর্ণ bytes[], size_t *pos, size_t bytelen)
{
  সংরচনা StaticTableEncoding *table = (সংরচনা StaticTableEncoding *)enc;
  পূর্ণাঙ্ক is_gr = bytes[*pos] & 0x80;

  প্রতি(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned বর্ণ c = bytes[*pos] ^ is_gr;

    যদি(c < 0x20 || c == 0x7f || c >= 0x80)
      প্রত্যাগম;

    যদি(table->chars[c])
      cp[(*cpi)++] = table->chars[c];
    অন্যথা
      cp[(*cpi)++] = c;
  }
}

#অন্তর্ভাব "encoding/DECdrawing.inc"
#অন্তর্ভাব "encoding/uk.inc"

স্থির সংরচনা {
  VTermEncodingType type;
  বর্ণ designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, রিক্ত },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, বর্ণ designation)
{
  প্রতি(পূর্ণাঙ্ক i = 0; encodings[i].designation; i++)
    যদি(encodings[i].type == type && encodings[i].designation == designation)
      প্রত্যাগম encodings[i].enc;
  প্রত্যাগম রিক্ত;
}
