/* recState.c */
/* share/src/rec $Id$ */

/* recState.c - Record Support Routines for State records */
/*
 *      Original Author: Bob Dalesio
 *      Current Author:  Marty Kraimer
 *      Date:            10-10-90 
 *
 *      Experimental Physics and Industrial Control System (EPICS)
 *
 *      Copyright 1991, the Regents of the University of California,
 *      and the University of Chicago Board of Governors.
 *
 *      This software was produced under  U.S. Government contracts:
 *      (W-7405-ENG-36) at the Los Alamos National Laboratory,
 *      and (W-31-109-ENG-38) at Argonne National Laboratory.
 *
 *      Initial development by:
 *              The Controls and Automation Group (AT-8)
 *              Ground Test Accelerator
 *              Accelerator Technology Division
 *              Los Alamos National Laboratory
 *
 *      Co-developed with
 *              The Controls and Computing Group
 *              Accelerator Systems Division
 *              Advanced Photon Source
 *              Argonne National Laboratory
 *
 * Modification Log:
 * -----------------
 * .01  10-10-90	mrk	extensible record and device support
 */

#include	<vxWorks.h>
#include	<types.h>
#include	<stdioLib.h>
#include	<lstLib.h>

#include	<dbAccess.h>
#include	<dbDefs.h>
#include	<dbFldTypes.h>
#include	<devSup.h>
#include	<errMdef.h>
#include	<link.h>
#include	<recSup.h>
#include	<stateRecord.h>

/* Create RSET - Record Support Entry Table*/
#define report NULL
#define initialize NULL
#define init_record NULL
long process();
#define special NULL
long get_value();
#define cvt_dbaddr NULL
#define get_array_info NULL
#define put_array_info NULL
#define get_units NULL
#define get_precision NULL
#define get_enum_str NULL
#define get_enum_strs NULL
#define put_enum_str NULL
#define get_graphic_double NULL
#define get_control_double NULL
#define get_alarm_double NULL

struct rset stateRSET={
	RSETNUMBER,
	report,
	initialize,
	init_record,
	process,
	special,
	get_value,
	cvt_dbaddr,
	get_array_info,
	put_array_info,
	get_units,
	get_precision,
	get_enum_str,
	get_enum_strs,
	put_enum_str,
	get_graphic_double,
	get_control_double,
	get_alarm_double };

void monitor();

static long process(paddr)
    struct dbAddr	*paddr;
{
	struct stateRecord	*pstate=(struct stateRecord *)(paddr->precord);

	pstate->udf = FALSE;
        pstate->pact=TRUE;
	tsLocalTime(&pstate->time);
	monitor(pstate);
        /* process the forward scan link record */
        if (pstate->flnk.type==DB_LINK) dbScanPassive(pstate->flnk.value.db_link.pdbAddr);
        pstate->pact=FALSE;
	return(0);
}

static long get_value(pstate,pvdes)
    struct stateRecord             *pstate;
    struct valueDes     *pvdes;
{
    pvdes->field_type = DBF_STRING;
    pvdes->no_elements=1;
    pvdes->pvalue = (caddr_t)(&pstate->val[0]);
    return(0);
}


static void monitor(pstate)
    struct stateRecord             *pstate;
{
    unsigned short  monitor_mask;
    short           stat,sevr,nsta,nsev;

    /* get previous stat and sevr  and new stat and sevr*/
    stat=pstate->stat;
    sevr=pstate->sevr;
    nsta=pstate->nsta;
    nsev=pstate->nsev;
    /*set current stat and sevr*/
    pstate->stat = nsta;
    pstate->sevr = nsev;
    pstate->nsta = 0;
    pstate->nsev = 0;

    /* Flags which events to fire on the value field */
    monitor_mask = 0;

    /* alarm condition changed this scan */
    if (stat!=nsta || sevr!=nsev) {
            /* post events for alarm condition change*/
            monitor_mask = DBE_ALARM;
            /* post stat and nsev fields */
            db_post_events(pstate,&pstate->stat,DBE_VALUE);
            db_post_events(pstate,&pstate->sevr,DBE_VALUE);
    }

    if(strncmp(pstate->oval,pstate->val,sizeof(pstate->val))) {
        db_post_events(pstate,&(pstate->val[0]),monitor_mask|DBE_VALUE);
	strncpy(pstate->oval,pstate->val,sizeof(pstate->val));
    }
    return;
}
