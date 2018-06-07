/*
 * This example shows how to create a compound data type with an array member,
 * and write an array which has the compound data type to the file.
 */

#include "ourex_hdf5.h"

#define FILE          "DSwith_array_member.h5"
#define DATASETNAME   "ArrayOfStructures"
#define LENGTH        10
#define RANK          1
#define ARRAY_RANK    1
#define ARRAY_DIM     3 

int
main(void)
{

    /* First structure  and dataset*/
    typedef struct s1_t {
	int    a;
	float  b[ARRAY_DIM];
    } s1_t;
    s1_t       s1[LENGTH];

    hid_t      s1_tid;     /* File datatype identifier */
    hid_t      array_tid; /* Array datatype handle */
    hid_t      file, dataset, space; /* Handles */
    herr_t     status;
    hsize_t    dim[] = {LENGTH};   /* Dataspace dimensions */
    hsize_t    array_dim[] = {ARRAY_DIM};   /* Array dimensions */

    int        i, j;

    /*
     * Initialize the data
     */
    for (i = 0; i< LENGTH; i++) {
        s1[i].a = i;
        for (j = 0; j < ARRAY_DIM; j++) {
             s1[i].b[j] = (float)(i+j);
        }
    }

    /*
     * Create the data space.
     */
    space = H5Screate_simple(RANK, dim, NULL);

    /*
     * Create the file.
     */
    file = H5Fcreate(FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Create the array data type. 
     */
     array_tid = H5Tarray_create(H5T_NATIVE_FLOAT, ARRAY_RANK, array_dim, NULL);

    /*
     * Create the memory data type. 
     */
    s1_tid = H5Tcreate (H5T_COMPOUND, sizeof(s1_t));
    H5Tinsert(s1_tid, "a_name", HOFFSET(s1_t, a), H5T_NATIVE_INT);
    H5Tinsert(s1_tid, "b_name", HOFFSET(s1_t, b), array_tid);

    /* 
     * Create the dataset.
     */
    dataset = H5Dcreate(file, DATASETNAME, s1_tid, space, H5P_DEFAULT);

    /*
     * Wtite data to the dataset; 
     */
    status = H5Dwrite(dataset, s1_tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, s1);

    /*
     * Release resources
     */
    H5Tclose(s1_tid);
    H5Tclose(array_tid);
    H5Sclose(space);
    H5Dclose(dataset);
    H5Fclose(file);
 

    return 0;
}
