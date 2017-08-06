/* $Id: spodtimes.c,v 2.1 2005/07/07 16:27:09 ksb  Exp $ */
/* $Revision: 2.1 $ */

#include "flump.h"

void tick()
{
	connection *sweep=NULL;
	time_t timenow=0;
	char *dat=NULL;
	time_t totalspod=0;
	time_t lastinv=0;
	char temp[TMP_SIZE];
	unsigned long invites=0;

	timenow=time(0);

	for (sweep=users;sweep;sweep=sweep->next) {
		if (sweep->quiet) continue;
		sweep->cspod+=timenow-sweep->last_spod;
		if (timenow-sweep->last_command<300) {
			sweep->ctruespod+=timenow-sweep->last_spod;
		}	
		sweep->last_spod=timenow;
		if (time(0)-sweep->last_sync>60) {
			sweep->last_sync=time(0);
			/* actual spod time */
			totalspod=0;
			dat=get_attribute(sweep->object,"totalspod");
			if (dat) {
				totalspod=atol(dat);
				if (totalspod<0) totalspod=0;
				FREE(dat);
			}	
			totalspod+=sweep->cspod;
			sweep->cspod=0;
			sprintf(temp,"%ld",(long)totalspod);
			set_attribute(sweep->object,"totalspod",temp);
			sprintf(temp,"%ld",(long)time(0));
			set_attribute(sweep->object,"laston",temp);
			/* true spod time */
			totalspod=0;
			dat=get_attribute(sweep->object,"truespod");
			if (dat) {
				totalspod=atol(dat);
				if (totalspod<0) totalspod=0;
				FREE(dat);
			}	
			totalspod+=sweep->ctruespod;
			sweep->ctruespod=0;
			sprintf(temp,"%ld",(long)totalspod);
			set_attribute(sweep->object,"truespod",temp);
			/* invites */
			lastinv=0;
			dat=get_attribute(sweep->object,"lastinv");
			if (dat) {
				lastinv=atol(dat);
				FREE(dat);
			}
			if (totalspod-lastinv>86400) {
				sprintf(temp,"%ld",(long)totalspod);
				set_attribute(sweep->object,"lastinv",temp);
				invites=0;
				dat=get_attribute(sweep->object,"invites");
				if (dat) {
					invites=atol(dat);
					FREE(dat);
				}	
				invites++;
				if (invites>10) invites=10;
				sprintf(temp,"%ld",invites);
				set_attribute(sweep->object,"invites",temp);
			}	
		}
	}	
}

char *version()
{
	return "Spodtimes (c) 2004 Karl Bastiman $Revision: 2.1 $";
}
