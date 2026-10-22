/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned पूर्णाङ्क utf8_seqlen(long codepoint);

#जइ defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned पूर्णाङ्क utf8_seqlen(long codepoint)
{
  जइ(codepoint < 0x0000080) प्रत्यागम 1;
  जइ(codepoint < 0x0000800) प्रत्यागम 2;
  जइ(codepoint < 0x0010000) प्रत्यागम 3;
  जइ(codepoint < 0x0200000) प्रत्यागम 4;
  जइ(codepoint < 0x4000000) प्रत्यागम 5;
  प्रत्यागम 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
पूर्णाङ्क fill_utf8(long codepoint, वर्ण *str);

#जइ defined(DEFINE_INLINES) || USE_INLINE
INLINE पूर्णाङ्क fill_utf8(long codepoint, वर्ण *str)
{
  पूर्णाङ्क nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  पूर्णाङ्क b = nbytes;
  जाव(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  विकल्प(nbytes) {
    पक्ष 1: str[0] =        (codepoint & 0x7f); भङ्ग;
    पक्ष 2: str[0] = 0xc0 | (codepoint & 0x1f); भङ्ग;
    पक्ष 3: str[0] = 0xe0 | (codepoint & 0x0f); भङ्ग;
    पक्ष 4: str[0] = 0xf0 | (codepoint & 0x07); भङ्ग;
    पक्ष 5: str[0] = 0xf8 | (codepoint & 0x03); भङ्ग;
    पक्ष 6: str[0] = 0xfc | (codepoint & 0x01); भङ्ग;
  }

  प्रत्यागम nbytes;
}
#endif
/* end copy */
