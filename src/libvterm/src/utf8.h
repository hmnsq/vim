/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned цяло utf8_seqlen(long codepoint);

#ако defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned цяло utf8_seqlen(long codepoint)
{
  ако(codepoint < 0x0000080) върни 1;
  ако(codepoint < 0x0000800) върни 2;
  ако(codepoint < 0x0010000) върни 3;
  ако(codepoint < 0x0200000) върни 4;
  ако(codepoint < 0x4000000) върни 5;
  върни 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
цяло fill_utf8(long codepoint, char *str);

#ако defined(DEFINE_INLINES) || USE_INLINE
INLINE цяло fill_utf8(long codepoint, char *str)
{
  цяло nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  цяло b = nbytes;
  докато(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  switch(nbytes) {
    case 1: str[0] =        (codepoint & 0x7f); break;
    case 2: str[0] = 0xc0 | (codepoint & 0x1f); break;
    case 3: str[0] = 0xe0 | (codepoint & 0x0f); break;
    case 4: str[0] = 0xf0 | (codepoint & 0x07); break;
    case 5: str[0] = 0xf8 | (codepoint & 0x03); break;
    case 6: str[0] = 0xfc | (codepoint & 0x01); break;
  }

  върни nbytes;
}
#endif
/* end copy */
