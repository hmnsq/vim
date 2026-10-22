#அந்தர்பாவ	<stdio.h>
#அந்தர்பாவ	<stdlib.h>
#அந்தர்பாவ	<string.h>
#அந்தர்பாவ	<unixio.h>
#அந்தர்பாவ	"vim.h"
முழு முதன்மை(முழு argc, எழுத்து *argv[])
{
    FILE	*fpi, *fpo;
    எழுத்து	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    முழு		err = 0, err_வரி = 0;

    mms = "mms";
    argc--;
    argv++;
    போது (argc-- > 0)
    {
	argp = *argv++;
	எனில் (*argp == '-')
	{
	    தேர்வு (*++argp)
	    {
		நிலை 'm':
		    mms = ++argp;
		    நிறுத்து;
		நிலை 'e':
		    எனில் (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    நிறுத்து;
		இயல்பு:
		    நிறுத்து;
	    }
	}
	இல்லையெனில்
	{
	    எனில் (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, அளவு(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    போது (fgets(buf, BUFSIZ, fpi))
    {
	எனில் (!memcmp(buf, "%CC-", 4))
	{
	    err_வரி++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	இல்லையெனில்
	{
	    எனில் (err_வரி)
	    {
		எனில் (strstr(buf, _("At line")))
		{
		    err_வரி = 0;
		    fprintf(fpo, "@");
		}
		இல்லையெனில்
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    போது (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    திரும்பு(0);
}
