#अन्तर्भाव "vterm_internal.h"

#define UNICODE_INVALID 0xFFFD

#यदि defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#endif

संरचना UTF8DecoderData {
  // number of bytes remaining in this codepoint
  पूर्णाङ्क bytes_remaining;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  पूर्णाङ्क bytes_total;

  पूर्णाङ्क this_cp;
};

स्थिर शून्य init_utf8(VTermEncoding *enc UNUSED, शून्य *data_)
{
  संरचना UTF8DecoderData *data = data_;

  data->bytes_remaining = 0;
  data->bytes_total     = 0;
}

स्थिर शून्य decode_utf8(VTermEncoding *enc UNUSED, शून्य *data_,
                        uint32_t cp[], पूर्णाङ्क *cpi, पूर्णाङ्क cplen,
                        अचर वर्ण bytes[], माप_प्रकार *pos, माप_प्रकार bytelen)
{
  संरचना UTF8DecoderData *data = data_;

#ifdef DEBUG_PRINT_UTF8
  म_लिखो("BEGIN UTF-8\n");
#endif

  लागि(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    अचिन्हित वर्ण c = bytes[*pos];

#ifdef DEBUG_PRINT_UTF8
    म_लिखो(" pos=%zd c=%02x rem=%d\n", *pos, c, data->bytes_remaining);
#endif

    यदि(c < 0x20) // C0
      फर्काऊ;

    अन्यथा यदि(c >= 0x20 && c < 0x7f) {
      यदि(data->bytes_remaining) {
        data->bytes_remaining = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	यदि (*cpi >= cplen)
	  रोक;
      }
      cp[(*cpi)++] = c;
#ifdef DEBUG_PRINT_UTF8
      म_लिखो(" UTF-8 char: U+%04x\n", c);
#endif
    }

    अन्यथा यदि(c == 0x7f) // DEL
      फर्काऊ;

    अन्यथा यदि(c >= 0x80 && c < 0xc0) {
      यदि(!data->bytes_remaining) {
        cp[(*cpi)++] = UNICODE_INVALID;
        जारी;
      }

      data->this_cp <<= 6;
      data->this_cp |= c & 0x3f;
      data->bytes_remaining--;

      यदि(!data->bytes_remaining) {
#ifdef DEBUG_PRINT_UTF8
        म_लिखो(" UTF-8 raw char U+%04x bytelen=%d ", data->this_cp, data->bytes_total);
#endif
        // Check for overlong sequences
        विकल्प(data->bytes_total) {
        पक्ष 2:
          यदि(data->this_cp <  0x0080) data->this_cp = UNICODE_INVALID;
          रोक;
        पक्ष 3:
          यदि(data->this_cp <  0x0800) data->this_cp = UNICODE_INVALID;
          रोक;
        पक्ष 4:
          यदि(data->this_cp < 0x10000) data->this_cp = UNICODE_INVALID;
          रोक;
        पक्ष 5:
          यदि(data->this_cp < 0x200000) data->this_cp = UNICODE_INVALID;
          रोक;
        पक्ष 6:
          यदि(data->this_cp < 0x4000000) data->this_cp = UNICODE_INVALID;
          रोक;
        }
        // Now look for plain invalid ones
        यदि((data->this_cp >= 0xD800 && data->this_cp <= 0xDFFF) ||
           data->this_cp == 0xFFFE ||
           data->this_cp == 0xFFFF)
          data->this_cp = UNICODE_INVALID;
#ifdef DEBUG_PRINT_UTF8
        म_लिखो(" char: U+%04x\n", data->this_cp);
#endif
        cp[(*cpi)++] = data->this_cp;
      }
    }

    अन्यथा यदि(c >= 0xc0 && c < 0xe0) {
      यदि(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x1f;
      data->bytes_total = 2;
      data->bytes_remaining = 1;
    }

    अन्यथा यदि(c >= 0xe0 && c < 0xf0) {
      यदि(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x0f;
      data->bytes_total = 3;
      data->bytes_remaining = 2;
    }

    अन्यथा यदि(c >= 0xf0 && c < 0xf8) {
      यदि(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x07;
      data->bytes_total = 4;
      data->bytes_remaining = 3;
    }

    अन्यथा यदि(c >= 0xf8 && c < 0xfc) {
      यदि(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x03;
      data->bytes_total = 5;
      data->bytes_remaining = 4;
    }

    अन्यथा यदि(c >= 0xfc && c < 0xfe) {
      यदि(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x01;
      data->bytes_total = 6;
      data->bytes_remaining = 5;
    }

    अन्यथा {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

स्थिर VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

स्थिर शून्य decode_usascii(VTermEncoding *enc UNUSED, शून्य *data UNUSED,
                           uint32_t cp[], पूर्णाङ्क *cpi, पूर्णाङ्क cplen,
                           अचर वर्ण bytes[], माप_प्रकार *pos, माप_प्रकार bytelen)
{
  पूर्णाङ्क is_gr = bytes[*pos] & 0x80;

  लागि(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    अचिन्हित वर्ण c = bytes[*pos] ^ is_gr;

    यदि(c < 0x20 || c == 0x7f || c >= 0x80)
      फर्काऊ;

    cp[(*cpi)++] = c;
  }
}

स्थिर VTermEncoding encoding_usascii = {
  रिक्त,           // init
  &decode_usascii // decode
};

संरचना StaticTableEncoding {
  अचर VTermEncoding enc;
  अचर uint32_t chars[128];
};

स्थिर शून्य decode_table(VTermEncoding *enc, शून्य *data UNUSED,
                         uint32_t cp[], पूर्णाङ्क *cpi, पूर्णाङ्क cplen,
                         अचर वर्ण bytes[], माप_प्रकार *pos, माप_प्रकार bytelen)
{
  संरचना StaticTableEncoding *table = (संरचना StaticTableEncoding *)enc;
  पूर्णाङ्क is_gr = bytes[*pos] & 0x80;

  लागि(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    अचिन्हित वर्ण c = bytes[*pos] ^ is_gr;

    यदि(c < 0x20 || c == 0x7f || c >= 0x80)
      फर्काऊ;

    यदि(table->chars[c])
      cp[(*cpi)++] = table->chars[c];
    अन्यथा
      cp[(*cpi)++] = c;
  }
}

#अन्तर्भाव "encoding/DECdrawing.inc"
#अन्तर्भाव "encoding/uk.inc"

स्थिर संरचना {
  VTermEncodingType type;
  वर्ण designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, रिक्त },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, वर्ण designation)
{
  लागि(पूर्णाङ्क i = 0; encodings[i].designation; i++)
    यदि(encodings[i].type == type && encodings[i].designation == designation)
      फर्काऊ encodings[i].enc;
  फर्काऊ रिक्त;
}
