/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned പൂർണ്ണസംഖ്യ utf8_seqlen(long codepoint);

#എങ്കിൽ defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned പൂർണ്ണസംഖ്യ utf8_seqlen(long codepoint)
{
  എങ്കിൽ(codepoint < 0x0000080) തിരികെ 1;
  എങ്കിൽ(codepoint < 0x0000800) തിരികെ 2;
  എങ്കിൽ(codepoint < 0x0010000) തിരികെ 3;
  എങ്കിൽ(codepoint < 0x0200000) തിരികെ 4;
  എങ്കിൽ(codepoint < 0x4000000) തിരികെ 5;
  തിരികെ 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
പൂർണ്ണസംഖ്യ fill_utf8(long codepoint, അക്ഷരം *str);

#എങ്കിൽ defined(DEFINE_INLINES) || USE_INLINE
INLINE പൂർണ്ണസംഖ്യ fill_utf8(long codepoint, അക്ഷരം *str)
{
  പൂർണ്ണസംഖ്യ nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  പൂർണ്ണസംഖ്യ b = nbytes;
  ആകുമ്പോൾ(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  തിരഞ്ഞെടുപ്പ്(nbytes) {
    അവസ്ഥ 1: str[0] =        (codepoint & 0x7f); നിർത്തുക;
    അവസ്ഥ 2: str[0] = 0xc0 | (codepoint & 0x1f); നിർത്തുക;
    അവസ്ഥ 3: str[0] = 0xe0 | (codepoint & 0x0f); നിർത്തുക;
    അവസ്ഥ 4: str[0] = 0xf0 | (codepoint & 0x07); നിർത്തുക;
    അവസ്ഥ 5: str[0] = 0xf8 | (codepoint & 0x03); നിർത്തുക;
    അവസ്ഥ 6: str[0] = 0xfc | (codepoint & 0x01); നിർത്തുക;
  }

  തിരികെ nbytes;
}
#endif
/* end copy */
