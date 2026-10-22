#અન્તર્ભાવ	<stdio.h>
#અન્તર્ભાવ	<stdlib.h>
#અન્તર્ભાવ	<string.h>
#અન્તર્ભાવ	<unixio.h>
#અન્તર્ભાવ	"vim.h"
પૂર્ણાંક મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[])
{
    FILE	*fpi, *fpo;
    અક્ષર	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    પૂર્ણાંક		err = 0, err_લીટી = 0;

    mms = "mms";
    argc--;
    argv++;
    જ્યારે (argc-- > 0)
    {
	argp = *argv++;
	જો (*argp == '-')
	{
	    પસંદગી (*++argp)
	    {
		સ્થિતિ 'm':
		    mms = ++argp;
		    તોડો;
		સ્થિતિ 'e':
		    જો (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    તોડો;
		મૂળભૂત:
		    તોડો;
	    }
	}
	નહીંતર
	{
	    જો (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, કદ(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    જ્યારે (fgets(buf, BUFSIZ, fpi))
    {
	જો (!memcmp(buf, "%CC-", 4))
	{
	    err_લીટી++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	નહીંતર
	{
	    જો (err_લીટી)
	    {
		જો (strstr(buf, _("At line")))
		{
		    err_લીટી = 0;
		    fprintf(fpo, "@");
		}
		નહીંતર
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    જ્યારે (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    પાછા(0);
}
