#ಅನ್ತರ್ಭಾವ "vterm_internal.h"

#define UNICODE_INVALID 0xFFFD

#ಯದಿ defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#endif

ಸಂರಚನಾ UTF8DecoderData {
  // number of bytes remaining in this codepoint
  ಪೂರ್ಣಾಙ್ಕ bytes_remaining;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  ಪೂರ್ಣಾಙ್ಕ bytes_total;

  ಪೂರ್ಣಾಙ್ಕ this_cp;
};

ಸ್ಥಿರ ಶೂನ್ಯ init_utf8(VTermEncoding *enc UNUSED, ಶೂನ್ಯ *data_)
{
  ಸಂರಚನಾ UTF8DecoderData *data = data_;

  data->bytes_remaining = 0;
  data->bytes_total     = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ decode_utf8(VTermEncoding *enc UNUSED, ಶೂನ್ಯ *data_,
                        uint32_t cp[], ಪೂರ್ಣಾಙ್ಕ *cpi, ಪೂರ್ಣಾಙ್ಕ cplen,
                        ಧ್ರುವ ವರ್ಣ bytes[], size_t *pos, size_t bytelen)
{
  ಸಂರಚನಾ UTF8DecoderData *data = data_;

#ifdef DEBUG_PRINT_UTF8
  printf("BEGIN UTF-8\n");
#endif

  ಪ್ರತಿ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned ವರ್ಣ c = bytes[*pos];

#ifdef DEBUG_PRINT_UTF8
    printf(" pos=%zd c=%02x rem=%d\n", *pos, c, data->bytes_remaining);
#endif

    ಯದಿ(c < 0x20) // C0
      ಪ್ರತ್ಯಾಗಮ;

    ಅನ್ಯಥಾ ಯದಿ(c >= 0x20 && c < 0x7f) {
      ಯದಿ(data->bytes_remaining) {
        data->bytes_remaining = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	ಯದಿ (*cpi >= cplen)
	  ಭಙ್ಗ;
      }
      cp[(*cpi)++] = c;
#ifdef DEBUG_PRINT_UTF8
      printf(" UTF-8 char: U+%04x\n", c);
#endif
    }

    ಅನ್ಯಥಾ ಯದಿ(c == 0x7f) // DEL
      ಪ್ರತ್ಯಾಗಮ;

    ಅನ್ಯಥಾ ಯದಿ(c >= 0x80 && c < 0xc0) {
      ಯದಿ(!data->bytes_remaining) {
        cp[(*cpi)++] = UNICODE_INVALID;
        ಅನುವರ್ತ;
      }

      data->this_cp <<= 6;
      data->this_cp |= c & 0x3f;
      data->bytes_remaining--;

      ಯದಿ(!data->bytes_remaining) {
#ifdef DEBUG_PRINT_UTF8
        printf(" UTF-8 raw char U+%04x bytelen=%d ", data->this_cp, data->bytes_total);
#endif
        // Check for overlong sequences
        ವಿಕಲ್ಪ(data->bytes_total) {
        ಪಕ್ಷ 2:
          ಯದಿ(data->this_cp <  0x0080) data->this_cp = UNICODE_INVALID;
          ಭಙ್ಗ;
        ಪಕ್ಷ 3:
          ಯದಿ(data->this_cp <  0x0800) data->this_cp = UNICODE_INVALID;
          ಭಙ್ಗ;
        ಪಕ್ಷ 4:
          ಯದಿ(data->this_cp < 0x10000) data->this_cp = UNICODE_INVALID;
          ಭಙ್ಗ;
        ಪಕ್ಷ 5:
          ಯದಿ(data->this_cp < 0x200000) data->this_cp = UNICODE_INVALID;
          ಭಙ್ಗ;
        ಪಕ್ಷ 6:
          ಯದಿ(data->this_cp < 0x4000000) data->this_cp = UNICODE_INVALID;
          ಭಙ್ಗ;
        }
        // Now look for plain invalid ones
        ಯದಿ((data->this_cp >= 0xD800 && data->this_cp <= 0xDFFF) ||
           data->this_cp == 0xFFFE ||
           data->this_cp == 0xFFFF)
          data->this_cp = UNICODE_INVALID;
#ifdef DEBUG_PRINT_UTF8
        printf(" char: U+%04x\n", data->this_cp);
#endif
        cp[(*cpi)++] = data->this_cp;
      }
    }

    ಅನ್ಯಥಾ ಯದಿ(c >= 0xc0 && c < 0xe0) {
      ಯದಿ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x1f;
      data->bytes_total = 2;
      data->bytes_remaining = 1;
    }

    ಅನ್ಯಥಾ ಯದಿ(c >= 0xe0 && c < 0xf0) {
      ಯದಿ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x0f;
      data->bytes_total = 3;
      data->bytes_remaining = 2;
    }

    ಅನ್ಯಥಾ ಯದಿ(c >= 0xf0 && c < 0xf8) {
      ಯದಿ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x07;
      data->bytes_total = 4;
      data->bytes_remaining = 3;
    }

    ಅನ್ಯಥಾ ಯದಿ(c >= 0xf8 && c < 0xfc) {
      ಯದಿ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x03;
      data->bytes_total = 5;
      data->bytes_remaining = 4;
    }

    ಅನ್ಯಥಾ ಯದಿ(c >= 0xfc && c < 0xfe) {
      ಯದಿ(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x01;
      data->bytes_total = 6;
      data->bytes_remaining = 5;
    }

    ಅನ್ಯಥಾ {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

ಸ್ಥಿರ VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

ಸ್ಥಿರ ಶೂನ್ಯ decode_usascii(VTermEncoding *enc UNUSED, ಶೂನ್ಯ *data UNUSED,
                           uint32_t cp[], ಪೂರ್ಣಾಙ್ಕ *cpi, ಪೂರ್ಣಾಙ್ಕ cplen,
                           ಧ್ರುವ ವರ್ಣ bytes[], size_t *pos, size_t bytelen)
{
  ಪೂರ್ಣಾಙ್ಕ is_gr = bytes[*pos] & 0x80;

  ಪ್ರತಿ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned ವರ್ಣ c = bytes[*pos] ^ is_gr;

    ಯದಿ(c < 0x20 || c == 0x7f || c >= 0x80)
      ಪ್ರತ್ಯಾಗಮ;

    cp[(*cpi)++] = c;
  }
}

ಸ್ಥಿರ VTermEncoding encoding_usascii = {
  ರಿಕ್ತ,           // init
  &decode_usascii // decode
};

ಸಂರಚನಾ StaticTableEncoding {
  ಧ್ರುವ VTermEncoding enc;
  ಧ್ರುವ uint32_t chars[128];
};

ಸ್ಥಿರ ಶೂನ್ಯ decode_table(VTermEncoding *enc, ಶೂನ್ಯ *data UNUSED,
                         uint32_t cp[], ಪೂರ್ಣಾಙ್ಕ *cpi, ಪೂರ್ಣಾಙ್ಕ cplen,
                         ಧ್ರುವ ವರ್ಣ bytes[], size_t *pos, size_t bytelen)
{
  ಸಂರಚನಾ StaticTableEncoding *table = (ಸಂರಚನಾ StaticTableEncoding *)enc;
  ಪೂರ್ಣಾಙ್ಕ is_gr = bytes[*pos] & 0x80;

  ಪ್ರತಿ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned ವರ್ಣ c = bytes[*pos] ^ is_gr;

    ಯದಿ(c < 0x20 || c == 0x7f || c >= 0x80)
      ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ(table->chars[c])
      cp[(*cpi)++] = table->chars[c];
    ಅನ್ಯಥಾ
      cp[(*cpi)++] = c;
  }
}

#ಅನ್ತರ್ಭಾವ "encoding/DECdrawing.inc"
#ಅನ್ತರ್ಭಾವ "encoding/uk.inc"

ಸ್ಥಿರ ಸಂರಚನಾ {
  VTermEncodingType type;
  ವರ್ಣ designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, ರಿಕ್ತ },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, ವರ್ಣ designation)
{
  ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i = 0; encodings[i].designation; i++)
    ಯದಿ(encodings[i].type == type && encodings[i].designation == designation)
      ಪ್ರತ್ಯಾಗಮ encodings[i].enc;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
