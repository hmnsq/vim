#ଅନ୍ତର୍ଭାଵ "vterm_internal.h"

#define UNICODE_INVALID 0xFFFD

#ଯଦି defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#endif

ସଂରଚନା UTF8DecoderData {
  // number of bytes remaining in this codepoint
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytes_remaining;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytes_total;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା this_cp;
};

ସ୍ଥିର ଶୂନ୍ୟ init_utf8(VTermEncoding *enc UNUSED, ଶୂନ୍ୟ *data_)
{
  ସଂରଚନା UTF8DecoderData *data = data_;

  data->bytes_remaining = 0;
  data->bytes_total     = 0;
}

ସ୍ଥିର ଶୂନ୍ୟ decode_utf8(VTermEncoding *enc UNUSED, ଶୂନ୍ୟ *data_,
                        uint32_t cp[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cpi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cplen,
                        ଅଚଳ ଅକ୍ଷର bytes[], size_t *pos, size_t bytelen)
{
  ସଂରଚନା UTF8DecoderData *data = data_;

#ifdef DEBUG_PRINT_UTF8
  printf("BEGIN UTF-8\n");
#endif

  ପାଇଁ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned ଅକ୍ଷର c = bytes[*pos];

#ifdef DEBUG_PRINT_UTF8
    printf(" pos=%zd c=%02x rem=%d\n", *pos, c, data->bytes_remaining);
#endif

    ଯଦି(c < 0x20) // C0
      ଫେରନ୍ତୁ;

    ଅନ୍ୟଥା ଯଦି(c >= 0x20 && c < 0x7f) {
      ଯଦି(data->bytes_remaining) {
        data->bytes_remaining = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	ଯଦି (*cpi >= cplen)
	  ଭାଙ୍ଗ;
      }
      cp[(*cpi)++] = c;
#ifdef DEBUG_PRINT_UTF8
      printf(" UTF-8 char: U+%04x\n", c);
#endif
    }

    ଅନ୍ୟଥା ଯଦି(c == 0x7f) // DEL
      ଫେରନ୍ତୁ;

    ଅନ୍ୟଥା ଯଦି(c >= 0x80 && c < 0xc0) {
      ଯଦି(!data->bytes_remaining) {
        cp[(*cpi)++] = UNICODE_INVALID;
        ଜାରି;
      }

      data->this_cp <<= 6;
      data->this_cp |= c & 0x3f;
      data->bytes_remaining--;

      ଯଦି(!data->bytes_remaining) {
#ifdef DEBUG_PRINT_UTF8
        printf(" UTF-8 raw char U+%04x bytelen=%d ", data->this_cp, data->bytes_total);
#endif
        // Check for overlong sequences
        ବାଛ(data->bytes_total) {
        ସ୍ଥିତି 2:
          ଯଦି(data->this_cp <  0x0080) data->this_cp = UNICODE_INVALID;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 3:
          ଯଦି(data->this_cp <  0x0800) data->this_cp = UNICODE_INVALID;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 4:
          ଯଦି(data->this_cp < 0x10000) data->this_cp = UNICODE_INVALID;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 5:
          ଯଦି(data->this_cp < 0x200000) data->this_cp = UNICODE_INVALID;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 6:
          ଯଦି(data->this_cp < 0x4000000) data->this_cp = UNICODE_INVALID;
          ଭାଙ୍ଗ;
        }
        // Now look for plain invalid ones
        ଯଦି((data->this_cp >= 0xD800 && data->this_cp <= 0xDFFF) ||
           data->this_cp == 0xFFFE ||
           data->this_cp == 0xFFFF)
          data->this_cp = UNICODE_INVALID;
#ifdef DEBUG_PRINT_UTF8
        printf(" char: U+%04x\n", data->this_cp);
#endif
        cp[(*cpi)++] = data->this_cp;
      }
    }

    ଅନ୍ୟଥା ଯଦି(c >= 0xc0 && c < 0xe0) {
      ଯଦି(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x1f;
      data->bytes_total = 2;
      data->bytes_remaining = 1;
    }

    ଅନ୍ୟଥା ଯଦି(c >= 0xe0 && c < 0xf0) {
      ଯଦି(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x0f;
      data->bytes_total = 3;
      data->bytes_remaining = 2;
    }

    ଅନ୍ୟଥା ଯଦି(c >= 0xf0 && c < 0xf8) {
      ଯଦି(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x07;
      data->bytes_total = 4;
      data->bytes_remaining = 3;
    }

    ଅନ୍ୟଥା ଯଦି(c >= 0xf8 && c < 0xfc) {
      ଯଦି(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x03;
      data->bytes_total = 5;
      data->bytes_remaining = 4;
    }

    ଅନ୍ୟଥା ଯଦି(c >= 0xfc && c < 0xfe) {
      ଯଦି(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x01;
      data->bytes_total = 6;
      data->bytes_remaining = 5;
    }

    ଅନ୍ୟଥା {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

ସ୍ଥିର VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

ସ୍ଥିର ଶୂନ୍ୟ decode_usascii(VTermEncoding *enc UNUSED, ଶୂନ୍ୟ *data UNUSED,
                           uint32_t cp[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cpi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cplen,
                           ଅଚଳ ଅକ୍ଷର bytes[], size_t *pos, size_t bytelen)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_gr = bytes[*pos] & 0x80;

  ପାଇଁ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned ଅକ୍ଷର c = bytes[*pos] ^ is_gr;

    ଯଦି(c < 0x20 || c == 0x7f || c >= 0x80)
      ଫେରନ୍ତୁ;

    cp[(*cpi)++] = c;
  }
}

ସ୍ଥିର VTermEncoding encoding_usascii = {
  ଖାଲି,           // init
  &decode_usascii // decode
};

ସଂରଚନା StaticTableEncoding {
  ଅଚଳ VTermEncoding enc;
  ଅଚଳ uint32_t chars[128];
};

ସ୍ଥିର ଶୂନ୍ୟ decode_table(VTermEncoding *enc, ଶୂନ୍ୟ *data UNUSED,
                         uint32_t cp[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cpi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cplen,
                         ଅଚଳ ଅକ୍ଷର bytes[], size_t *pos, size_t bytelen)
{
  ସଂରଚନା StaticTableEncoding *table = (ସଂରଚନା StaticTableEncoding *)enc;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_gr = bytes[*pos] & 0x80;

  ପାଇଁ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned ଅକ୍ଷର c = bytes[*pos] ^ is_gr;

    ଯଦି(c < 0x20 || c == 0x7f || c >= 0x80)
      ଫେରନ୍ତୁ;

    ଯଦି(table->chars[c])
      cp[(*cpi)++] = table->chars[c];
    ଅନ୍ୟଥା
      cp[(*cpi)++] = c;
  }
}

#ଅନ୍ତର୍ଭାଵ "encoding/DECdrawing.inc"
#ଅନ୍ତର୍ଭାଵ "encoding/uk.inc"

ସ୍ଥିର ସଂରଚନା {
  VTermEncodingType type;
  ଅକ୍ଷର designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, ଖାଲି },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, ଅକ୍ଷର designation)
{
  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; encodings[i].designation; i++)
    ଯଦି(encodings[i].type == type && encodings[i].designation == designation)
      ଫେରନ୍ତୁ encodings[i].enc;
  ଫେରନ୍ତୁ ଖାଲି;
}
