/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned ціле utf8_seqlen(long codepoint);

#якщо defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned ціле utf8_seqlen(long codepoint)
{
  якщо(codepoint < 0x0000080) повернути 1;
  якщо(codepoint < 0x0000800) повернути 2;
  якщо(codepoint < 0x0010000) повернути 3;
  якщо(codepoint < 0x0200000) повернути 4;
  якщо(codepoint < 0x4000000) повернути 5;
  повернути 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
ціле fill_utf8(long codepoint, char *str);

#якщо defined(DEFINE_INLINES) || USE_INLINE
INLINE ціле fill_utf8(long codepoint, char *str)
{
  ціле nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  ціле b = nbytes;
  поки(b > 1) {
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

  повернути nbytes;
}
#endif
/* end copy */
