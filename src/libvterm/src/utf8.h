/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned 정수 utf8_seqlen(long codepoint);

#만약 defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned 정수 utf8_seqlen(long codepoint)
{
  만약(codepoint < 0x0000080) 반환 1;
  만약(codepoint < 0x0000800) 반환 2;
  만약(codepoint < 0x0010000) 반환 3;
  만약(codepoint < 0x0200000) 반환 4;
  만약(codepoint < 0x4000000) 반환 5;
  반환 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
정수 fill_utf8(long codepoint, char *str);

#만약 defined(DEFINE_INLINES) || USE_INLINE
INLINE 정수 fill_utf8(long codepoint, char *str)
{
  정수 nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  정수 b = nbytes;
  동안(b > 1) {
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

  반환 nbytes;
}
#endif
/* end copy */
