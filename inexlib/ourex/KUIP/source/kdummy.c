#include <kuip/kout.h>

#include <kuip/kuip.h>
#include "kuip/kfor.h"
#include <kuip/klink.h>

typedef struct {
  int DATA[100];
} KQuest;

KQuest quest_ = {0};

void kiinit_(){}
int kiargc_(){return 0;}

void mhi_open_(){kprintf("Mhi_open : dummy\n");}
void mhi_close_(){kprintf("Mhi_close : dummy\n");}

int kxunit_(){kprintf("kxunit : dummy\n");return -1;}
int kxmdata(){kprintf("kxmdata : dummy\n");return -1;}

#ifdef _WIN32
int pause_() {return getchar();} /*GB not found on NT.*/
#endif

int kxvcreate(){
  if( kjmpaddr.vector_create_C != NULL ) {
    return (*kjmpaddr.vector_create_C)(kjmpaddr.vector_create_C_data);
  } else {
    kprintf("kxvcreate : dummy\n");
    return -1;
  }
}
int kxdelv_(){
  if( kjmpaddr.vector_delete_C != NULL ) {
    return (*kjmpaddr.vector_delete_C)(kjmpaddr.vector_delete_C_data);
  } else {
    kprintf("kxdelv : dummy\n");
    return -1;
  }
}
int kxlisv_(){
  if( kjmpaddr.vector_list_C != NULL ) {
    return (*kjmpaddr.vector_list_C)(kjmpaddr.vector_list_C_data);
  } else {
    kprintf("kxlisv : dummy\n");
    return -1;
  }
}
int kxcopv_(){
  if( kjmpaddr.vector_copy_C != NULL ) {
    return (*kjmpaddr.vector_copy_C)(kjmpaddr.vector_copy_C_data);
  } else {
    kprintf("kxcopv : dummy\n");
    return -1;
  }
}
int kxvinput(){
  if( kjmpaddr.vector_input_C != NULL ) {
    return (*kjmpaddr.vector_input_C)(kjmpaddr.vector_input_C_data);
  } else {
    kprintf("kxvinput : dummy\n");
    return -1;
  }
}
int kxvprint(){
  if( kjmpaddr.vector_print_C != NULL ) {
    return (*kjmpaddr.vector_print_C)(kjmpaddr.vector_print_C_data);
  } else {
    kprintf("kxvprint : dummy\n");
    return -1;
  }
}
int kxreav_(){
  if( kjmpaddr.vector_read_C != NULL ) {
    return (*kjmpaddr.vector_read_C)(kjmpaddr.vector_read_C_data);
  } else {
    kprintf("kxreav : dummy\n");
    return -1;
  }
}
int kxwriv_(){
  if( kjmpaddr.vector_write_C != NULL ) {
    return (*kjmpaddr.vector_write_C)(kjmpaddr.vector_write_C_data);
  } else {
    kprintf("kxwriv : dummy\n");
    return -1;
  }
}
int kxvope_(){
  if( kjmpaddr.vector_operation_C != NULL ) {
    return (*kjmpaddr.vector_operation_C)(kjmpaddr.vector_operation_C_data);
  } else {
    kprintf("kxvope : dummy\n");
    return -1;
  }
}
