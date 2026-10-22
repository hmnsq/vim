/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned পূর্ণসংখ্যা utf8_seqlen(long codepoint);

#যদি defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned পূর্ণসংখ্যা utf8_seqlen(long codepoint)
{
  যদি(codepoint < 0x0000080) ফেরত 1;
  যদি(codepoint < 0x0000800) ফেরত 2;
  যদি(codepoint < 0x0010000) ফেরত 3;
  যদি(codepoint < 0x0200000) ফেরত 4;
  যদি(codepoint < 0x4000000) ফেরত 5;
  ফেরত 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
পূর্ণসংখ্যা fill_utf8(long codepoint, অক্ষর *str);

#যদি defined(DEFINE_INLINES) || USE_INLINE
INLINE পূর্ণসংখ্যা fill_utf8(long codepoint, অক্ষর *str)
{
  পূর্ণসংখ্যা nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  পূর্ণসংখ্যা b = nbytes;
  যখন(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  নির্বাচন(nbytes) {
    ক্ষেত্র 1: str[0] =        (codepoint & 0x7f); বিরতি;
    ক্ষেত্র 2: str[0] = 0xc0 | (codepoint & 0x1f); বিরতি;
    ক্ষেত্র 3: str[0] = 0xe0 | (codepoint & 0x0f); বিরতি;
    ক্ষেত্র 4: str[0] = 0xf0 | (codepoint & 0x07); বিরতি;
    ক্ষেত্র 5: str[0] = 0xf8 | (codepoint & 0x03); বিরতি;
    ক্ষেত্র 6: str[0] = 0xfc | (codepoint & 0x01); বিরতি;
  }

  ফেরত nbytes;
}
#endif
/* end copy */
