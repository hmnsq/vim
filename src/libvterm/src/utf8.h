/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned целое utf8_seqlen(long codepoint);

#если defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned целое utf8_seqlen(long codepoint)
{
  если(codepoint < 0x0000080) вернуть 1;
  если(codepoint < 0x0000800) вернуть 2;
  если(codepoint < 0x0010000) вернуть 3;
  если(codepoint < 0x0200000) вернуть 4;
  если(codepoint < 0x4000000) вернуть 5;
  вернуть 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
целое fill_utf8(long codepoint, char *str);

#если defined(DEFINE_INLINES) || USE_INLINE
INLINE целое fill_utf8(long codepoint, char *str)
{
  целое nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  целое b = nbytes;
  пока(b > 1) {
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

  вернуть nbytes;
}
#endif
/* end copy */
