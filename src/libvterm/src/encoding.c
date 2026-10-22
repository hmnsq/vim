#ضمن "vterm_internal.h"

#وصف_ڪريو UNICODE_INVALID 0xFFFD

#إذا defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#جيڪڏهن_پورو

بنية UTF8_Decoder_معلومات {
  // number of bytes remaining in this codepoint
  صحيح بائيٽون_بچيل;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  صحيح بائيٽون_ڪل;

  صحيح this_cp;
};

ساكن فراغ init_utf8(VTermEncoding *enc اڻ_استعمال_ٿيل, فراغ *معلومات_4)
{
  بنية UTF8_Decoder_معلومات *معلومات = معلومات_4;

  معلومات->بائيٽون_بچيل = 0;
  معلومات->بائيٽون_ڪل     = 0;
}

ساكن فراغ decode_utf8(VTermEncoding *enc اڻ_استعمال_ٿيل, فراغ *معلومات_4,
                        uint32_t cp[], صحيح *cpi, صحيح cplen,
                        ثابت حرف بائيٽون[], size_t *pos, size_t bytelen)
{
  بنية UTF8_Decoder_معلومات *معلومات = معلومات_4;

#جيڪڏهن_وصف_ٿيل DEBUG_PRINT_UTF8
  لکو("BEGIN UTF-8\n");
#جيڪڏهن_پورو

  لكل(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    بنانشان حرف c = بائيٽون[*pos];

#جيڪڏهن_وصف_ٿيل DEBUG_PRINT_UTF8
    لکو(" pos=%zd c=%02x rem=%d\n", *pos, c, معلومات->بائيٽون_بچيل);
#جيڪڏهن_پورو

    إذا(c < 0x20) // C0
      أرجع;

    وإلا إذا(c >= 0x20 && c < 0x7f) {
      إذا(معلومات->بائيٽون_بچيل) {
        معلومات->بائيٽون_بچيل = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	إذا (*cpi >= cplen)
	  اكسر;
      }
      cp[(*cpi)++] = c;
#جيڪڏهن_وصف_ٿيل DEBUG_PRINT_UTF8
      لکو(" UTF-8 char: U+%04x\n", c);
#جيڪڏهن_پورو
    }

    وإلا إذا(c == 0x7f) // DEL
      أرجع;

    وإلا إذا(c >= 0x80 && c < 0xc0) {
      إذا(!معلومات->بائيٽون_بچيل) {
        cp[(*cpi)++] = UNICODE_INVALID;
        استمر;
      }

      معلومات->this_cp <<= 6;
      معلومات->this_cp |= c & 0x3f;
      معلومات->بائيٽون_بچيل--;

      إذا(!معلومات->بائيٽون_بچيل) {
#جيڪڏهن_وصف_ٿيل DEBUG_PRINT_UTF8
        لکو(" UTF-8 raw char U+%04x bytelen=%d ", معلومات->this_cp, معلومات->بائيٽون_ڪل);
#جيڪڏهن_پورو
        // Check for overlong sequences
        بدل(معلومات->بائيٽون_ڪل) {
        حالة 2:
          إذا(معلومات->this_cp <  0x0080) معلومات->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 3:
          إذا(معلومات->this_cp <  0x0800) معلومات->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 4:
          إذا(معلومات->this_cp < 0x10000) معلومات->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 5:
          إذا(معلومات->this_cp < 0x200000) معلومات->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 6:
          إذا(معلومات->this_cp < 0x4000000) معلومات->this_cp = UNICODE_INVALID;
          اكسر;
        }
        // Now look for plain invalid ones
        إذا((معلومات->this_cp >= 0xD800 && معلومات->this_cp <= 0xDFFF) ||
           معلومات->this_cp == 0xFFFE ||
           معلومات->this_cp == 0xFFFF)
          معلومات->this_cp = UNICODE_INVALID;
#جيڪڏهن_وصف_ٿيل DEBUG_PRINT_UTF8
        لکو(" char: U+%04x\n", معلومات->this_cp);
#جيڪڏهن_پورو
        cp[(*cpi)++] = معلومات->this_cp;
      }
    }

    وإلا إذا(c >= 0xc0 && c < 0xe0) {
      إذا(معلومات->بائيٽون_بچيل)
        cp[(*cpi)++] = UNICODE_INVALID;

      معلومات->this_cp = c & 0x1f;
      معلومات->بائيٽون_ڪل = 2;
      معلومات->بائيٽون_بچيل = 1;
    }

    وإلا إذا(c >= 0xe0 && c < 0xf0) {
      إذا(معلومات->بائيٽون_بچيل)
        cp[(*cpi)++] = UNICODE_INVALID;

      معلومات->this_cp = c & 0x0f;
      معلومات->بائيٽون_ڪل = 3;
      معلومات->بائيٽون_بچيل = 2;
    }

    وإلا إذا(c >= 0xf0 && c < 0xf8) {
      إذا(معلومات->بائيٽون_بچيل)
        cp[(*cpi)++] = UNICODE_INVALID;

      معلومات->this_cp = c & 0x07;
      معلومات->بائيٽون_ڪل = 4;
      معلومات->بائيٽون_بچيل = 3;
    }

    وإلا إذا(c >= 0xf8 && c < 0xfc) {
      إذا(معلومات->بائيٽون_بچيل)
        cp[(*cpi)++] = UNICODE_INVALID;

      معلومات->this_cp = c & 0x03;
      معلومات->بائيٽون_ڪل = 5;
      معلومات->بائيٽون_بچيل = 4;
    }

    وإلا إذا(c >= 0xfc && c < 0xfe) {
      إذا(معلومات->بائيٽون_بچيل)
        cp[(*cpi)++] = UNICODE_INVALID;

      معلومات->this_cp = c & 0x01;
      معلومات->بائيٽون_ڪل = 6;
      معلومات->بائيٽون_بچيل = 5;
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

ساكن فراغ decode_usascii(VTermEncoding *enc اڻ_استعمال_ٿيل, فراغ *معلومات اڻ_استعمال_ٿيل,
                           uint32_t cp[], صحيح *cpi, صحيح cplen,
                           ثابت حرف بائيٽون[], size_t *pos, size_t bytelen)
{
  صحيح is_gr = بائيٽون[*pos] & 0x80;

  لكل(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    بنانشان حرف c = بائيٽون[*pos] ^ is_gr;

    إذا(c < 0x20 || c == 0x7f || c >= 0x80)
      أرجع;

    cp[(*cpi)++] = c;
  }
}

ساكن VTermEncoding encoding_usascii = {
  عدم,           // init
  &decode_usascii // decode
};

بنية Static_جدول_Encoding {
  ثابت VTermEncoding enc;
  ثابت uint32_t chars[128];
};

ساكن فراغ decode_جدول(VTermEncoding *enc, فراغ *معلومات اڻ_استعمال_ٿيل,
                         uint32_t cp[], صحيح *cpi, صحيح cplen,
                         ثابت حرف بائيٽون[], size_t *pos, size_t bytelen)
{
  بنية Static_جدول_Encoding *جدول = (بنية Static_جدول_Encoding *)enc;
  صحيح is_gr = بائيٽون[*pos] & 0x80;

  لكل(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    بنانشان حرف c = بائيٽون[*pos] ^ is_gr;

    إذا(c < 0x20 || c == 0x7f || c >= 0x80)
      أرجع;

    إذا(جدول->chars[c])
      cp[(*cpi)++] = جدول->chars[c];
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
    إذا(encodings[i].type == type && encodings[i].designation == designation)
      أرجع encodings[i].enc;
  أرجع عدم;
}
