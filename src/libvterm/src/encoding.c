#అన్తర్భావ "vterm_internal.h"

#నిర్వచించు UNICODE_INVALID 0xFFFD

#ఒకవేళ defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#అయితే_ముగింపు

నిర్మాణం UTF8_Decoder_దత్తాంశం {
  // number of bytes remaining in this codepoint
  పూర్ణాంకం బైట్లు_మిగిలిన;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  పూర్ణాంకం బైట్లు_మొత్తం;

  పూర్ణాంకం this_cp;
};

స్థిర శూన్య init_utf8(VTermEncoding *enc వాడని, శూన్య *దత్తాంశం_4)
{
  నిర్మాణం UTF8_Decoder_దత్తాంశం *దత్తాంశం = దత్తాంశం_4;

  దత్తాంశం->బైట్లు_మిగిలిన = 0;
  దత్తాంశం->బైట్లు_మొత్తం     = 0;
}

స్థిర శూన్య decode_utf8(VTermEncoding *enc వాడని, శూన్య *దత్తాంశం_4,
                        uint32_t cp[], పూర్ణాంకం *cpi, పూర్ణాంకం cplen,
                        స్థిరరాశి అక్షరం బైట్లు[], size_t *pos, size_t bytelen)
{
  నిర్మాణం UTF8_Decoder_దత్తాంశం *దత్తాంశం = దత్తాంశం_4;

#నిర్వచితమైతే DEBUG_PRINT_UTF8
  రాయి("BEGIN UTF-8\n");
#అయితే_ముగింపు

  కొరకు(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    గుర్తులేని అక్షరం c = బైట్లు[*pos];

#నిర్వచితమైతే DEBUG_PRINT_UTF8
    రాయి(" pos=%zd c=%02x rem=%d\n", *pos, c, దత్తాంశం->బైట్లు_మిగిలిన);
#అయితే_ముగింపు

    ఒకవేళ(c < 0x20) // C0
      తిరిగి;

    లేకపోతే ఒకవేళ(c >= 0x20 && c < 0x7f) {
      ఒకవేళ(దత్తాంశం->బైట్లు_మిగిలిన) {
        దత్తాంశం->బైట్లు_మిగిలిన = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	ఒకవేళ (*cpi >= cplen)
	  ఆపు;
      }
      cp[(*cpi)++] = c;
#నిర్వచితమైతే DEBUG_PRINT_UTF8
      రాయి(" UTF-8 char: U+%04x\n", c);
#అయితే_ముగింపు
    }

    లేకపోతే ఒకవేళ(c == 0x7f) // DEL
      తిరిగి;

    లేకపోతే ఒకవేళ(c >= 0x80 && c < 0xc0) {
      ఒకవేళ(!దత్తాంశం->బైట్లు_మిగిలిన) {
        cp[(*cpi)++] = UNICODE_INVALID;
        కొనసాగు;
      }

      దత్తాంశం->this_cp <<= 6;
      దత్తాంశం->this_cp |= c & 0x3f;
      దత్తాంశం->బైట్లు_మిగిలిన--;

      ఒకవేళ(!దత్తాంశం->బైట్లు_మిగిలిన) {
#నిర్వచితమైతే DEBUG_PRINT_UTF8
        రాయి(" UTF-8 raw char U+%04x bytelen=%d ", దత్తాంశం->this_cp, దత్తాంశం->బైట్లు_మొత్తం);
#అయితే_ముగింపు
        // Check for overlong sequences
        ఎంపిక(దత్తాంశం->బైట్లు_మొత్తం) {
        స్థితి 2:
          ఒకవేళ(దత్తాంశం->this_cp <  0x0080) దత్తాంశం->this_cp = UNICODE_INVALID;
          ఆపు;
        స్థితి 3:
          ఒకవేళ(దత్తాంశం->this_cp <  0x0800) దత్తాంశం->this_cp = UNICODE_INVALID;
          ఆపు;
        స్థితి 4:
          ఒకవేళ(దత్తాంశం->this_cp < 0x10000) దత్తాంశం->this_cp = UNICODE_INVALID;
          ఆపు;
        స్థితి 5:
          ఒకవేళ(దత్తాంశం->this_cp < 0x200000) దత్తాంశం->this_cp = UNICODE_INVALID;
          ఆపు;
        స్థితి 6:
          ఒకవేళ(దత్తాంశం->this_cp < 0x4000000) దత్తాంశం->this_cp = UNICODE_INVALID;
          ఆపు;
        }
        // Now look for plain invalid ones
        ఒకవేళ((దత్తాంశం->this_cp >= 0xD800 && దత్తాంశం->this_cp <= 0xDFFF) ||
           దత్తాంశం->this_cp == 0xFFFE ||
           దత్తాంశం->this_cp == 0xFFFF)
          దత్తాంశం->this_cp = UNICODE_INVALID;
#నిర్వచితమైతే DEBUG_PRINT_UTF8
        రాయి(" char: U+%04x\n", దత్తాంశం->this_cp);
#అయితే_ముగింపు
        cp[(*cpi)++] = దత్తాంశం->this_cp;
      }
    }

    లేకపోతే ఒకవేళ(c >= 0xc0 && c < 0xe0) {
      ఒకవేళ(దత్తాంశం->బైట్లు_మిగిలిన)
        cp[(*cpi)++] = UNICODE_INVALID;

      దత్తాంశం->this_cp = c & 0x1f;
      దత్తాంశం->బైట్లు_మొత్తం = 2;
      దత్తాంశం->బైట్లు_మిగిలిన = 1;
    }

    లేకపోతే ఒకవేళ(c >= 0xe0 && c < 0xf0) {
      ఒకవేళ(దత్తాంశం->బైట్లు_మిగిలిన)
        cp[(*cpi)++] = UNICODE_INVALID;

      దత్తాంశం->this_cp = c & 0x0f;
      దత్తాంశం->బైట్లు_మొత్తం = 3;
      దత్తాంశం->బైట్లు_మిగిలిన = 2;
    }

    లేకపోతే ఒకవేళ(c >= 0xf0 && c < 0xf8) {
      ఒకవేళ(దత్తాంశం->బైట్లు_మిగిలిన)
        cp[(*cpi)++] = UNICODE_INVALID;

      దత్తాంశం->this_cp = c & 0x07;
      దత్తాంశం->బైట్లు_మొత్తం = 4;
      దత్తాంశం->బైట్లు_మిగిలిన = 3;
    }

    లేకపోతే ఒకవేళ(c >= 0xf8 && c < 0xfc) {
      ఒకవేళ(దత్తాంశం->బైట్లు_మిగిలిన)
        cp[(*cpi)++] = UNICODE_INVALID;

      దత్తాంశం->this_cp = c & 0x03;
      దత్తాంశం->బైట్లు_మొత్తం = 5;
      దత్తాంశం->బైట్లు_మిగిలిన = 4;
    }

    లేకపోతే ఒకవేళ(c >= 0xfc && c < 0xfe) {
      ఒకవేళ(దత్తాంశం->బైట్లు_మిగిలిన)
        cp[(*cpi)++] = UNICODE_INVALID;

      దత్తాంశం->this_cp = c & 0x01;
      దత్తాంశం->బైట్లు_మొత్తం = 6;
      దత్తాంశం->బైట్లు_మిగిలిన = 5;
    }

    లేకపోతే {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

స్థిర VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

స్థిర శూన్య decode_usascii(VTermEncoding *enc వాడని, శూన్య *దత్తాంశం వాడని,
                           uint32_t cp[], పూర్ణాంకం *cpi, పూర్ణాంకం cplen,
                           స్థిరరాశి అక్షరం బైట్లు[], size_t *pos, size_t bytelen)
{
  పూర్ణాంకం is_gr = బైట్లు[*pos] & 0x80;

  కొరకు(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    గుర్తులేని అక్షరం c = బైట్లు[*pos] ^ is_gr;

    ఒకవేళ(c < 0x20 || c == 0x7f || c >= 0x80)
      తిరిగి;

    cp[(*cpi)++] = c;
  }
}

స్థిర VTermEncoding encoding_usascii = {
  ఖాళీ,           // init
  &decode_usascii // decode
};

నిర్మాణం Static_పట్టిక_Encoding {
  స్థిరరాశి VTermEncoding enc;
  స్థిరరాశి uint32_t chars[128];
};

స్థిర శూన్య decode_పట్టిక(VTermEncoding *enc, శూన్య *దత్తాంశం వాడని,
                         uint32_t cp[], పూర్ణాంకం *cpi, పూర్ణాంకం cplen,
                         స్థిరరాశి అక్షరం బైట్లు[], size_t *pos, size_t bytelen)
{
  నిర్మాణం Static_పట్టిక_Encoding *పట్టిక = (నిర్మాణం Static_పట్టిక_Encoding *)enc;
  పూర్ణాంకం is_gr = బైట్లు[*pos] & 0x80;

  కొరకు(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    గుర్తులేని అక్షరం c = బైట్లు[*pos] ^ is_gr;

    ఒకవేళ(c < 0x20 || c == 0x7f || c >= 0x80)
      తిరిగి;

    ఒకవేళ(పట్టిక->chars[c])
      cp[(*cpi)++] = పట్టిక->chars[c];
    లేకపోతే
      cp[(*cpi)++] = c;
  }
}

#అన్తర్భావ "encoding/DECdrawing.inc"
#అన్తర్భావ "encoding/uk.inc"

స్థిర నిర్మాణం {
  VTermEncodingType type;
  అక్షరం designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, ఖాళీ },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, అక్షరం designation)
{
  కొరకు(పూర్ణాంకం i = 0; encodings[i].designation; i++)
    ఒకవేళ(encodings[i].type == type && encodings[i].designation == designation)
      తిరిగి encodings[i].enc;
  తిరిగి ఖాళీ;
}
