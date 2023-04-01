#include <stdio.h>
#include <stdint.h>

/* version */
#define BF_RELEASE_VERSION "1"
#define BF_MAJOR_VERSION "1"
#define BF_MINOR_VERSION "0"

#define BF_VERSION BF_RELEASE_VERSION "." BF_MAJOR_VERSION "." BF_MINOR_VERSION
#define BF_VERSION_NAME "brainfreak " BF_VERSION

/* constants */
#define BF_MAX_BYTES 8          // might be dumb
#define BF_MIN_BYTES 1          // same here
#define BF_DEFAULT_BYTES 1      // according to bf standard (8 bits)

#define BF_MAX_SIZE SIZE_MAX    // size_t restriction
#define BF_MIN_SIZE 1           // maybe not.
#define BF_DEFAULT_SIZE 30000   // according to bf standard

/* syntax */
#define BF_PTR_INCR '>'
#define BF_PTR_DECR '<'
#define BF_CELL_INCR '+'
#define BF_CELL_DECR '-'
#define BF_LOOP_OPEN '['
#define BF_LOOP_CLOSE ']'
#define BF_IO_OUT '.'
#define BF_IO_IN ','


/* intrernal interpreter struct */
typedef struct {
    size_t size;                // size of the stack
    size_t bytes;               // size of stack cells in bytes (1 byte = 8 bits)

    void* stack;                // stack
    unsigned long ptr;          // stack pointer position

                                // current state of the interpreter
    char state;                 // i=idle c=interpreting
    unsigned int errn;          // error code if something break
} BF_data;

/* macros */


/* creation of the interpreter */
BF_data *BF_new(size_t size, size_t bytes);
/* free memory */
void BF_close(BF_data *bf);

/* parse a string and execute it*/
unsigned int BF_do_string(BF_data *bf, char *str);
/* execute code from a file */
unsigned int BF_do_file(BF_data *bf, const char *file);

/* translate errors */
char* BF_read_error(unsigned int errn);


/* BF_data manipulation */

/* increment the stack ptr by i amount*/
unsigned int BF_stackptr_incr(BF_data *bf, unsigned long i);

/* decrement the stack ptr by i amount*/
unsigned int BF_stackptr_decr(BF_data *bf, unsigned long i);

/* set the stack ptr to i*/
unsigned int BF_stackptr_set(BF_data *bf, unsigned long i);

/* get the stack ptr */
unsigned long BF_stackptr_get(BF_data *bf);


/* increment the stack value at current stack ptr by i amount*/
unsigned int BF_stack_incr(BF_data *bf, unsigned long i);

/* decrement the stack value at current stack ptr by i amount*/
unsigned int BF_stack_decr(BF_data *bf, unsigned long i);

/* increment the stack value at n by i amount*/
unsigned int BF_stack_incr_at(BF_data *bf, unsigned long i, size_t n);

/* decrement the stack value at n by i amount*/
unsigned int BF_stack_decr_at(BF_data *bf, unsigned long i, size_t n);

/* set the stack value at current stack ptr */
unsigned int BF_stack_set(BF_data *bf, long i);

/* set the stack value at n */
unsigned int BF_stack_set_at(BF_data *bf, long i, size_t n);

/* get the stack value at current stack ptr */
long BF_stack_get(BF_data *bf);

/* get the stack value at n */
long BF_stack_get_at(BF_data *bf, size_t n);