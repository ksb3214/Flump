/* $Id: quotes.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include "flump.h"

extern pthread_mutex_t db_mutex;

void quotes(connection *u, char *arg)
{
	int found = 0;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row;
	int ernum, id;
	char temp[TMP_SIZE];
	char *name = NULL;

	if(arg) {
		id = atoi(arg);
		sprintf(temp,"SELECT id, quote, posted, user FROM quotes WHERE id = '%d'", id);
	} else
		sprintf(temp,"SELECT id, quote, posted, user FROM quotes ORDER BY posted DESC LIMIT 5 OFFSET 0");

	pthread_mutex_lock(&db_mutex);
	ernum = mysql_query(global.database, temp);
	if(ernum)
	{
		debug("Database select failed:");
		debug((char *)mysql_error(global.database));
		debug(temp);
		pthread_mutex_unlock(&db_mutex);
		swrite(NULL, u, "[error] Database select failed.\n");
		return;
	}

	result = mysql_store_result(global.database);
	while((row = mysql_fetch_row(result)))
	{
		found++;
		if(found == 1)
		{
			format_bar(u, "Quotes", temp);
			swrite(NULL, u, temp);
		}
		name = db_get_obj_name(atol(row[3]));
		sprintf(temp, "%s (%s^A#%s^N)\n", row[1], name ? name : "unknown", row[0]);
		swrite(NULL, u, temp);
		if(name)
			FREE(name);
	}
	MYSQL_FREE(result);
	pthread_mutex_unlock(&db_mutex);

	if(!found)
		if(arg)
			sprintf(temp, "Quote #%s not found.\n", arg);
		else
			sprintf(temp, "There are currently no quotes.\n");
	else
		format_bar(u, NULL, temp);
	swrite(NULL, u, temp);
}

int init()
{
	add_command("quotes", &quotes, NULL, NULL);
	return 1;
}

int uninit()
{
	remove_command("quotes", NULL);
	return 1;
}

char *version()
{
	return "Quotes (c) 2004 Peter Nelson $Revision: 2.0 $";
}
