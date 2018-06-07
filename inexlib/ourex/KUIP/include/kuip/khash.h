/*
 * $Id: khash.h,v 1.1.1.1 2013/07/18 11:08:49 barrand Exp $
 *
 * $Log: khash.h,v $
 * Revision 1.1.1.1  2013/07/18 11:08:49  barrand
 *
 *
 * Revision 1.2  2003/11/04 09:54:45  barrand
 * *** empty log message ***
 *
 * Revision 1.1.1.1  1999/05/25 13:17:43  barrand
 * KUIP
 *
 * Revision 1.1.1.1  1996/03/08 15:33:00  mclareni
 * Kuip
 *
 */
/* khash.h: hash table management */

typedef struct _HashArray {
  char       *name;             /* symbol name */
  void       *value;            /* symbol value */
  int         tag;
} HashArray;

typedef struct _HashEntry {
  struct _HashEntry *next;      /* link to next entry */
  char       *name;             /* symbol name */
  void       *value;            /* symbol value */
  int         tag;
} HashEntry;

typedef struct {
  int         size;             /* table size should be a prime number */
  HashEntry **entries;          /* pointer to array of size entries */
  int        nentries;          /* number of entries */
  int         is_string;        /* flag if strdup/free(value) should be used */
  int         tag_only;         /* flag if only tag field is used */
  int         case_convert;
  int       (*strcmp)();        /* strcmp/strcasecmp */
  int       (*hash_cmp)();      /* hash_cmp/hash_casecmp */
} HashTable;

/*GB : Darwin : hash_ -> khash. has_create exist in /usr/include/strhash.h */
extern HashArray* khash_array(   HashTable* );
extern void       khash_clear(   HashTable* );
extern void       khash_config(  HashTable*, const char* );
extern HashTable* khash_create(  int );
extern void       khash_destroy( HashTable* );
extern int        khash_entries( HashTable* );
extern void       khash_insert(  HashTable*, const char*, const void*, int );
extern void*      khash_lookup(  HashTable*, const char*, int* );
extern void*      khash_remove(  HashTable*, const char* );


