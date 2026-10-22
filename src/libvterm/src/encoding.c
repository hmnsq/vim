#অন্তর্ভাব "vterm_internal.h"

#define UNICODE_INVALID 0xFFFD

#যদি defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#endif

কাঠামো UTF8_Decoder_তথ্য {
  // number of bytes remaining in this codepoint
  পূর্ণসংখ্যা bytes_remaining;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  পূর্ণসংখ্যা bytes_যোগ;

  পূর্ণসংখ্যা this_cp;
};

স্থির শূন্য init_utf8(VTermEncoding *enc UNUSED, শূন্য *তথ্য_4)
{
  কাঠামো UTF8_Decoder_তথ্য *তথ্য = তথ্য_4;

  তথ্য->bytes_remaining = 0;
  তথ্য->bytes_যোগ     = 0;
}

স্থির শূন্য decode_utf8(VTermEncoding *enc UNUSED, শূন্য *তথ্য_4,
                        uint32_t cp[], পূর্ণসংখ্যা *cpi, পূর্ণসংখ্যা cplen,
                        ধ্রুবক অক্ষর bytes[], size_t *pos, size_t bytelen)
{
  কাঠামো UTF8_Decoder_তথ্য *তথ্য = তথ্য_4;

#ifdef DEBUG_PRINT_UTF8
  printf("BEGIN UTF-8\n");
#endif

  জন্য(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned অক্ষর c = bytes[*pos];

#ifdef DEBUG_PRINT_UTF8
    printf(" pos=%zd c=%02x rem=%d\n", *pos, c, তথ্য->bytes_remaining);
#endif

    যদি(c < 0x20) // C0
      ফেরত;

    অন্যথা যদি(c >= 0x20 && c < 0x7f) {
      যদি(তথ্য->bytes_remaining) {
        তথ্য->bytes_remaining = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	যদি (*cpi >= cplen)
	  বিরতি;
      }
      cp[(*cpi)++] = c;
#ifdef DEBUG_PRINT_UTF8
      printf(" UTF-8 char: U+%04x\n", c);
#endif
    }

    অন্যথা যদি(c == 0x7f) // DEL
      ফেরত;

    অন্যথা যদি(c >= 0x80 && c < 0xc0) {
      যদি(!তথ্য->bytes_remaining) {
        cp[(*cpi)++] = UNICODE_INVALID;
        চালু;
      }

      তথ্য->this_cp <<= 6;
      তথ্য->this_cp |= c & 0x3f;
      তথ্য->bytes_remaining--;

      যদি(!তথ্য->bytes_remaining) {
#ifdef DEBUG_PRINT_UTF8
        printf(" UTF-8 raw char U+%04x bytelen=%d ", তথ্য->this_cp, তথ্য->bytes_যোগ);
#endif
        // Check for overlong sequences
        নির্বাচন(তথ্য->bytes_যোগ) {
        ক্ষেত্র 2:
          যদি(তথ্য->this_cp <  0x0080) তথ্য->this_cp = UNICODE_INVALID;
          বিরতি;
        ক্ষেত্র 3:
          যদি(তথ্য->this_cp <  0x0800) তথ্য->this_cp = UNICODE_INVALID;
          বিরতি;
        ক্ষেত্র 4:
          যদি(তথ্য->this_cp < 0x10000) তথ্য->this_cp = UNICODE_INVALID;
          বিরতি;
        ক্ষেত্র 5:
          যদি(তথ্য->this_cp < 0x200000) তথ্য->this_cp = UNICODE_INVALID;
          বিরতি;
        ক্ষেত্র 6:
          যদি(তথ্য->this_cp < 0x4000000) তথ্য->this_cp = UNICODE_INVALID;
          বিরতি;
        }
        // Now look for plain invalid ones
        যদি((তথ্য->this_cp >= 0xD800 && তথ্য->this_cp <= 0xDFFF) ||
           তথ্য->this_cp == 0xFFFE ||
           তথ্য->this_cp == 0xFFFF)
          তথ্য->this_cp = UNICODE_INVALID;
#ifdef DEBUG_PRINT_UTF8
        printf(" char: U+%04x\n", তথ্য->this_cp);
#endif
        cp[(*cpi)++] = তথ্য->this_cp;
      }
    }

    অন্যথা যদি(c >= 0xc0 && c < 0xe0) {
      যদি(তথ্য->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      তথ্য->this_cp = c & 0x1f;
      তথ্য->bytes_যোগ = 2;
      তথ্য->bytes_remaining = 1;
    }

    অন্যথা যদি(c >= 0xe0 && c < 0xf0) {
      যদি(তথ্য->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      তথ্য->this_cp = c & 0x0f;
      তথ্য->bytes_যোগ = 3;
      তথ্য->bytes_remaining = 2;
    }

    অন্যথা যদি(c >= 0xf0 && c < 0xf8) {
      যদি(তথ্য->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      তথ্য->this_cp = c & 0x07;
      তথ্য->bytes_যোগ = 4;
      তথ্য->bytes_remaining = 3;
    }

    অন্যথা যদি(c >= 0xf8 && c < 0xfc) {
      যদি(তথ্য->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      তথ্য->this_cp = c & 0x03;
      তথ্য->bytes_যোগ = 5;
      তথ্য->bytes_remaining = 4;
    }

    অন্যথা যদি(c >= 0xfc && c < 0xfe) {
      যদি(তথ্য->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      তথ্য->this_cp = c & 0x01;
      তথ্য->bytes_যোগ = 6;
      তথ্য->bytes_remaining = 5;
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

স্থির শূন্য decode_usascii(VTermEncoding *enc UNUSED, শূন্য *তথ্য UNUSED,
                           uint32_t cp[], পূর্ণসংখ্যা *cpi, পূর্ণসংখ্যা cplen,
                           ধ্রুবক অক্ষর bytes[], size_t *pos, size_t bytelen)
{
  পূর্ণসংখ্যা is_gr = bytes[*pos] & 0x80;

  জন্য(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned অক্ষর c = bytes[*pos] ^ is_gr;

    যদি(c < 0x20 || c == 0x7f || c >= 0x80)
      ফেরত;

    cp[(*cpi)++] = c;
  }
}

স্থির VTermEncoding encoding_usascii = {
  ফাঁকা,           // init
  &decode_usascii // decode
};

কাঠামো StaticTableEncoding {
  ধ্রুবক VTermEncoding enc;
  ধ্রুবক uint32_t chars[128];
};

স্থির শূন্য decode_table(VTermEncoding *enc, শূন্য *তথ্য UNUSED,
                         uint32_t cp[], পূর্ণসংখ্যা *cpi, পূর্ণসংখ্যা cplen,
                         ধ্রুবক অক্ষর bytes[], size_t *pos, size_t bytelen)
{
  কাঠামো StaticTableEncoding *table = (কাঠামো StaticTableEncoding *)enc;
  পূর্ণসংখ্যা is_gr = bytes[*pos] & 0x80;

  জন্য(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned অক্ষর c = bytes[*pos] ^ is_gr;

    যদি(c < 0x20 || c == 0x7f || c >= 0x80)
      ফেরত;

    যদি(table->chars[c])
      cp[(*cpi)++] = table->chars[c];
    অন্যথা
      cp[(*cpi)++] = c;
  }
}

#অন্তর্ভাব "encoding/DECdrawing.inc"
#অন্তর্ভাব "encoding/uk.inc"

স্থির কাঠামো {
  VTermEncodingType type;
  অক্ষর designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, ফাঁকা },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, অক্ষর designation)
{
  জন্য(পূর্ণসংখ্যা i = 0; encodings[i].designation; i++)
    যদি(encodings[i].type == type && encodings[i].designation == designation)
      ফেরত encodings[i].enc;
  ফেরত ফাঁকা;
}
