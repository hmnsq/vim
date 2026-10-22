#அந்தர்பாவ "vterm_internal.h"

#define UNICODE_INVALID 0xFFFD

#எனில் defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#endif

கட்டமைப்பு UTF8DecoderData {
  // number of bytes remaining in this codepoint
  முழு bytes_remaining;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  முழு bytes_total;

  முழு this_cp;
};

நிலையான வெறுமை init_utf8(VTermEncoding *enc UNUSED, வெறுமை *data_)
{
  கட்டமைப்பு UTF8DecoderData *data = data_;

  data->bytes_remaining = 0;
  data->bytes_total     = 0;
}

நிலையான வெறுமை decode_utf8(VTermEncoding *enc UNUSED, வெறுமை *data_,
                        uint32_t cp[], முழு *cpi, முழு cplen,
                        மாறிலி எழுத்து bytes[], size_t *pos, size_t bytelen)
{
  கட்டமைப்பு UTF8DecoderData *data = data_;

#ifdef DEBUG_PRINT_UTF8
  printf("BEGIN UTF-8\n");
#endif

  க்கு(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned எழுத்து c = bytes[*pos];

#ifdef DEBUG_PRINT_UTF8
    printf(" pos=%zd c=%02x rem=%d\n", *pos, c, data->bytes_remaining);
#endif

    எனில்(c < 0x20) // C0
      திரும்பு;

    இல்லையெனில் எனில்(c >= 0x20 && c < 0x7f) {
      எனில்(data->bytes_remaining) {
        data->bytes_remaining = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	எனில் (*cpi >= cplen)
	  நிறுத்து;
      }
      cp[(*cpi)++] = c;
#ifdef DEBUG_PRINT_UTF8
      printf(" UTF-8 char: U+%04x\n", c);
#endif
    }

    இல்லையெனில் எனில்(c == 0x7f) // DEL
      திரும்பு;

    இல்லையெனில் எனில்(c >= 0x80 && c < 0xc0) {
      எனில்(!data->bytes_remaining) {
        cp[(*cpi)++] = UNICODE_INVALID;
        தொடர்;
      }

      data->this_cp <<= 6;
      data->this_cp |= c & 0x3f;
      data->bytes_remaining--;

      எனில்(!data->bytes_remaining) {
#ifdef DEBUG_PRINT_UTF8
        printf(" UTF-8 raw char U+%04x bytelen=%d ", data->this_cp, data->bytes_total);
#endif
        // Check for overlong sequences
        தேர்வு(data->bytes_total) {
        நிலை 2:
          எனில்(data->this_cp <  0x0080) data->this_cp = UNICODE_INVALID;
          நிறுத்து;
        நிலை 3:
          எனில்(data->this_cp <  0x0800) data->this_cp = UNICODE_INVALID;
          நிறுத்து;
        நிலை 4:
          எனில்(data->this_cp < 0x10000) data->this_cp = UNICODE_INVALID;
          நிறுத்து;
        நிலை 5:
          எனில்(data->this_cp < 0x200000) data->this_cp = UNICODE_INVALID;
          நிறுத்து;
        நிலை 6:
          எனில்(data->this_cp < 0x4000000) data->this_cp = UNICODE_INVALID;
          நிறுத்து;
        }
        // Now look for plain invalid ones
        எனில்((data->this_cp >= 0xD800 && data->this_cp <= 0xDFFF) ||
           data->this_cp == 0xFFFE ||
           data->this_cp == 0xFFFF)
          data->this_cp = UNICODE_INVALID;
#ifdef DEBUG_PRINT_UTF8
        printf(" char: U+%04x\n", data->this_cp);
#endif
        cp[(*cpi)++] = data->this_cp;
      }
    }

    இல்லையெனில் எனில்(c >= 0xc0 && c < 0xe0) {
      எனில்(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x1f;
      data->bytes_total = 2;
      data->bytes_remaining = 1;
    }

    இல்லையெனில் எனில்(c >= 0xe0 && c < 0xf0) {
      எனில்(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x0f;
      data->bytes_total = 3;
      data->bytes_remaining = 2;
    }

    இல்லையெனில் எனில்(c >= 0xf0 && c < 0xf8) {
      எனில்(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x07;
      data->bytes_total = 4;
      data->bytes_remaining = 3;
    }

    இல்லையெனில் எனில்(c >= 0xf8 && c < 0xfc) {
      எனில்(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x03;
      data->bytes_total = 5;
      data->bytes_remaining = 4;
    }

    இல்லையெனில் எனில்(c >= 0xfc && c < 0xfe) {
      எனில்(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x01;
      data->bytes_total = 6;
      data->bytes_remaining = 5;
    }

    இல்லையெனில் {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

நிலையான VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

நிலையான வெறுமை decode_usascii(VTermEncoding *enc UNUSED, வெறுமை *data UNUSED,
                           uint32_t cp[], முழு *cpi, முழு cplen,
                           மாறிலி எழுத்து bytes[], size_t *pos, size_t bytelen)
{
  முழு is_gr = bytes[*pos] & 0x80;

  க்கு(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned எழுத்து c = bytes[*pos] ^ is_gr;

    எனில்(c < 0x20 || c == 0x7f || c >= 0x80)
      திரும்பு;

    cp[(*cpi)++] = c;
  }
}

நிலையான VTermEncoding encoding_usascii = {
  வெற்று,           // init
  &decode_usascii // decode
};

கட்டமைப்பு StaticTableEncoding {
  மாறிலி VTermEncoding enc;
  மாறிலி uint32_t chars[128];
};

நிலையான வெறுமை decode_table(VTermEncoding *enc, வெறுமை *data UNUSED,
                         uint32_t cp[], முழு *cpi, முழு cplen,
                         மாறிலி எழுத்து bytes[], size_t *pos, size_t bytelen)
{
  கட்டமைப்பு StaticTableEncoding *table = (கட்டமைப்பு StaticTableEncoding *)enc;
  முழு is_gr = bytes[*pos] & 0x80;

  க்கு(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned எழுத்து c = bytes[*pos] ^ is_gr;

    எனில்(c < 0x20 || c == 0x7f || c >= 0x80)
      திரும்பு;

    எனில்(table->chars[c])
      cp[(*cpi)++] = table->chars[c];
    இல்லையெனில்
      cp[(*cpi)++] = c;
  }
}

#அந்தர்பாவ "encoding/DECdrawing.inc"
#அந்தர்பாவ "encoding/uk.inc"

நிலையான கட்டமைப்பு {
  VTermEncodingType type;
  எழுத்து designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, வெற்று },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, எழுத்து designation)
{
  க்கு(முழு i = 0; encodings[i].designation; i++)
    எனில்(encodings[i].type == type && encodings[i].designation == designation)
      திரும்பு encodings[i].enc;
  திரும்பு வெற்று;
}
