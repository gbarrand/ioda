/*   Impression temps elapsed / Temps CPU     */
/*                           Reza  -  93/94   */

#ifndef TIMING_H_SEEN
#define TIMING_H_SEEN

#ifdef __cplusplus
extern "C" {
#endif

void InitTim(void);
void PrtTimSetDebug(int dbg);  
void PrtTim(const char * Comm);                                                        
                                                                              
#ifdef __cplusplus
}
#endif

#endif

