#અન્તર્ભાવ "vterm_internal.h"

#define UNICODE_INVALID 0xFFFD

#જો defined(DEBUG) && DEBUG > 1
# define DEBUG_PRINT_UTF8
#endif

સંરચના UTF8DecoderData {
  // number of bytes remaining in this codepoint
  પૂર્ણાંક bytes_remaining;

  // number of bytes total in this codepoint once it's finished
  // (for detecting overlongs)
  પૂર્ણાંક bytes_total;

  પૂર્ણાંક this_cp;
};

સ્થિર શૂન્ય init_utf8(VTermEncoding *enc UNUSED, શૂન્ય *data_)
{
  સંરચના UTF8DecoderData *data = data_;

  data->bytes_remaining = 0;
  data->bytes_total     = 0;
}

સ્થિર શૂન્ય decode_utf8(VTermEncoding *enc UNUSED, શૂન્ય *data_,
                        uint32_t cp[], પૂર્ણાંક *cpi, પૂર્ણાંક cplen,
                        અચલ અક્ષર bytes[], size_t *pos, size_t bytelen)
{
  સંરચના UTF8DecoderData *data = data_;

#ifdef DEBUG_PRINT_UTF8
  printf("BEGIN UTF-8\n");
#endif

  માટે(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned અક્ષર c = bytes[*pos];

#ifdef DEBUG_PRINT_UTF8
    printf(" pos=%zd c=%02x rem=%d\n", *pos, c, data->bytes_remaining);
#endif

    જો(c < 0x20) // C0
      પાછા;

    નહીંતર જો(c >= 0x20 && c < 0x7f) {
      જો(data->bytes_remaining) {
        data->bytes_remaining = 0;
        cp[(*cpi)++] = UNICODE_INVALID;
	// VIM: avoid going over the end
	જો (*cpi >= cplen)
	  તોડો;
      }
      cp[(*cpi)++] = c;
#ifdef DEBUG_PRINT_UTF8
      printf(" UTF-8 char: U+%04x\n", c);
#endif
    }

    નહીંતર જો(c == 0x7f) // DEL
      પાછા;

    નહીંતર જો(c >= 0x80 && c < 0xc0) {
      જો(!data->bytes_remaining) {
        cp[(*cpi)++] = UNICODE_INVALID;
        ચાલુ;
      }

      data->this_cp <<= 6;
      data->this_cp |= c & 0x3f;
      data->bytes_remaining--;

      જો(!data->bytes_remaining) {
#ifdef DEBUG_PRINT_UTF8
        printf(" UTF-8 raw char U+%04x bytelen=%d ", data->this_cp, data->bytes_total);
#endif
        // Check for overlong sequences
        પસંદગી(data->bytes_total) {
        સ્થિતિ 2:
          જો(data->this_cp <  0x0080) data->this_cp = UNICODE_INVALID;
          તોડો;
        સ્થિતિ 3:
          જો(data->this_cp <  0x0800) data->this_cp = UNICODE_INVALID;
          તોડો;
        સ્થિતિ 4:
          જો(data->this_cp < 0x10000) data->this_cp = UNICODE_INVALID;
          તોડો;
        સ્થિતિ 5:
          જો(data->this_cp < 0x200000) data->this_cp = UNICODE_INVALID;
          તોડો;
        સ્થિતિ 6:
          જો(data->this_cp < 0x4000000) data->this_cp = UNICODE_INVALID;
          તોડો;
        }
        // Now look for plain invalid ones
        જો((data->this_cp >= 0xD800 && data->this_cp <= 0xDFFF) ||
           data->this_cp == 0xFFFE ||
           data->this_cp == 0xFFFF)
          data->this_cp = UNICODE_INVALID;
#ifdef DEBUG_PRINT_UTF8
        printf(" char: U+%04x\n", data->this_cp);
#endif
        cp[(*cpi)++] = data->this_cp;
      }
    }

    નહીંતર જો(c >= 0xc0 && c < 0xe0) {
      જો(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x1f;
      data->bytes_total = 2;
      data->bytes_remaining = 1;
    }

    નહીંતર જો(c >= 0xe0 && c < 0xf0) {
      જો(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x0f;
      data->bytes_total = 3;
      data->bytes_remaining = 2;
    }

    નહીંતર જો(c >= 0xf0 && c < 0xf8) {
      જો(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x07;
      data->bytes_total = 4;
      data->bytes_remaining = 3;
    }

    નહીંતર જો(c >= 0xf8 && c < 0xfc) {
      જો(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x03;
      data->bytes_total = 5;
      data->bytes_remaining = 4;
    }

    નહીંતર જો(c >= 0xfc && c < 0xfe) {
      જો(data->bytes_remaining)
        cp[(*cpi)++] = UNICODE_INVALID;

      data->this_cp = c & 0x01;
      data->bytes_total = 6;
      data->bytes_remaining = 5;
    }

    નહીંતર {
      cp[(*cpi)++] = UNICODE_INVALID;
    }
  }
}

સ્થિર VTermEncoding encoding_utf8 = {
  &init_utf8,  // init
  &decode_utf8 // decode
};

સ્થિર શૂન્ય decode_usascii(VTermEncoding *enc UNUSED, શૂન્ય *data UNUSED,
                           uint32_t cp[], પૂર્ણાંક *cpi, પૂર્ણાંક cplen,
                           અચલ અક્ષર bytes[], size_t *pos, size_t bytelen)
{
  પૂર્ણાંક is_gr = bytes[*pos] & 0x80;

  માટે(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned અક્ષર c = bytes[*pos] ^ is_gr;

    જો(c < 0x20 || c == 0x7f || c >= 0x80)
      પાછા;

    cp[(*cpi)++] = c;
  }
}

સ્થિર VTermEncoding encoding_usascii = {
  ખાલી,           // init
  &decode_usascii // decode
};

સંરચના StaticTableEncoding {
  અચલ VTermEncoding enc;
  અચલ uint32_t chars[128];
};

સ્થિર શૂન્ય decode_table(VTermEncoding *enc, શૂન્ય *data UNUSED,
                         uint32_t cp[], પૂર્ણાંક *cpi, પૂર્ણાંક cplen,
                         અચલ અક્ષર bytes[], size_t *pos, size_t bytelen)
{
  સંરચના StaticTableEncoding *table = (સંરચના StaticTableEncoding *)enc;
  પૂર્ણાંક is_gr = bytes[*pos] & 0x80;

  માટે(; *pos < bytelen && *cpi < cplen; (*pos)++) {
    unsigned અક્ષર c = bytes[*pos] ^ is_gr;

    જો(c < 0x20 || c == 0x7f || c >= 0x80)
      પાછા;

    જો(table->chars[c])
      cp[(*cpi)++] = table->chars[c];
    નહીંતર
      cp[(*cpi)++] = c;
  }
}

#અન્તર્ભાવ "encoding/DECdrawing.inc"
#અન્તર્ભાવ "encoding/uk.inc"

સ્થિર સંરચના {
  VTermEncodingType type;
  અક્ષર designation;
  VTermEncoding *enc;
}
encodings[] = {
  { ENC_UTF8,      'u', &encoding_utf8 },
  { ENC_SINGLE_94, '0', (VTermEncoding*)&encoding_DECdrawing },
  { ENC_SINGLE_94, 'A', (VTermEncoding*)&encoding_uk },
  { ENC_SINGLE_94, 'B', &encoding_usascii },
  { 0, 0, ખાલી },
};

/* This ought to be INTERNAL but isn't because it's used by unit testing */
VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, અક્ષર designation)
{
  માટે(પૂર્ણાંક i = 0; encodings[i].designation; i++)
    જો(encodings[i].type == type && encodings[i].designation == designation)
      પાછા encodings[i].enc;
  પાછા ખાલી;
}
