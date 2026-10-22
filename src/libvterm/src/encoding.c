#അന്തര്ഭാവ "vterm_internal.h"

#define UNICODE_INVALID 0xFFFD

#എങ്കിൽ defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#endif

ഘടന UTF8DecoderData {
  // number of bytes remaining in this codepoint
  പൂർണ്ണസംഖ്യ bytes_remaining;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  പൂർണ്ണസംഖ്യ bytes_total;

  പൂർണ്ണസംഖ്യ this_cp;
};

സ്ഥിതം ശൂന്യം init_utf8(VTermEncoding *enc UNUSED, ശൂന്യം *data_)
{
  ഘടന UTF8DecoderData *data = data_;

  data->bytes_remaining = 0;
  data->bytes_total     = 0;
}

സ്ഥിതം ശൂന്യം decode_utf8(VTermEncoding *enc UNUSED, ശൂന്യം *data_,
                        uint32_t cp[], പൂർണ്ണസംഖ്യ *cpi, പൂർണ്ണസംഖ്യ cplen,
                        സ്ഥിരം അക്ഷരം bytes[], size_t *pos, size_t bytelen)
{
  ഘടന UTF8DecoderData *data = data_;

#ifdef DEBUG_PRINT_UTF8
  printf("BEGIN UTF-8\n");
#endif

  വേണ്ടി(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned അക്ഷരം c = bytes[*pos];

#ifdef DEBUG_PRINT_UTF8
    printf(" pos=%zd c=%02x rem=%d\n", *pos, c, data->bytes_remaining);
#endif

    എങ്കിൽ(c < 0x20) // C0
      തിരികെ;

    അല്ലെങ്കിൽ എങ്കിൽ(c >= 0x20 && c < 0x7f) {
      എങ്കിൽ(data->bytes_remaining) {
        data->bytes_remaining = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	എങ്കിൽ (*cpi >= cplen)
	  നിർത്തുക;
      }
      cp[(*cpi)++] = c;
#ifdef DEBUG_PRINT_UTF8
      printf(" UTF-8 char: U+%04x\n", c);
#endif
    }

    അല്ലെങ്കിൽ എങ്കിൽ(c == 0x7f) // DEL
      തിരികെ;

    അല്ലെങ്കിൽ എങ്കിൽ(c >= 0x80 && c < 0xc0) {
      എങ്കിൽ(!data->bytes_remaining) {
        cp[(*cpi)++] = UNICODE_INVALID;
        തുടരുക;
      }

      data->this_cp <<= 6;
      data->this_cp |= c & 0x3f;
      data->bytes_remaining--;

      എങ്കിൽ(!data->bytes_remaining) {
#ifdef DEBUG_PRINT_UTF8
        printf(" UTF-8 raw char U+%04x bytelen=%d ", data->this_cp, data->bytes_total);
#endif
        // Check for overlong sequences
        തിരഞ്ഞെടുപ്പ്(data->bytes_total) {
        അവസ്ഥ 2:
          എങ്കിൽ(data->this_cp <  0x0080) data->this_cp = UNICODE_INVALID;
          നിർത്തുക;
        അവസ്ഥ 3:
          എങ്കിൽ(data->this_cp <  0x0800) data->this_cp = UNICODE_INVALID;
          നിർത്തുക;
        അവസ്ഥ 4:
          എങ്കിൽ(data->this_cp < 0x10000) data->this_cp = UNICODE_INVALID;
          നിർത്തുക;
        അവസ്ഥ 5:
          എങ്കിൽ(data->this_cp < 0x200000) data->this_cp = UNICODE_INVALID;
          നിർത്തുക;
        അവസ്ഥ 6:
          എങ്കിൽ(data->this_cp < 0x4000000) data->this_cp = UNICODE_INVALID;
          നിർത്തുക;
        }
        // Now look for plain invalid ones
        എങ്കിൽ((data->this_cp >= 0xD800 && data->this_cp <= 0xDFFF) ||
           data->this_cp == 0xFFFE ||
           data->this_cp == 0xFFFF)
          data->this_cp = UNICODE_INVALID;
#ifdef DEBUG_PRINT_UTF8
        printf(" char: U+%04x\n", data->this_cp);
#endif
        cp[(*cpi)++] = data->this_cp;
      }
    }

    അല്ലെങ്കിൽ എങ്കിൽ(c >= 0xc0 && c < 0xe0) {
      എങ്കിൽ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x1f;
      data->bytes_total = 2;
      data->bytes_remaining = 1;
    }

    അല്ലെങ്കിൽ എങ്കിൽ(c >= 0xe0 && c < 0xf0) {
      എങ്കിൽ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x0f;
      data->bytes_total = 3;
      data->bytes_remaining = 2;
    }

    അല്ലെങ്കിൽ എങ്കിൽ(c >= 0xf0 && c < 0xf8) {
      എങ്കിൽ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x07;
      data->bytes_total = 4;
      data->bytes_remaining = 3;
    }

    അല്ലെങ്കിൽ എങ്കിൽ(c >= 0xf8 && c < 0xfc) {
      എങ്കിൽ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x03;
      data->bytes_total = 5;
      data->bytes_remaining = 4;
    }

    അല്ലെങ്കിൽ എങ്കിൽ(c >= 0xfc && c < 0xfe) {
      എങ്കിൽ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x01;
      data->bytes_total = 6;
      data->bytes_remaining = 5;
    }

    അല്ലെങ്കിൽ {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

സ്ഥിതം VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

സ്ഥിതം ശൂന്യം decode_usascii(VTermEncoding *enc UNUSED, ശൂന്യം *data UNUSED,
                           uint32_t cp[], പൂർണ്ണസംഖ്യ *cpi, പൂർണ്ണസംഖ്യ cplen,
                           സ്ഥിരം അക്ഷരം bytes[], size_t *pos, size_t bytelen)
{
  പൂർണ്ണസംഖ്യ is_gr = bytes[*pos] & 0x80;

  വേണ്ടി(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned അക്ഷരം c = bytes[*pos] ^ is_gr;

    എങ്കിൽ(c < 0x20 || c == 0x7f || c >= 0x80)
      തിരികെ;

    cp[(*cpi)++] = c;
  }
}

സ്ഥിതം VTermEncoding encoding_usascii = {
  ശൂന്യ,           // init
  &decode_usascii // decode
};

ഘടന StaticTableEncoding {
  സ്ഥിരം VTermEncoding enc;
  സ്ഥിരം uint32_t chars[128];
};

സ്ഥിതം ശൂന്യം decode_table(VTermEncoding *enc, ശൂന്യം *data UNUSED,
                         uint32_t cp[], പൂർണ്ണസംഖ്യ *cpi, പൂർണ്ണസംഖ്യ cplen,
                         സ്ഥിരം അക്ഷരം bytes[], size_t *pos, size_t bytelen)
{
  ഘടന StaticTableEncoding *table = (ഘടന StaticTableEncoding *)enc;
  പൂർണ്ണസംഖ്യ is_gr = bytes[*pos] & 0x80;

  വേണ്ടി(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned അക്ഷരം c = bytes[*pos] ^ is_gr;

    എങ്കിൽ(c < 0x20 || c == 0x7f || c >= 0x80)
      തിരികെ;

    എങ്കിൽ(table->chars[c])
      cp[(*cpi)++] = table->chars[c];
    അല്ലെങ്കിൽ
      cp[(*cpi)++] = c;
  }
}

#അന്തര്ഭാവ "encoding/DECdrawing.inc"
#അന്തര്ഭാവ "encoding/uk.inc"

സ്ഥിതം ഘടന {
  VTermEncodingType type;
  അക്ഷരം designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, ശൂന്യ },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, അക്ഷരം designation)
{
  വേണ്ടി(പൂർണ്ണസംഖ്യ i = 0; encodings[i].designation; i++)
    എങ്കിൽ(encodings[i].type == type && encodings[i].designation == designation)
      തിരികെ encodings[i].enc;
  തിരികെ ശൂന്യ;
}
