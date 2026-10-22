// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#ifdef _WIN32
#	include <fcntl.h>
#	include <io.h>
#endif

	पूर्णांक
main(पूर्णांक argc, अक्षर **argv)
{
	अक्षर buffer[बफ_मान];
	अक्षर *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#ifdef _WIN32
	_setmode(_fileno(मानक_निवेश), O_द्विमान_2);
	_setmode(_fileno(मानक_निकास), O_द्विमान_2);
#endif

	पर्यंत (ख_माला_लो(buffer, बफ_मान, मानक_निवेश) != रिक्त)
	{
		साठी (p = buffer; *p != 0; p++)
		{
			जर (म_भेदन(p, "charset=utf-8", 13) == 0
				|| म_भेदन(p, "charset=UTF-8", 13) == 0)
			{
				ख_माला_दो("charset=CP932", मानक_निकास);
				p += 12;
			}
			अन्यथा जर (म_भेदन(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				ख_माला_दो("# Generated from ja.po, DO NOT EDIT.", मानक_निकास);
			}
			अन्यथा जर (*(अचिन्हित अक्षर *)p == 0x81 && p[1] == '_')
			{
				अक्षर_दो('\\');
				++p;
			}
			अन्यथा
			{
				जर (*p & 0x80)
				{
					अक्षर_दो(*p++);
					जर (*p == '\\')
						अक्षर_दो(*p);
				}
				अक्षर_दो(*p);
			}
		}
	}
}

// vim:set ts=4 sts=4 sw=4 noet:
