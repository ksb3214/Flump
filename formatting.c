/* $Id: formatting.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include <flump.h>

void format_bar(connection *u, char *info, char *buffer)
{
	char *bar="-+";
	char temp[TMP_SIZE];
	int count=0;
	
/*	bar = config_get("format_bar");
*/
	temp[0]='\0';
	for (count=0;count<(int)((u?u->twidth-1:78)/2);count++) {
		strcat(temp, bar);
	}

	if (info) {
		if (strlen(info)<(strlen(temp)-6)) {
			strncpy(temp+3,info,strlen(info));
			temp[2]=' ';
			temp[strlen(info)+3]=' ';
		}	
	}	
	sprintf(buffer,"%s\n",temp);
}
