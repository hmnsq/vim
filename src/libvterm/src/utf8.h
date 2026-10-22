/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned 整数 utf8_seqlen(long codepoint);

#もし defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned 整数 utf8_seqlen(long codepoint)
{
  もし(codepoint < 0x0000080) 戻る 1;
  もし(codepoint < 0x0000800) 戻る 2;
  もし(codepoint < 0x0010000) 戻る 3;
  もし(codepoint < 0x0200000) 戻る 4;
  もし(codepoint < 0x4000000) 戻る 5;
  戻る 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
整数 fill_utf8(long codepoint, char *str);

#もし defined(DEFINE_INLINES) || USE_INLINE
INLINE 整数 fill_utf8(long codepoint, char *str)
{
  整数 nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  整数 b = nbytes;
  ながら(b > 1) {
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

  戻る nbytes;
}
#endif
/* end copy */
