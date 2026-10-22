/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned ಪೂರ್ಣಾಙ್ಕ utf8_seqlen(long codepoint);

#ಯದಿ defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned ಪೂರ್ಣಾಙ್ಕ utf8_seqlen(long codepoint)
{
  ಯದಿ(codepoint < 0x0000080) ಪ್ರತ್ಯಾಗಮ 1;
  ಯದಿ(codepoint < 0x0000800) ಪ್ರತ್ಯಾಗಮ 2;
  ಯದಿ(codepoint < 0x0010000) ಪ್ರತ್ಯಾಗಮ 3;
  ಯದಿ(codepoint < 0x0200000) ಪ್ರತ್ಯಾಗಮ 4;
  ಯದಿ(codepoint < 0x4000000) ಪ್ರತ್ಯಾಗಮ 5;
  ಪ್ರತ್ಯಾಗಮ 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
ಪೂರ್ಣಾಙ್ಕ fill_utf8(long codepoint, ವರ್ಣ *str);

#ಯದಿ defined(DEFINE_INLINES) || USE_INLINE
INLINE ಪೂರ್ಣಾಙ್ಕ fill_utf8(long codepoint, ವರ್ಣ *str)
{
  ಪೂರ್ಣಾಙ್ಕ nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  ಪೂರ್ಣಾಙ್ಕ b = nbytes;
  ಯಾವತ್(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  ವಿಕಲ್ಪ(nbytes) {
    ಪಕ್ಷ 1: str[0] =        (codepoint & 0x7f); ಭಙ್ಗ;
    ಪಕ್ಷ 2: str[0] = 0xc0 | (codepoint & 0x1f); ಭಙ್ಗ;
    ಪಕ್ಷ 3: str[0] = 0xe0 | (codepoint & 0x0f); ಭಙ್ಗ;
    ಪಕ್ಷ 4: str[0] = 0xf0 | (codepoint & 0x07); ಭಙ್ಗ;
    ಪಕ್ಷ 5: str[0] = 0xf8 | (codepoint & 0x03); ಭಙ್ಗ;
    ಪಕ್ಷ 6: str[0] = 0xfc | (codepoint & 0x01); ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ nbytes;
}
#endif
/* end copy */
