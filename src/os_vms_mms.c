#अन्तर्भाव	<stdio.h>
#अन्तर्भाव	<stdlib.h>
#अन्तर्भाव	<string.h>
#अन्तर्भाव	<unixio.h>
#अन्तर्भाव	"vim.h"
पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण *argv[])
{
    खाता	*fpi, *fpo;
    वर्ण	cmd[132], buf[बफ_मान], *argp, *error_file, target[132], *mms;
    पूर्णाङ्क		err = 0, err_पंक्ति = 0;

    mms = "mms";
    argc--;
    argv++;
    यावत् (argc-- > 0)
    {
	argp = *argv++;
	यदि (*argp == '-')
	{
	    विकल्प (*++argp)
	    {
		पक्ष 'm':
		    mms = ++argp;
		    भङ्ग;
		पक्ष 'e':
		    यदि (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    भङ्ग;
		स्वभाव:
		    भङ्ग;
	    }
	}
	अन्यथा
	{
	    यदि (*target)
		म_जोड़ो(target, " ");
	    म_जोड़ो(target, argp);
	}
    }
    vim_snprintf(cmd, परिमाण(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    प्रणाली(cmd);
    fpi = ख_खोलो("tmp:errors.vim_tmp", "r");
    fpo = ख_खोलो(error_file, "w");
    यावत् (ख_माला_लो(buf, बफ_मान, fpi))
    {
	यदि (!स_भेद(buf, "%CC-", 4))
	{
	    err_पंक्ति++;
	    buf[म_माप(buf)-1] = '\0';
	    err++;
	}
	अन्यथा
	{
	    यदि (err_पंक्ति)
	    {
		यदि (म_माला(buf, _("At line")))
		{
		    err_पंक्ति = 0;
		    ख_लिखो(fpo, "@");
		}
		अन्यथा
		    buf[म_माप(buf)-1] = '\0';
	    }
	}
	ख_लिखो(fpo, "%s", buf);
    }
    ख_बंद(fpi);
    ख_बंद(fpo);
    यावत् (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    निकास(err ? 44 : 1);
    प्रत्यागम(0);
}
