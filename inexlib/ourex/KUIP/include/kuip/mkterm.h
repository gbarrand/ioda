/*
 * $Id: mkterm.h,v 1.1.1.1 2013/07/18 11:08:49 barrand Exp $
 *
 * $Log: mkterm.h,v $
 * Revision 1.1.1.1  2013/07/18 11:08:49  barrand
 *
 *
 * Revision 1.1.1.1  1999/05/25 13:17:45  barrand
 * KUIP
 *
 * Revision 1.1.1.1  1996/03/08 15:33:01  mclareni
 * Kuip
 *
 */

#define ESCAPE          "#@"


typedef void (*KxtermActionProc)(
#ifndef NO_PROTOTYPES
    char**              /* params */,
    int                 /* num_params */
#endif
);

typedef struct _KxtermActionsRec{
    char               *string;
    KxtermActionProc    proc;
} KxtermActionsRec;

typedef KxtermActionsRec  *KxtermActionList;

extern void             kxterm_add_actions(
                                     KxtermActionList);
extern void             handle_kxterm_action(
                                     char *);
extern void             send_kxterm_cmd(
                                     char**);
extern void             send_single_kxterm_cmd(
                                     char*);

