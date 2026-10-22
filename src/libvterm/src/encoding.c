#ضمن "vterm_internal.h"

#تعریف_کرو UNICODE_INVALID 0xFFFD

#إذا defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#جے_انت

بنية UTF8_Decoder_انکڑے {
  // number of bytes remaining in this codepoint
  صحيح بائٹاں_بچیا;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  صحيح بائٹاں_کل;

  صحيح this_cp;
};

ساكن فراغ init_utf8(VTermEncoding *enc بن_ورتیا, فراغ *انکڑے_4)
{
  بنية UTF8_Decoder_انکڑے *انکڑے = انکڑے_4;

  انکڑے->بائٹاں_بچیا = 0;
  انکڑے->بائٹاں_کل     = 0;
}

ساكن فراغ decode_utf8(VTermEncoding *enc بن_ورتیا, فراغ *انکڑے_4,
                        uint32_t cp[], صحيح *cpi, صحيح cplen,
                        ثابت حرف بائٹاں[], size_t *pos, size_t bytelen)
{
  بنية UTF8_Decoder_انکڑے *انکڑے = انکڑے_4;

#جے_تعریف_شدہ DEBUG_PRINT_UTF8
  لکھو("BEGIN UTF-8\n");
#جے_انت

  لكل(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    بنانشانی حرف c = بائٹاں[*pos];

#جے_تعریف_شدہ DEBUG_PRINT_UTF8
    لکھو(" pos=%zd c=%02x rem=%d\n", *pos, c, انکڑے->بائٹاں_بچیا);
#جے_انت

    إذا(c < 0x20) // C0
      أرجع;

    وإلا إذا(c >= 0x20 && c < 0x7f) {
      إذا(انکڑے->بائٹاں_بچیا) {
        انکڑے->بائٹاں_بچیا = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	إذا (*cpi >= cplen)
	  اكسر;
      }
      cp[(*cpi)++] = c;
#جے_تعریف_شدہ DEBUG_PRINT_UTF8
      لکھو(" UTF-8 char: U+%04x\n", c);
#جے_انت
    }

    وإلا إذا(c == 0x7f) // DEL
      أرجع;

    وإلا إذا(c >= 0x80 && c < 0xc0) {
      إذا(!انکڑے->بائٹاں_بچیا) {
        cp[(*cpi)++] = UNICODE_INVALID;
        استمر;
      }

      انکڑے->this_cp <<= 6;
      انکڑے->this_cp |= c & 0x3f;
      انکڑے->بائٹاں_بچیا--;

      إذا(!انکڑے->بائٹاں_بچیا) {
#جے_تعریف_شدہ DEBUG_PRINT_UTF8
        لکھو(" UTF-8 raw char U+%04x bytelen=%d ", انکڑے->this_cp, انکڑے->بائٹاں_کل);
#جے_انت
        // Check for overlong sequences
        بدل(انکڑے->بائٹاں_کل) {
        حالة 2:
          إذا(انکڑے->this_cp <  0x0080) انکڑے->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 3:
          إذا(انکڑے->this_cp <  0x0800) انکڑے->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 4:
          إذا(انکڑے->this_cp < 0x10000) انکڑے->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 5:
          إذا(انکڑے->this_cp < 0x200000) انکڑے->this_cp = UNICODE_INVALID;
          اكسر;
        حالة 6:
          إذا(انکڑے->this_cp < 0x4000000) انکڑے->this_cp = UNICODE_INVALID;
          اكسر;
        }
        // Now look for plain invalid ones
        إذا((انکڑے->this_cp >= 0xD800 && انکڑے->this_cp <= 0xDFFF) ||
           انکڑے->this_cp == 0xFFFE ||
           انکڑے->this_cp == 0xFFFF)
          انکڑے->this_cp = UNICODE_INVALID;
#جے_تعریف_شدہ DEBUG_PRINT_UTF8
        لکھو(" char: U+%04x\n", انکڑے->this_cp);
#جے_انت
        cp[(*cpi)++] = انکڑے->this_cp;
      }
    }

    وإلا إذا(c >= 0xc0 && c < 0xe0) {
      إذا(انکڑے->بائٹاں_بچیا)
        cp[(*cpi)++] = UNICODE_INVALID;

      انکڑے->this_cp = c & 0x1f;
      انکڑے->بائٹاں_کل = 2;
      انکڑے->بائٹاں_بچیا = 1;
    }

    وإلا إذا(c >= 0xe0 && c < 0xf0) {
      إذا(انکڑے->بائٹاں_بچیا)
        cp[(*cpi)++] = UNICODE_INVALID;

      انکڑے->this_cp = c & 0x0f;
      انکڑے->بائٹاں_کل = 3;
      انکڑے->بائٹاں_بچیا = 2;
    }

    وإلا إذا(c >= 0xf0 && c < 0xf8) {
      إذا(انکڑے->بائٹاں_بچیا)
        cp[(*cpi)++] = UNICODE_INVALID;

      انکڑے->this_cp = c & 0x07;
      انکڑے->بائٹاں_کل = 4;
      انکڑے->بائٹاں_بچیا = 3;
    }

    وإلا إذا(c >= 0xf8 && c < 0xfc) {
      إذا(انکڑے->بائٹاں_بچیا)
        cp[(*cpi)++] = UNICODE_INVALID;

      انکڑے->this_cp = c & 0x03;
      انکڑے->بائٹاں_کل = 5;
      انکڑے->بائٹاں_بچیا = 4;
    }

    وإلا إذا(c >= 0xfc && c < 0xfe) {
      إذا(انکڑے->بائٹاں_بچیا)
        cp[(*cpi)++] = UNICODE_INVALID;

      انکڑے->this_cp = c & 0x01;
      انکڑے->بائٹاں_کل = 6;
      انکڑے->بائٹاں_بچیا = 5;
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

ساكن فراغ decode_usascii(VTermEncoding *enc بن_ورتیا, فراغ *انکڑے بن_ورتیا,
                           uint32_t cp[], صحيح *cpi, صحيح cplen,
                           ثابت حرف بائٹاں[], size_t *pos, size_t bytelen)
{
  صحيح is_gr = بائٹاں[*pos] & 0x80;

  لكل(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    بنانشانی حرف c = بائٹاں[*pos] ^ is_gr;

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

ساكن فراغ decode_جدول(VTermEncoding *enc, فراغ *انکڑے بن_ورتیا,
                         uint32_t cp[], صحيح *cpi, صحيح cplen,
                         ثابت حرف بائٹاں[], size_t *pos, size_t bytelen)
{
  بنية Static_جدول_Encoding *جدول = (بنية Static_جدول_Encoding *)enc;
  صحيح is_gr = بائٹاں[*pos] & 0x80;

  لكل(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    بنانشانی حرف c = بائٹاں[*pos] ^ is_gr;

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
