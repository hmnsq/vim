/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned số_nguyên utf8_seqlen(long codepoint);

#nếu defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned số_nguyên utf8_seqlen(long codepoint)
{
  nếu(codepoint < 0x0000080) trả_về 1;
  nếu(codepoint < 0x0000800) trả_về 2;
  nếu(codepoint < 0x0010000) trả_về 3;
  nếu(codepoint < 0x0200000) trả_về 4;
  nếu(codepoint < 0x4000000) trả_về 5;
  trả_về 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
số_nguyên fill_utf8(long codepoint, char *str);

#nếu defined(DEFINE_INLINES) || USE_INLINE
INLINE số_nguyên fill_utf8(long codepoint, char *str)
{
  số_nguyên nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  số_nguyên b = nbytes;
  trong_khi(b > 1) {
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

  trả_về nbytes;
}
#endif
/* end copy */
