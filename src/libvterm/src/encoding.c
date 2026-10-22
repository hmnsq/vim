#ضمن "vterm_internal.h"

#تعریف_کن UNICODE_INVALID 0xFFFD

#إذا defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#پایان_اگر

بنية UTF8_Decoder_داده {
  // number of bytes remaining in this codepoint
  صحيح بایتها_مانده;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  صحيح بایتها_مجموع;

  صحيح this_cp;
};

ساكن فراغ init_utf8(VTermEncoding *enc استفادهنشده, فراغ *داده_4)
{
  بنية UTF8_Decoder_داده *داده = داده_4;

  داده->بایتها_مانده = 0;
  داده->بایتها_مجموع     = 0;
}

ساكن فراغ decode_utf8(VTermEncoding *enc استفادهنشده, فراغ *داده_4,
                        uint32_t cp[], صحيح *cpi, صحيح cplen,
                        ثابت حرف بایتها[], size_t *pos, size_t bytelen)
{
  بنية UTF8_Decoder_داده *داده = داده_4;

#اگر_تعریف_شده DEBUG_PRINT_UTF8
  چاپ_کن("BEGIN UTF-8\n");
#پایان_اگر

  لكل(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    بیعلامت حرف c = بایتها[*pos];

#اگر_تعریف_شده DEBUG_PRINT_UTF8
    چاپ_کن(" pos=%zd c=%02x rem=%d\n", *pos, c, داده->بایتها_مانده);
#پایان_اگر

    إذا(c < 0x20) // C0
      أرجع;

    وإلا إذا(c >= 0x20 && c < 0x7f) {
      إذا(داده->بایتها_مانده) {
        داده->بایتها_مانده = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	إذا (*cpi >= cplen)
	  اكسر;
      }
      cp[(*cpi)++] = c;
#اگر_تعریف_شده DEBUG_PRINT_UTF8
      چاپ_کن(" UTF-8 char: U+%04x\n", c);
#پایان_اگر
    }

    وإلا إذا(c == 0x7f) // DEL
      أرجع;

    وإلا إذا(c >= 0x80 && c < 0xc0) {
      إذا(!داده->بایتها_مانده) {
        cp[(*cpi)++] = UNICODE_INVALID;
        استمر;
      }

      داده->this_cp <<= 6;
      داده->this_cp |= c & 0x3f;
      داده->بایتها_مانده--;

      إذا(!داده->بایتها_مانده) {
#اگر_تعریف_شده DEBUG_PRINT_UTF8
        چاپ_کن(" UTF-8 raw char U+%04x bytelen=%d ", داده->this_cp, داده->بایتها_مجموع);
#پایان_اگر
        // Check for overlong sequences
        بدل(داده->بایتها_مجموع) {
        حالة 2:
          إذا(داده->this_cp <  0x0080) داده->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 3:
          إذا(داده->this_cp <  0x0800) داده->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 4:
          إذا(داده->this_cp < 0x10000) داده->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 5:
          إذا(داده->this_cp < 0x200000) داده->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 6:
          إذا(داده->this_cp < 0x4000000) داده->this_cp = UNICODE_INVALID;
          اكسر;
        }
        // Now look for plain invalid ones
        إذا((داده->this_cp >= 0xD800 && داده->this_cp <= 0xDFFF) ||
           داده->this_cp == 0xFFFE ||
           داده->this_cp == 0xFFFF)
          داده->this_cp = UNICODE_INVALID;
#اگر_تعریف_شده DEBUG_PRINT_UTF8
        چاپ_کن(" char: U+%04x\n", داده->this_cp);
#پایان_اگر
        cp[(*cpi)++] = داده->this_cp;
      }
    }

    وإلا إذا(c >= 0xc0 && c < 0xe0) {
      إذا(داده->بایتها_مانده)
        cp[(*cpi)++] = UNICODE_INVALID;

      داده->this_cp = c & 0x1f;
      داده->بایتها_مجموع = 2;
      داده->بایتها_مانده = 1;
    }

    وإلا إذا(c >= 0xe0 && c < 0xf0) {
      إذا(داده->بایتها_مانده)
        cp[(*cpi)++] = UNICODE_INVALID;

      داده->this_cp = c & 0x0f;
      داده->بایتها_مجموع = 3;
      داده->بایتها_مانده = 2;
    }

    وإلا إذا(c >= 0xf0 && c < 0xf8) {
      إذا(داده->بایتها_مانده)
        cp[(*cpi)++] = UNICODE_INVALID;

      داده->this_cp = c & 0x07;
      داده->بایتها_مجموع = 4;
      داده->بایتها_مانده = 3;
    }

    وإلا إذا(c >= 0xf8 && c < 0xfc) {
      إذا(داده->بایتها_مانده)
        cp[(*cpi)++] = UNICODE_INVALID;

      داده->this_cp = c & 0x03;
      داده->بایتها_مجموع = 5;
      داده->بایتها_مانده = 4;
    }

    وإلا إذا(c >= 0xfc && c < 0xfe) {
      إذا(داده->بایتها_مانده)
        cp[(*cpi)++] = UNICODE_INVALID;

      داده->this_cp = c & 0x01;
      داده->بایتها_مجموع = 6;
      داده->بایتها_مانده = 5;
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

ساكن فراغ decode_usascii(VTermEncoding *enc استفادهنشده, فراغ *داده استفادهنشده,
                           uint32_t cp[], صحيح *cpi, صحيح cplen,
                           ثابت حرف بایتها[], size_t *pos, size_t bytelen)
{
  صحيح is_gr = بایتها[*pos] & 0x80;

  لكل(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    بیعلامت حرف c = بایتها[*pos] ^ is_gr;

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

ساكن فراغ decode_جدول(VTermEncoding *enc, فراغ *داده استفادهنشده,
                         uint32_t cp[], صحيح *cpi, صحيح cplen,
                         ثابت حرف بایتها[], size_t *pos, size_t bytelen)
{
  بنية Static_جدول_Encoding *جدول = (بنية Static_جدول_Encoding *)enc;
  صحيح is_gr = بایتها[*pos] & 0x80;

  لكل(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    بیعلامت حرف c = بایتها[*pos] ^ is_gr;

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
