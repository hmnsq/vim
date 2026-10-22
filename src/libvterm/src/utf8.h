/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned цео utf8_seqlen(long codepoint);

#ако defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned цео utf8_seqlen(long codepoint)
{
  ако(codepoint < 0x0000080) врати 1;
  ако(codepoint < 0x0000800) врати 2;
  ако(codepoint < 0x0010000) врати 3;
  ако(codepoint < 0x0200000) врати 4;
  ако(codepoint < 0x4000000) врати 5;
  врати 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
цео fill_utf8(long codepoint, char *str);

#ако defined(DEFINE_INLINES) || USE_INLINE
INLINE цео fill_utf8(long codepoint, char *str)
{
  цео nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  цео b = nbytes;
  док(b > 1) {
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

  врати nbytes;
}
#endif
/* end copy */
