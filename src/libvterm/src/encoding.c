#ಅನ್ತರ್ಭಾವ "vterm_internal.h"

#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_INVALID 0xFFFD

#ಆದರೆ defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ UTF8_Decoder_ದತ್ತಾಂಶ {
  // number of bytes remaining in this codepoint
  ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು_ಉಳಿದ;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು_ಒಟ್ಟು;

  ಪೂರ್ಣಾಂಕ this_cp;
};

ಸ್ಥಿರ ಶೂನ್ಯ init_utf8(VTermEncoding *enc ಬಳಸದ, ಶೂನ್ಯ *ದತ್ತಾಂಶ_4)
{
  ರಚನೆ UTF8_Decoder_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ = ದತ್ತಾಂಶ_4;

  ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ = 0;
  ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಒಟ್ಟು     = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ decode_utf8(VTermEncoding *enc ಬಳಸದ, ಶೂನ್ಯ *ದತ್ತಾಂಶ_4,
                        uint32_t cp[], ಪೂರ್ಣಾಂಕ *cpi, ಪೂರ್ಣಾಂಕ cplen,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ಬೈಟುಗಳು[], size_t *pos, size_t bytelen)
{
  ರಚನೆ UTF8_Decoder_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ = ದತ್ತಾಂಶ_4;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_PRINT_UTF8
  ಬರೆ("BEGIN UTF-8\n");
#ಆದರೆ_ಅಂತ್ಯ

  ಗಾಗಿ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c = ಬೈಟುಗಳು[*pos];

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_PRINT_UTF8
    ಬರೆ(" pos=%zd c=%02x rem=%d\n", *pos, c, ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(c < 0x20) // C0
      ಹಿಂತಿರುಗಿ;

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c >= 0x20 && c < 0x7f) {
      ಆದರೆ(ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ) {
        ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	ಆದರೆ (*cpi >= cplen)
	  ನಿಲ್ಲಿಸು;
      }
      cp[(*cpi)++] = c;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_PRINT_UTF8
      ಬರೆ(" UTF-8 char: U+%04x\n", c);
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == 0x7f) // DEL
      ಹಿಂತಿರುಗಿ;

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c >= 0x80 && c < 0xc0) {
      ಆದರೆ(!ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ) {
        cp[(*cpi)++] = UNICODE_INVALID;
        ಮುಂದುವರಿ;
      }

      ದತ್ತಾಂಶ->this_cp <<= 6;
      ದತ್ತಾಂಶ->this_cp |= c & 0x3f;
      ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ--;

      ಆದರೆ(!ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_PRINT_UTF8
        ಬರೆ(" UTF-8 raw char U+%04x bytelen=%d ", ದತ್ತಾಂಶ->this_cp, ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಒಟ್ಟು);
#ಆದರೆ_ಅಂತ್ಯ
        // Check for overlong sequences
        ಆಯ್ಕೆ(ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಒಟ್ಟು) {
        ಸ್ಥಿತಿ 2:
          ಆದರೆ(ದತ್ತಾಂಶ->this_cp <  0x0080) ದತ್ತಾಂಶ->this_cp = UNICODE_INVALID;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 3:
          ಆದರೆ(ದತ್ತಾಂಶ->this_cp <  0x0800) ದತ್ತಾಂಶ->this_cp = UNICODE_INVALID;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 4:
          ಆದರೆ(ದತ್ತಾಂಶ->this_cp < 0x10000) ದತ್ತಾಂಶ->this_cp = UNICODE_INVALID;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 5:
          ಆದರೆ(ದತ್ತಾಂಶ->this_cp < 0x200000) ದತ್ತಾಂಶ->this_cp = UNICODE_INVALID;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 6:
          ಆದರೆ(ದತ್ತಾಂಶ->this_cp < 0x4000000) ದತ್ತಾಂಶ->this_cp = UNICODE_INVALID;
          ನಿಲ್ಲಿಸು;
        }
        // Now look for plain invalid ones
        ಆದರೆ((ದತ್ತಾಂಶ->this_cp >= 0xD800 && ದತ್ತಾಂಶ->this_cp <= 0xDFFF) ||
           ದತ್ತಾಂಶ->this_cp == 0xFFFE ||
           ದತ್ತಾಂಶ->this_cp == 0xFFFF)
          ದತ್ತಾಂಶ->this_cp = UNICODE_INVALID;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_PRINT_UTF8
        ಬರೆ(" char: U+%04x\n", ದತ್ತಾಂಶ->this_cp);
#ಆದರೆ_ಅಂತ್ಯ
        cp[(*cpi)++] = ದತ್ತಾಂಶ->this_cp;
      }
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c >= 0xc0 && c < 0xe0) {
      ಆದರೆ(ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ)
        cp[(*cpi)++] = UNICODE_INVALID;

      ದತ್ತಾಂಶ->this_cp = c & 0x1f;
      ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಒಟ್ಟು = 2;
      ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ = 1;
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c >= 0xe0 && c < 0xf0) {
      ಆದರೆ(ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ)
        cp[(*cpi)++] = UNICODE_INVALID;

      ದತ್ತಾಂಶ->this_cp = c & 0x0f;
      ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಒಟ್ಟು = 3;
      ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ = 2;
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c >= 0xf0 && c < 0xf8) {
      ಆದರೆ(ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ)
        cp[(*cpi)++] = UNICODE_INVALID;

      ದತ್ತಾಂಶ->this_cp = c & 0x07;
      ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಒಟ್ಟು = 4;
      ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ = 3;
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c >= 0xf8 && c < 0xfc) {
      ಆದರೆ(ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ)
        cp[(*cpi)++] = UNICODE_INVALID;

      ದತ್ತಾಂಶ->this_cp = c & 0x03;
      ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಒಟ್ಟು = 5;
      ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ = 4;
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c >= 0xfc && c < 0xfe) {
      ಆದರೆ(ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ)
        cp[(*cpi)++] = UNICODE_INVALID;

      ದತ್ತಾಂಶ->this_cp = c & 0x01;
      ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಒಟ್ಟು = 6;
      ದತ್ತಾಂಶ->ಬೈಟುಗಳು_ಉಳಿದ = 5;
    }

    ಇಲ್ಲದಿದ್ದರೆ {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

ಸ್ಥಿರ VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

ಸ್ಥಿರ ಶೂನ್ಯ decode_usascii(VTermEncoding *enc ಬಳಸದ, ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ,
                           uint32_t cp[], ಪೂರ್ಣಾಂಕ *cpi, ಪೂರ್ಣಾಂಕ cplen,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ಬೈಟುಗಳು[], size_t *pos, size_t bytelen)
{
  ಪೂರ್ಣಾಂಕ is_gr = ಬೈಟುಗಳು[*pos] & 0x80;

  ಗಾಗಿ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c = ಬೈಟುಗಳು[*pos] ^ is_gr;

    ಆದರೆ(c < 0x20 || c == 0x7f || c >= 0x80)
      ಹಿಂತಿರುಗಿ;

    cp[(*cpi)++] = c;
  }
}

ಸ್ಥಿರ VTermEncoding encoding_usascii = {
  ಖಾಲಿ,           // init
  &decode_usascii // decode
};

ರಚನೆ Static_ಕೋಷ್ಟಕ_Encoding {
  ಸ್ಥಿರಾಂಕ VTermEncoding enc;
  ಸ್ಥಿರಾಂಕ uint32_t chars[128];
};

ಸ್ಥಿರ ಶೂನ್ಯ decode_ಕೋಷ್ಟಕ(VTermEncoding *enc, ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ,
                         uint32_t cp[], ಪೂರ್ಣಾಂಕ *cpi, ಪೂರ್ಣಾಂಕ cplen,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ಬೈಟುಗಳು[], size_t *pos, size_t bytelen)
{
  ರಚನೆ Static_ಕೋಷ್ಟಕ_Encoding *ಕೋಷ್ಟಕ = (ರಚನೆ Static_ಕೋಷ್ಟಕ_Encoding *)enc;
  ಪೂರ್ಣಾಂಕ is_gr = ಬೈಟುಗಳು[*pos] & 0x80;

  ಗಾಗಿ(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c = ಬೈಟುಗಳು[*pos] ^ is_gr;

    ಆದರೆ(c < 0x20 || c == 0x7f || c >= 0x80)
      ಹಿಂತಿರುಗಿ;

    ಆದರೆ(ಕೋಷ್ಟಕ->chars[c])
      cp[(*cpi)++] = ಕೋಷ್ಟಕ->chars[c];
    ಇಲ್ಲದಿದ್ದರೆ
      cp[(*cpi)++] = c;
  }
}

#ಅನ್ತರ್ಭಾವ "encoding/DECdrawing.inc"
#ಅನ್ತರ್ಭಾವ "encoding/uk.inc"

ಸ್ಥಿರ ರಚನೆ {
  VTermEncodingType type;
  ಅಕ್ಷರ designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, ಖಾಲಿ },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, ಅಕ್ಷರ designation)
{
  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 0; encodings[i].designation; i++)
    ಆದರೆ(encodings[i].type == type && encodings[i].designation == designation)
      ಹಿಂತಿರುಗಿ encodings[i].enc;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
