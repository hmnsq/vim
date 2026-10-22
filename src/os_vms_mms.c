#ਅਨ੍ਤਰ੍ਭਾਵ	<stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ	<stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ	<string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ	<unixio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ	"vim.h"
ਪੂਰਨ ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
    FILE	*fpi, *fpo;
    ਅੱਖਰ	cmd[132], buf[BUFSIZ], *argp, *error_file, ਨਿਸ਼ਾਨਾ[132], *mms;
    ਪੂਰਨ		err = 0, err_ਸਤਰ = 0;

    mms = "mms";
    argc--;
    argv++;
    ਜਦੋਂ (argc-- > 0)
    {
	argp = *argv++;
	ਜੇ (*argp == '-')
	{
	    ਚੋਣ (*++argp)
	    {
		ਹਾਲਤ 'm':
		    mms = ++argp;
		    ਰੋਕੋ;
		ਹਾਲਤ 'e':
		    ਜੇ (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    ਰੋਕੋ;
		ਮੂਲ:
		    ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (*ਨਿਸ਼ਾਨਾ)
		strcat(ਨਿਸ਼ਾਨਾ, " ");
	    strcat(ਨਿਸ਼ਾਨਾ, argp);
	}
    }
    vim_snprintf(cmd, ਆਕਾਰ(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, ਨਿਸ਼ਾਨਾ);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    ਜਦੋਂ (fgets(buf, BUFSIZ, fpi))
    {
	ਜੇ (!memcmp(buf, "%CC-", 4))
	{
	    err_ਸਤਰ++;
	    buf[ਲੜੀ_ਲੰਬਾਈ(buf)-1] = '\0';
	    err++;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (err_ਸਤਰ)
	    {
		ਜੇ (ਲੜੀ_ਖੋਜ(buf, _("At line")))
		{
		    err_ਸਤਰ = 0;
		    fprintf(fpo, "@");
		}
		ਨਹੀਂਤਾਂ
		    buf[ਲੜੀ_ਲੰਬਾਈ(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    ਜਦੋਂ (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    ਬਾਹਰ_ਨਿਕਲੋ(err ? 44 : 1);
    ਵਾਪਸ(0);
}
