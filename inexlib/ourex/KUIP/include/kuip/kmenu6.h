/*
 * $Id: kmenu6.h,v 1.1.1.1 2013/07/18 11:08:49 barrand Exp $
 *
 * $Log: kmenu6.h,v $
 * Revision 1.1.1.1  2013/07/18 11:08:49  barrand
 *
 *
 * Revision 1.1.1.1  1999/05/25 13:17:44  barrand
 * KUIP
 *
 * Revision 1.1.1.1  1996/03/08 15:33:01  mclareni
 * Kuip
 *
 */
/* Interface for building lists (with Motif) */
typedef struct {
   char         *listLabel;
   char         *label;
   char         *help;
   IntFunc      *OKcallback;
   IntFunc      *user_callback;
   char         *OKcallback_data;
} ListData;

