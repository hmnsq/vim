/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ utf8_seqlen(ದೀರ್ಘ codepoint);

#ಆದರೆ defined(DEFINE_INLINES) || USE_INLINE
INLINE ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ utf8_seqlen(ದೀರ್ಘ codepoint)
{
  ಆದರೆ(codepoint < 0x0000080) ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ(codepoint < 0x0000800) ಹಿಂತಿರುಗಿ 2;
  ಆದರೆ(codepoint < 0x0010000) ಹಿಂತಿರುಗಿ 3;
  ಆದರೆ(codepoint < 0x0200000) ಹಿಂತಿರುಗಿ 4;
  ಆದರೆ(codepoint < 0x4000000) ಹಿಂತಿರುಗಿ 5;
  ಹಿಂತಿರುಗಿ 6;
}
#ಆದರೆ_ಅಂತ್ಯ

/* Does NOT NUL-terminate the buffer */
ಪೂರ್ಣಾಂಕ fill_utf8(ದೀರ್ಘ codepoint, ಅಕ್ಷರ *str);

#ಆದರೆ defined(DEFINE_INLINES) || USE_INLINE
INLINE ಪೂರ್ಣಾಂಕ fill_utf8(ದೀರ್ಘ codepoint, ಅಕ್ಷರ *str)
{
  ಪೂರ್ಣಾಂಕ nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  ಪೂರ್ಣಾಂಕ b = nbytes;
  ಆಗ(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  ಆಯ್ಕೆ(nbytes) {
    ಸ್ಥಿತಿ 1: str[0] =        (codepoint & 0x7f); ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 2: str[0] = 0xc0 | (codepoint & 0x1f); ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 3: str[0] = 0xe0 | (codepoint & 0x0f); ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 4: str[0] = 0xf0 | (codepoint & 0x07); ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 5: str[0] = 0xf8 | (codepoint & 0x03); ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 6: str[0] = 0xfc | (codepoint & 0x01); ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ nbytes;
}
#ಆದರೆ_ಅಂತ್ಯ
/* end copy */
