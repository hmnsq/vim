#شامل "vterm_internal.h"

#تعریف_کرو UNICODE_INVALID 0xFFFD

#اگر defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#اگر_ختم

ساخت UTF8_Decoder_ڈیٹا {
  // number of bytes remaining in this codepoint
  صحیح بائٹیں_بقایا;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  صحیح بائٹیں_کل;

  صحیح this_cp;
};

ساکن خالی init_utf8(VTermEncoding *enc غیر_مستعمل, خالی *ڈیٹا_4)
{
  ساخت UTF8_Decoder_ڈیٹا *ڈیٹا = ڈیٹا_4;

  ڈیٹا->بائٹیں_بقایا = 0;
  ڈیٹا->بائٹیں_کل     = 0;
}

ساکن خالی decode_utf8(VTermEncoding *enc غیر_مستعمل, خالی *ڈیٹا_4,
                        uint32_t cp[], صحیح *cpi, صحیح cplen,
                        ثابت حرف بائٹیں[], size_t *pos, size_t bytelen)
{
  ساخت UTF8_Decoder_ڈیٹا *ڈیٹا = ڈیٹا_4;

#اگر_تعریف_شدہ DEBUG_PRINT_UTF8
  لکھو("BEGIN UTF-8\n");
#اگر_ختم

  برائے(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    غیرعلامتی حرف c = بائٹیں[*pos];

#اگر_تعریف_شدہ DEBUG_PRINT_UTF8
    لکھو(" pos=%zd c=%02x rem=%d\n", *pos, c, ڈیٹا->بائٹیں_بقایا);
#اگر_ختم

    اگر(c < 0x20) // C0
      واپس;

    ورنہ اگر(c >= 0x20 && c < 0x7f) {
      اگر(ڈیٹا->بائٹیں_بقایا) {
        ڈیٹا->بائٹیں_بقایا = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	اگر (*cpi >= cplen)
	  توڑو;
      }
      cp[(*cpi)++] = c;
#اگر_تعریف_شدہ DEBUG_PRINT_UTF8
      لکھو(" UTF-8 char: U+%04x\n", c);
#اگر_ختم
    }

    ورنہ اگر(c == 0x7f) // DEL
      واپس;

    ورنہ اگر(c >= 0x80 && c < 0xc0) {
      اگر(!ڈیٹا->بائٹیں_بقایا) {
        cp[(*cpi)++] = UNICODE_INVALID;
        جاری;
      }

      ڈیٹا->this_cp <<= 6;
      ڈیٹا->this_cp |= c & 0x3f;
      ڈیٹا->بائٹیں_بقایا--;

      اگر(!ڈیٹا->بائٹیں_بقایا) {
#اگر_تعریف_شدہ DEBUG_PRINT_UTF8
        لکھو(" UTF-8 raw char U+%04x bytelen=%d ", ڈیٹا->this_cp, ڈیٹا->بائٹیں_کل);
#اگر_ختم
        // Check for overlong sequences
        انتخاب(ڈیٹا->بائٹیں_کل) {
        حالت 2:
          اگر(ڈیٹا->this_cp <  0x0080) ڈیٹا->this_cp = UNICODE_INVALID;
          توڑو;
        حالت 3:
          اگر(ڈیٹا->this_cp <  0x0800) ڈیٹا->this_cp = UNICODE_INVALID;
          توڑو;
        حالت 4:
          اگر(ڈیٹا->this_cp < 0x10000) ڈیٹا->this_cp = UNICODE_INVALID;
          توڑو;
        حالت 5:
          اگر(ڈیٹا->this_cp < 0x200000) ڈیٹا->this_cp = UNICODE_INVALID;
          توڑو;
        حالت 6:
          اگر(ڈیٹا->this_cp < 0x4000000) ڈیٹا->this_cp = UNICODE_INVALID;
          توڑو;
        }
        // Now look for plain invalid ones
        اگر((ڈیٹا->this_cp >= 0xD800 && ڈیٹا->this_cp <= 0xDFFF) ||
           ڈیٹا->this_cp == 0xFFFE ||
           ڈیٹا->this_cp == 0xFFFF)
          ڈیٹا->this_cp = UNICODE_INVALID;
#اگر_تعریف_شدہ DEBUG_PRINT_UTF8
        لکھو(" char: U+%04x\n", ڈیٹا->this_cp);
#اگر_ختم
        cp[(*cpi)++] = ڈیٹا->this_cp;
      }
    }

    ورنہ اگر(c >= 0xc0 && c < 0xe0) {
      اگر(ڈیٹا->بائٹیں_بقایا)
        cp[(*cpi)++] = UNICODE_INVALID;

      ڈیٹا->this_cp = c & 0x1f;
      ڈیٹا->بائٹیں_کل = 2;
      ڈیٹا->بائٹیں_بقایا = 1;
    }

    ورنہ اگر(c >= 0xe0 && c < 0xf0) {
      اگر(ڈیٹا->بائٹیں_بقایا)
        cp[(*cpi)++] = UNICODE_INVALID;

      ڈیٹا->this_cp = c & 0x0f;
      ڈیٹا->بائٹیں_کل = 3;
      ڈیٹا->بائٹیں_بقایا = 2;
    }

    ورنہ اگر(c >= 0xf0 && c < 0xf8) {
      اگر(ڈیٹا->بائٹیں_بقایا)
        cp[(*cpi)++] = UNICODE_INVALID;

      ڈیٹا->this_cp = c & 0x07;
      ڈیٹا->بائٹیں_کل = 4;
      ڈیٹا->بائٹیں_بقایا = 3;
    }

    ورنہ اگر(c >= 0xf8 && c < 0xfc) {
      اگر(ڈیٹا->بائٹیں_بقایا)
        cp[(*cpi)++] = UNICODE_INVALID;

      ڈیٹا->this_cp = c & 0x03;
      ڈیٹا->بائٹیں_کل = 5;
      ڈیٹا->بائٹیں_بقایا = 4;
    }

    ورنہ اگر(c >= 0xfc && c < 0xfe) {
      اگر(ڈیٹا->بائٹیں_بقایا)
        cp[(*cpi)++] = UNICODE_INVALID;

      ڈیٹا->this_cp = c & 0x01;
      ڈیٹا->بائٹیں_کل = 6;
      ڈیٹا->بائٹیں_بقایا = 5;
    }

    ورنہ {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

ساکن VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

ساکن خالی decode_usascii(VTermEncoding *enc غیر_مستعمل, خالی *ڈیٹا غیر_مستعمل,
                           uint32_t cp[], صحیح *cpi, صحیح cplen,
                           ثابت حرف بائٹیں[], size_t *pos, size_t bytelen)
{
  صحیح is_gr = بائٹیں[*pos] & 0x80;

  برائے(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    غیرعلامتی حرف c = بائٹیں[*pos] ^ is_gr;

    اگر(c < 0x20 || c == 0x7f || c >= 0x80)
      واپس;

    cp[(*cpi)++] = c;
  }
}

ساکن VTermEncoding encoding_usascii = {
  تہی,           // init
  &decode_usascii // decode
};

ساخت Static_جدول_Encoding {
  ثابت VTermEncoding enc;
  ثابت uint32_t chars[128];
};

ساکن خالی decode_جدول(VTermEncoding *enc, خالی *ڈیٹا غیر_مستعمل,
                         uint32_t cp[], صحیح *cpi, صحیح cplen,
                         ثابت حرف بائٹیں[], size_t *pos, size_t bytelen)
{
  ساخت Static_جدول_Encoding *جدول = (ساخت Static_جدول_Encoding *)enc;
  صحیح is_gr = بائٹیں[*pos] & 0x80;

  برائے(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    غیرعلامتی حرف c = بائٹیں[*pos] ^ is_gr;

    اگر(c < 0x20 || c == 0x7f || c >= 0x80)
      واپس;

    اگر(جدول->chars[c])
      cp[(*cpi)++] = جدول->chars[c];
    ورنہ
      cp[(*cpi)++] = c;
  }
}

#شامل "encoding/DECdrawing.inc"
#شامل "encoding/uk.inc"

ساکن ساخت {
  VTermEncodingType type;
  حرف designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, تہی },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, حرف designation)
{
  برائے(صحیح i = 0; encodings[i].designation; i++)
    اگر(encodings[i].type == type && encodings[i].designation == designation)
      واپس encodings[i].enc;
  واپس تہی;
}
