#अन्तर्भाव	<stdio.h>
#अन्तर्भाव	<stdlib.h>
#अन्तर्भाव	<string.h>
#अन्तर्भाव	<unixio.h>
#अन्तर्भाव	"vim.h"
पूर्णांक main(पूर्णांक argc, अक्षर *argv[])
{
    खाता	*fpi, *fpo;
    अक्षर	cmd[132], buf[बफ_मान], *argp, *error_file, target[132], *mms;
    पूर्णांक		err = 0, err_ओळ = 0;

    mms = "mms";
    argc--;
    argv++;
    पर्यंत (argc-- > 0)
    {
	argp = *argv++;
	जर (*argp == '-')
	{
	    निवड (*++argp)
	    {
		स्थिती 'm':
		    mms = ++argp;
		    थांबा;
		स्थिती 'e':
		    जर (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    थांबा;
		पूर्वनिर्धारित:
		    थांबा;
	    }
	}
	अन्यथा
	{
	    जर (*target)
		म_जोड़ो(target, " ");
	    म_जोड़ो(target, argp);
	}
    }
    vim_snprintf(cmd, आकार(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    प्रणाली(cmd);
    fpi = ख_खोलो("tmp:errors.vim_tmp", "r");
    fpo = ख_खोलो(error_file, "w");
    पर्यंत (ख_माला_लो(buf, बफ_मान, fpi))
    {
	जर (!स_भेद(buf, "%CC-", 4))
	{
	    err_ओळ++;
	    buf[म_माप(buf)-1] = '\0';
	    err++;
	}
	अन्यथा
	{
	    जर (err_ओळ)
	    {
		जर (म_माला(buf, _("At line")))
		{
		    err_ओळ = 0;
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
    पर्यंत (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    निकास(err ? 44 : 1);
    परत(0);
}
