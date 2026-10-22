#ଅନ୍ତର୍ଭାଵ	<stdio.h>
#ଅନ୍ତର୍ଭାଵ	<stdlib.h>
#ଅନ୍ତର୍ଭାଵ	<string.h>
#ଅନ୍ତର୍ଭାଵ	<unixio.h>
#ଅନ୍ତର୍ଭାଵ	"vim.h"
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[])
{
    FILE	*fpi, *fpo;
    ଅକ୍ଷର	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err = 0, err_ଧାଡ଼ି = 0;

    mms = "mms";
    argc--;
    argv++;
    ଯେତେବେଳେ (argc-- > 0)
    {
	argp = *argv++;
	ଯଦି (*argp == '-')
	{
	    ବାଛ (*++argp)
	    {
		ସ୍ଥିତି 'm':
		    mms = ++argp;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି 'e':
		    ଯଦି (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, ଆକାର(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    ଯେତେବେଳେ (fgets(buf, BUFSIZ, fpi))
    {
	ଯଦି (!memcmp(buf, "%CC-", 4))
	{
	    err_ଧାଡ଼ି++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (err_ଧାଡ଼ି)
	    {
		ଯଦି (strstr(buf, _("At line")))
		{
		    err_ଧାଡ଼ି = 0;
		    fprintf(fpo, "@");
		}
		ଅନ୍ୟଥା
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    ଯେତେବେଳେ (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    ଫେରନ୍ତୁ(0);
}
