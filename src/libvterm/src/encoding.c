#شامل "vterm_internal.h"

#define UNICODE_INVALID 0xFFFD

#اگر defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#endif

ساختار UTF8DecoderData {
  // number of bytes remaining in this codepoint
  صحیح bytes_remaining;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  صحیح bytes_total;

  صحیح this_cp;
};

ایستا تهی init_utf8(VTermEncoding *enc UNUSED, تهی *data_)
{
  ساختار UTF8DecoderData *data = data_;

  data->bytes_remaining = 0;
  data->bytes_total     = 0;
}

ایستا تهی decode_utf8(VTermEncoding *enc UNUSED, تهی *data_,
                        uint32_t cp[], صحیح *cpi, صحیح cplen,
                        ثابت نویسه bytes[], size_t *pos, size_t bytelen)
{
  ساختار UTF8DecoderData *data = data_;

#ifdef DEBUG_PRINT_UTF8
  printf("BEGIN UTF-8\n");
#endif

  برای(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned نویسه c = bytes[*pos];

#ifdef DEBUG_PRINT_UTF8
    printf(" pos=%zd c=%02x rem=%d\n", *pos, c, data->bytes_remaining);
#endif

    اگر(c < 0x20) // C0
      بازگشت;

    وگرنه اگر(c >= 0x20 && c < 0x7f) {
      اگر(data->bytes_remaining) {
        data->bytes_remaining = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	اگر (*cpi >= cplen)
	  بشکن;
      }
      cp[(*cpi)++] = c;
#ifdef DEBUG_PRINT_UTF8
      printf(" UTF-8 char: U+%04x\n", c);
#endif
    }

    وگرنه اگر(c == 0x7f) // DEL
      بازگشت;

    وگرنه اگر(c >= 0x80 && c < 0xc0) {
      اگر(!data->bytes_remaining) {
        cp[(*cpi)++] = UNICODE_INVALID;
        ادامه;
      }

      data->this_cp <<= 6;
      data->this_cp |= c & 0x3f;
      data->bytes_remaining--;

      اگر(!data->bytes_remaining) {
#ifdef DEBUG_PRINT_UTF8
        printf(" UTF-8 raw char U+%04x bytelen=%d ", data->this_cp, data->bytes_total);
#endif
        // Check for overlong sequences
        انتخاب(data->bytes_total) {
        حالت 2:
          اگر(data->this_cp <  0x0080) data->this_cp = UNICODE_INVALID;
          بشکن;
        حالت 3:
          اگر(data->this_cp <  0x0800) data->this_cp = UNICODE_INVALID;
          بشکن;
        حالت 4:
          اگر(data->this_cp < 0x10000) data->this_cp = UNICODE_INVALID;
          بشکن;
        حالت 5:
          اگر(data->this_cp < 0x200000) data->this_cp = UNICODE_INVALID;
          بشکن;
        حالت 6:
          اگر(data->this_cp < 0x4000000) data->this_cp = UNICODE_INVALID;
          بشکن;
        }
        // Now look for plain invalid ones
        اگر((data->this_cp >= 0xD800 && data->this_cp <= 0xDFFF) ||
           data->this_cp == 0xFFFE ||
           data->this_cp == 0xFFFF)
          data->this_cp = UNICODE_INVALID;
#ifdef DEBUG_PRINT_UTF8
        printf(" char: U+%04x\n", data->this_cp);
#endif
        cp[(*cpi)++] = data->this_cp;
      }
    }

    وگرنه اگر(c >= 0xc0 && c < 0xe0) {
      اگر(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x1f;
      data->bytes_total = 2;
      data->bytes_remaining = 1;
    }

    وگرنه اگر(c >= 0xe0 && c < 0xf0) {
      اگر(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x0f;
      data->bytes_total = 3;
      data->bytes_remaining = 2;
    }

    وگرنه اگر(c >= 0xf0 && c < 0xf8) {
      اگر(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x07;
      data->bytes_total = 4;
      data->bytes_remaining = 3;
    }

    وگرنه اگر(c >= 0xf8 && c < 0xfc) {
      اگر(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x03;
      data->bytes_total = 5;
      data->bytes_remaining = 4;
    }

    وگرنه اگر(c >= 0xfc && c < 0xfe) {
      اگر(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x01;
      data->bytes_total = 6;
      data->bytes_remaining = 5;
    }

    وگرنه {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

ایستا VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

ایستا تهی decode_usascii(VTermEncoding *enc UNUSED, تهی *data UNUSED,
                           uint32_t cp[], صحیح *cpi, صحیح cplen,
                           ثابت نویسه bytes[], size_t *pos, size_t bytelen)
{
  صحیح is_gr = bytes[*pos] & 0x80;

  برای(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned نویسه c = bytes[*pos] ^ is_gr;

    اگر(c < 0x20 || c == 0x7f || c >= 0x80)
      بازگشت;

    cp[(*cpi)++] = c;
  }
}

ایستا VTermEncoding encoding_usascii = {
  پوچ,           // init
  &decode_usascii // decode
};

ساختار StaticTableEncoding {
  ثابت VTermEncoding enc;
  ثابت uint32_t chars[128];
};

ایستا تهی decode_table(VTermEncoding *enc, تهی *data UNUSED,
                         uint32_t cp[], صحیح *cpi, صحیح cplen,
                         ثابت نویسه bytes[], size_t *pos, size_t bytelen)
{
  ساختار StaticTableEncoding *table = (ساختار StaticTableEncoding *)enc;
  صحیح is_gr = bytes[*pos] & 0x80;

  برای(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned نویسه c = bytes[*pos] ^ is_gr;

    اگر(c < 0x20 || c == 0x7f || c >= 0x80)
      بازگشت;

    اگر(table->chars[c])
      cp[(*cpi)++] = table->chars[c];
    وگرنه
      cp[(*cpi)++] = c;
  }
}

#شامل "encoding/DECdrawing.inc"
#شامل "encoding/uk.inc"

ایستا ساختار {
  VTermEncodingType type;
  نویسه designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, پوچ },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, نویسه designation)
{
  برای(صحیح i = 0; encodings[i].designation; i++)
    اگر(encodings[i].type == type && encodings[i].designation == designation)
      بازگشت encodings[i].enc;
  بازگشت پوچ;
}
