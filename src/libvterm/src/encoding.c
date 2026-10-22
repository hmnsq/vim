#ਅਨ੍ਤਰ੍ਭਾਵ "vterm_internal.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_INVALID 0xFFFD

#ਜੇ defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#ਜੇ_ਅੰਤ

ਬਣਤਰ UTF8_Decoder_ਅੰਕੜੇ {
  // number of bytes remaining in this codepoint
  ਪੂਰਨ ਬਾਈਟਾਂ_ਬਚਿਆ;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  ਪੂਰਨ ਬਾਈਟਾਂ_ਕੁੱਲ;

  ਪੂਰਨ this_cp;
};

ਸਥਿਰ ਖਾਲੀ init_utf8(VTermEncoding *enc ਅਣਵਰਤਿਆ, ਖਾਲੀ *ਅੰਕੜੇ_4)
{
  ਬਣਤਰ UTF8_Decoder_ਅੰਕੜੇ *ਅੰਕੜੇ = ਅੰਕੜੇ_4;

  ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ = 0;
  ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਕੁੱਲ     = 0;
}

ਸਥਿਰ ਖਾਲੀ decode_utf8(VTermEncoding *enc ਅਣਵਰਤਿਆ, ਖਾਲੀ *ਅੰਕੜੇ_4,
                        uint32_t cp[], ਪੂਰਨ *cpi, ਪੂਰਨ cplen,
                        ਅਚਲ ਅੱਖਰ ਬਾਈਟਾਂ[], size_t *pos, size_t bytelen)
{
  ਬਣਤਰ UTF8_Decoder_ਅੰਕੜੇ *ਅੰਕੜੇ = ਅੰਕੜੇ_4;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_PRINT_UTF8
  ਲਿਖੋ("BEGIN UTF-8\n");
#ਜੇ_ਅੰਤ

  ਲਈ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c = ਬਾਈਟਾਂ[*pos];

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_PRINT_UTF8
    ਲਿਖੋ(" pos=%zd c=%02x rem=%d\n", *pos, c, ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ);
#ਜੇ_ਅੰਤ

    ਜੇ(c < 0x20) // C0
      ਵਾਪਸ;

    ਨਹੀਂਤਾਂ ਜੇ(c >= 0x20 && c < 0x7f) {
      ਜੇ(ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ) {
        ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	ਜੇ (*cpi >= cplen)
	  ਰੋਕੋ;
      }
      cp[(*cpi)++] = c;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_PRINT_UTF8
      ਲਿਖੋ(" UTF-8 char: U+%04x\n", c);
#ਜੇ_ਅੰਤ
    }

    ਨਹੀਂਤਾਂ ਜੇ(c == 0x7f) // DEL
      ਵਾਪਸ;

    ਨਹੀਂਤਾਂ ਜੇ(c >= 0x80 && c < 0xc0) {
      ਜੇ(!ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ) {
        cp[(*cpi)++] = UNICODE_INVALID;
        ਜਾਰੀ;
      }

      ਅੰਕੜੇ->this_cp <<= 6;
      ਅੰਕੜੇ->this_cp |= c & 0x3f;
      ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ--;

      ਜੇ(!ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_PRINT_UTF8
        ਲਿਖੋ(" UTF-8 raw char U+%04x bytelen=%d ", ਅੰਕੜੇ->this_cp, ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਕੁੱਲ);
#ਜੇ_ਅੰਤ
        // Check for overlong sequences
        ਚੋਣ(ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਕੁੱਲ) {
        ਹਾਲਤ 2:
          ਜੇ(ਅੰਕੜੇ->this_cp <  0x0080) ਅੰਕੜੇ->this_cp = UNICODE_INVALID;
          ਰੋਕੋ;
        ਹਾਲਤ 3:
          ਜੇ(ਅੰਕੜੇ->this_cp <  0x0800) ਅੰਕੜੇ->this_cp = UNICODE_INVALID;
          ਰੋਕੋ;
        ਹਾਲਤ 4:
          ਜੇ(ਅੰਕੜੇ->this_cp < 0x10000) ਅੰਕੜੇ->this_cp = UNICODE_INVALID;
          ਰੋਕੋ;
        ਹਾਲਤ 5:
          ਜੇ(ਅੰਕੜੇ->this_cp < 0x200000) ਅੰਕੜੇ->this_cp = UNICODE_INVALID;
          ਰੋਕੋ;
        ਹਾਲਤ 6:
          ਜੇ(ਅੰਕੜੇ->this_cp < 0x4000000) ਅੰਕੜੇ->this_cp = UNICODE_INVALID;
          ਰੋਕੋ;
        }
        // Now look for plain invalid ones
        ਜੇ((ਅੰਕੜੇ->this_cp >= 0xD800 && ਅੰਕੜੇ->this_cp <= 0xDFFF) ||
           ਅੰਕੜੇ->this_cp == 0xFFFE ||
           ਅੰਕੜੇ->this_cp == 0xFFFF)
          ਅੰਕੜੇ->this_cp = UNICODE_INVALID;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_PRINT_UTF8
        ਲਿਖੋ(" char: U+%04x\n", ਅੰਕੜੇ->this_cp);
#ਜੇ_ਅੰਤ
        cp[(*cpi)++] = ਅੰਕੜੇ->this_cp;
      }
    }

    ਨਹੀਂਤਾਂ ਜੇ(c >= 0xc0 && c < 0xe0) {
      ਜੇ(ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ)
        cp[(*cpi)++] = UNICODE_INVALID;

      ਅੰਕੜੇ->this_cp = c & 0x1f;
      ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਕੁੱਲ = 2;
      ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ = 1;
    }

    ਨਹੀਂਤਾਂ ਜੇ(c >= 0xe0 && c < 0xf0) {
      ਜੇ(ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ)
        cp[(*cpi)++] = UNICODE_INVALID;

      ਅੰਕੜੇ->this_cp = c & 0x0f;
      ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਕੁੱਲ = 3;
      ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ = 2;
    }

    ਨਹੀਂਤਾਂ ਜੇ(c >= 0xf0 && c < 0xf8) {
      ਜੇ(ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ)
        cp[(*cpi)++] = UNICODE_INVALID;

      ਅੰਕੜੇ->this_cp = c & 0x07;
      ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਕੁੱਲ = 4;
      ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ = 3;
    }

    ਨਹੀਂਤਾਂ ਜੇ(c >= 0xf8 && c < 0xfc) {
      ਜੇ(ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ)
        cp[(*cpi)++] = UNICODE_INVALID;

      ਅੰਕੜੇ->this_cp = c & 0x03;
      ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਕੁੱਲ = 5;
      ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ = 4;
    }

    ਨਹੀਂਤਾਂ ਜੇ(c >= 0xfc && c < 0xfe) {
      ਜੇ(ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ)
        cp[(*cpi)++] = UNICODE_INVALID;

      ਅੰਕੜੇ->this_cp = c & 0x01;
      ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਕੁੱਲ = 6;
      ਅੰਕੜੇ->ਬਾਈਟਾਂ_ਬਚਿਆ = 5;
    }

    ਨਹੀਂਤਾਂ {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

ਸਥਿਰ VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

ਸਥਿਰ ਖਾਲੀ decode_usascii(VTermEncoding *enc ਅਣਵਰਤਿਆ, ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
                           uint32_t cp[], ਪੂਰਨ *cpi, ਪੂਰਨ cplen,
                           ਅਚਲ ਅੱਖਰ ਬਾਈਟਾਂ[], size_t *pos, size_t bytelen)
{
  ਪੂਰਨ is_gr = ਬਾਈਟਾਂ[*pos] & 0x80;

  ਲਈ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c = ਬਾਈਟਾਂ[*pos] ^ is_gr;

    ਜੇ(c < 0x20 || c == 0x7f || c >= 0x80)
      ਵਾਪਸ;

    cp[(*cpi)++] = c;
  }
}

ਸਥਿਰ VTermEncoding encoding_usascii = {
  ਨਿਰਾ,           // init
  &decode_usascii // decode
};

ਬਣਤਰ Static_ਸਾਰਣੀ_Encoding {
  ਅਚਲ VTermEncoding enc;
  ਅਚਲ uint32_t chars[128];
};

ਸਥਿਰ ਖਾਲੀ decode_ਸਾਰਣੀ(VTermEncoding *enc, ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
                         uint32_t cp[], ਪੂਰਨ *cpi, ਪੂਰਨ cplen,
                         ਅਚਲ ਅੱਖਰ ਬਾਈਟਾਂ[], size_t *pos, size_t bytelen)
{
  ਬਣਤਰ Static_ਸਾਰਣੀ_Encoding *ਸਾਰਣੀ = (ਬਣਤਰ Static_ਸਾਰਣੀ_Encoding *)enc;
  ਪੂਰਨ is_gr = ਬਾਈਟਾਂ[*pos] & 0x80;

  ਲਈ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c = ਬਾਈਟਾਂ[*pos] ^ is_gr;

    ਜੇ(c < 0x20 || c == 0x7f || c >= 0x80)
      ਵਾਪਸ;

    ਜੇ(ਸਾਰਣੀ->chars[c])
      cp[(*cpi)++] = ਸਾਰਣੀ->chars[c];
    ਨਹੀਂਤਾਂ
      cp[(*cpi)++] = c;
  }
}

#ਅਨ੍ਤਰ੍ਭਾਵ "encoding/DECdrawing.inc"
#ਅਨ੍ਤਰ੍ਭਾਵ "encoding/uk.inc"

ਸਥਿਰ ਬਣਤਰ {
  VTermEncodingType type;
  ਅੱਖਰ designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, ਨਿਰਾ },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, ਅੱਖਰ designation)
{
  ਲਈ(ਪੂਰਨ i = 0; encodings[i].designation; i++)
    ਜੇ(encodings[i].type == type && encodings[i].designation == designation)
      ਵਾਪਸ encodings[i].enc;
  ਵਾਪਸ ਨਿਰਾ;
}
