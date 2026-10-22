#അന്തര്ഭാവ	<stdio.h>
#അന്തര്ഭാവ	<stdlib.h>
#അന്തര്ഭാവ	<string.h>
#അന്തര്ഭാവ	<unixio.h>
#അന്തര്ഭാവ	"vim.h"
പൂർണ്ണസംഖ്യ പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
{
    FILE	*fpi, *fpo;
    അക്ഷരം	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    പൂർണ്ണസംഖ്യ		err = 0, err_വരി = 0;

    mms = "mms";
    argc--;
    argv++;
    ആകുമ്പോൾ (argc-- > 0)
    {
	argp = *argv++;
	എങ്കിൽ (*argp == '-')
	{
	    തിരഞ്ഞെടുപ്പ് (*++argp)
	    {
		അവസ്ഥ 'm':
		    mms = ++argp;
		    നിർത്തുക;
		അവസ്ഥ 'e':
		    എങ്കിൽ (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		    നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, വലുപ്പം(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    ആകുമ്പോൾ (fgets(buf, BUFSIZ, fpi))
    {
	എങ്കിൽ (!memcmp(buf, "%CC-", 4))
	{
	    err_വരി++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (err_വരി)
	    {
		എങ്കിൽ (strstr(buf, _("At line")))
		{
		    err_വരി = 0;
		    fprintf(fpo, "@");
		}
		അല്ലെങ്കിൽ
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    ആകുമ്പോൾ (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    തിരികെ(0);
}
