/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned પૂર્ણાંક utf8_seqlen(long codepoint);

#જો defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned પૂર્ણાંક utf8_seqlen(long codepoint)
{
  જો(codepoint < 0x0000080) પાછા 1;
  જો(codepoint < 0x0000800) પાછા 2;
  જો(codepoint < 0x0010000) પાછા 3;
  જો(codepoint < 0x0200000) પાછા 4;
  જો(codepoint < 0x4000000) પાછા 5;
  પાછા 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
પૂર્ણાંક fill_utf8(long codepoint, અક્ષર *str);

#જો defined(DEFINE_INLINES) || USE_INLINE
INLINE પૂર્ણાંક fill_utf8(long codepoint, અક્ષર *str)
{
  પૂર્ણાંક nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  પૂર્ણાંક b = nbytes;
  જ્યારે(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  પસંદગી(nbytes) {
    સ્થિતિ 1: str[0] =        (codepoint & 0x7f); તોડો;
    સ્થિતિ 2: str[0] = 0xc0 | (codepoint & 0x1f); તોડો;
    સ્થિતિ 3: str[0] = 0xe0 | (codepoint & 0x0f); તોડો;
    સ્થિતિ 4: str[0] = 0xf0 | (codepoint & 0x07); તોડો;
    સ્થિતિ 5: str[0] = 0xf8 | (codepoint & 0x03); તોડો;
    સ્થિતિ 6: str[0] = 0xfc | (codepoint & 0x01); તોડો;
  }

  પાછા nbytes;
}
#endif
/* end copy */
