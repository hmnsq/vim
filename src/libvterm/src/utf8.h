/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned ཧྲིལ utf8_seqlen(long codepoint);

#གལ་སྲིད defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned ཧྲིལ utf8_seqlen(long codepoint)
{
  གལ་སྲིད(codepoint < 0x0000080) ལོག 1;
  གལ་སྲིད(codepoint < 0x0000800) ལོག 2;
  གལ་སྲིད(codepoint < 0x0010000) ལོག 3;
  གལ་སྲིད(codepoint < 0x0200000) ལོག 4;
  གལ་སྲིད(codepoint < 0x4000000) ལོག 5;
  ལོག 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
ཧྲིལ fill_utf8(long codepoint, var_na *str);

#གལ་སྲིད defined(DEFINE_INLINES) || USE_INLINE
INLINE ཧྲིལ fill_utf8(long codepoint, var_na *str)
{
  ཧྲིལ nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  ཧྲིལ b = nbytes;
  སྐབས(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  switch(nbytes) {
    pak_sa 1: str[0] =        (codepoint & 0x7f); bha_nga;
    pak_sa 2: str[0] = 0xc0 | (codepoint & 0x1f); bha_nga;
    pak_sa 3: str[0] = 0xe0 | (codepoint & 0x0f); bha_nga;
    pak_sa 4: str[0] = 0xf0 | (codepoint & 0x07); bha_nga;
    pak_sa 5: str[0] = 0xf8 | (codepoint & 0x03); bha_nga;
    pak_sa 6: str[0] = 0xfc | (codepoint & 0x01); bha_nga;
  }

  ལོག nbytes;
}
#endif
/* end copy */
