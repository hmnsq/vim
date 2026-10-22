/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ utf8_seqlen(ਲੰਮਾ codepoint);

#ਜੇ defined(DEFINE_INLINES) || USE_INLINE
INLINE ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ utf8_seqlen(ਲੰਮਾ codepoint)
{
  ਜੇ(codepoint < 0x0000080) ਵਾਪਸ 1;
  ਜੇ(codepoint < 0x0000800) ਵਾਪਸ 2;
  ਜੇ(codepoint < 0x0010000) ਵਾਪਸ 3;
  ਜੇ(codepoint < 0x0200000) ਵਾਪਸ 4;
  ਜੇ(codepoint < 0x4000000) ਵਾਪਸ 5;
  ਵਾਪਸ 6;
}
#ਜੇ_ਅੰਤ

/* Does NOT NUL-terminate the buffer */
ਪੂਰਨ fill_utf8(ਲੰਮਾ codepoint, ਅੱਖਰ *str);

#ਜੇ defined(DEFINE_INLINES) || USE_INLINE
INLINE ਪੂਰਨ fill_utf8(ਲੰਮਾ codepoint, ਅੱਖਰ *str)
{
  ਪੂਰਨ nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  ਪੂਰਨ b = nbytes;
  ਜਦੋਂ(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  ਚੋਣ(nbytes) {
    ਹਾਲਤ 1: str[0] =        (codepoint & 0x7f); ਰੋਕੋ;
    ਹਾਲਤ 2: str[0] = 0xc0 | (codepoint & 0x1f); ਰੋਕੋ;
    ਹਾਲਤ 3: str[0] = 0xe0 | (codepoint & 0x0f); ਰੋਕੋ;
    ਹਾਲਤ 4: str[0] = 0xf0 | (codepoint & 0x07); ਰੋਕੋ;
    ਹਾਲਤ 5: str[0] = 0xf8 | (codepoint & 0x03); ਰੋਕੋ;
    ਹਾਲਤ 6: str[0] = 0xfc | (codepoint & 0x01); ਰੋਕੋ;
  }

  ਵਾਪਸ nbytes;
}
#ਜੇ_ਅੰਤ
/* end copy */
