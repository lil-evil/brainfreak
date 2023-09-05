#include <stdio.h>
#include <stdlib.h>

#include "bf.h"
#include "bf_error.h"


/* creation of the interpreter */
BF_data *BF_new(size_t size, size_t bytes){
    BF_data *bf = malloc(sizeof(BF_data));
    bf->size = 0;
    bf->ptr = 0;
    bf->errn = BF_OK;

    if(!(bytes & (bytes - 1)) == 0 && bytes != 0){bf->errn = BF_ERR_INVALID_BYTE; return bf;}
    if(size > BF_MAX_SIZE){bf->errn = BF_ERR_MAX_SIZE_EXCEED; return bf;}
    if(size < BF_MIN_SIZE){bf->errn = BF_ERR_MIN_SIZE_EXCEED; return bf;}
    if(bytes > BF_MAX_BYTES){bf->errn = BF_ERR_MAX_BYTE_EXCEED; return bf;}
    if(bytes < BF_MIN_BYTES){bf->errn = BF_ERR_MIN_BYTE_EXCEED; return bf;}

    bf->stack = calloc(size, bytes);
    if(bf->stack == NULL){
        bf->errn = BF_ERR_ALLOCATION_FAILED;
        return bf;
    }

    bf->size = size;
    bf->bytes = bytes;
    bf->state = 'i';

    return bf;    
}

/* free memory */
void BF_close(BF_data *bf){
    free(bf->stack);
    free(bf);
}

/* parse a string and execute it*/
unsigned int BF_do_string(BF_data *bf, char *code){
    int line = 0, position = 0;
    unsigned int res = BF_OK;

    bf->state = 'c';
    while (*code > 0) {
        switch (*code) {
            case BF_PTR_INCR : 
                res = BF_stackptr_incr(bf, 1); 
                if( res > 0 ){
                    printf("An error occured at line %d, position %d : %s\n", line, position+1, BF_read_error(res));
                    return res;
                }
                break;
            case BF_PTR_DECR : 
                res = BF_stackptr_decr(bf, 1); 
                if( res > 0 ){
                    printf("An error occured at line %d, position %d : %s\n", line, position+1, BF_read_error(res));
                    return res;
                }
                break;

            case BF_CELL_INCR :
                res = BF_stack_incr(bf, 1); 
                if( res > 0 ){
                    printf("An error occured at line %d, position %d : %s\n", line, position+1, BF_read_error(res));
                    return res;
                }
                break;
            case BF_CELL_DECR :
                res = BF_stack_decr(bf, 1); 
                if( res > 0 ){
                    printf("An error occured at line %d, position %d : %s\n", line, position+1, BF_read_error(res));
                    return res;
                }
                break;
            case BF_LOOP_OPEN :
                if(BF_stack_get(bf) == 0){
                    int depth = 1;
                    while( depth > 0 && *code > 0) {
                        ++code; ++position;
                        if(*code == ']') depth--;
                        else if (*code == '[') depth++;
                    }
                }
                break;
            case BF_LOOP_CLOSE :
                if(BF_stack_get(bf) != 0){
                    int depth = 1;
                    while(depth > 0 && *code > 0){
                        --code; --position;
                        if(*code == '[') depth--;
                        else if (*code == ']') depth++;
                    }
                }
                break;

            case BF_IO_OUT : putchar(BF_stack_get(bf)); break;
            case BF_IO_IN :
                res = BF_stack_set(bf, getchar());
                if( res > 0 ){
                    printf("An error occured at line %d, position %d : %s\n", line, position+1, BF_read_error(res));
                    return res;
                }
                break;
            case '\n' : ++line; position = 0; break;
        }
        ++position;
        ++code;         // consume a char into the code
    }

    bf->state = 'i';
    return BF_OK;
}

/* execute code from a file */
unsigned int BF_do_file(BF_data *bf, const char *file){
    FILE *fp = fopen(file, "r");

    if(fp == NULL){
        return BF_ERR_OPEN_FILE;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char buffer[size+1] = {};

    size_t byte_read = fread(&buffer, sizeof(char), size, fp);
    fclose(fp);

    // force a null char to prevent segfault
    buffer[size] = '\0';

    unsigned int res = BF_do_string(bf, buffer);
    return res;
}

/* BF_data manipulation */

/* increment the stack ptr by i amount*/
inline unsigned int BF_stackptr_incr(BF_data *bf, unsigned long i){
    if((bf->ptr)+i > bf->size){
        return BF_ERR_STKPTR_EXCEED_STK_SIZE;
    }
    (bf->ptr) += i;
    return BF_OK;
}

/* decrement the stack ptr by i amount*/
inline unsigned int BF_stackptr_decr(BF_data *bf, unsigned long i){
    if((bf->ptr)-i > bf->size){     // as it's unsigned, it cannot be bellow 0, but will go to max value
        return BF_ERR_STKPTR_BELOW_ZERO;
    }
    (bf->ptr) -= i;
    return BF_OK;
}

/* set the stack ptr to i*/
inline unsigned int BF_stackptr_set(BF_data *bf, unsigned long i){
    if(i > bf->size){
        return BF_ERR_STKPTR_EXCEED_STK_SIZE;
    }

    (bf->ptr) = i;
    return BF_OK;
}

/* get the stack ptr */
unsigned long BF_stackptr_get(BF_data *bf){
    return bf->ptr;
}


/* increment the stack value at current stack ptr by i amount*/
unsigned int BF_stack_incr(BF_data *bf, unsigned long i){
    if(bf->bytes == 1){
        *((char*)bf->stack + bf->ptr) += i ;
    } else if(bf->bytes == 2){
        *((short*)bf->stack + bf->ptr) += i ;
    } else if(bf->bytes == 4){
        *((int*)bf->stack + bf->ptr) += i ;
    } else if(bf->bytes == 8){
        *((long*)bf->stack + bf->ptr) += i ;        
    }
    return BF_OK;
}

/* decrement the stack value at current stack ptr by i amount*/
unsigned int BF_stack_decr(BF_data *bf, unsigned long i){
    if(bf->bytes == 1){
        *((char*)bf->stack + bf->ptr) -= i ;
    } else if(bf->bytes == 2){
        *((short*)bf->stack + bf->ptr) -= i ;
    } else if(bf->bytes == 4){
        *((int*)bf->stack + bf->ptr) -= i ;
    } else if(bf->bytes == 8){
        *((long*)bf->stack + bf->ptr) -= i ;        
    }
    return BF_OK;
}

/* increment the stack value at n by i amount*/
unsigned int BF_stack_incr_at(BF_data *bf, unsigned long i, size_t n){
    if(n > bf->size){
        return BF_ERR_STKPTR_EXCEED_STK_SIZE;
    }

    if(bf->bytes == 1){
        *((char*)bf->stack + n) += i ;
    } else if(bf->bytes == 2){
        *((short*)bf->stack + n) += i ;
    } else if(bf->bytes == 4){
        *((int*)bf->stack + n) += i ;
    } else if(bf->bytes == 8){
        *((long*)bf->stack + n) += i ;        
    }
    return BF_OK;
}

/* decrement the stack value at n by i amount*/
unsigned int BF_stack_decr_at(BF_data *bf, unsigned long i, size_t n){
    if(n > bf->size){
        return BF_ERR_STKPTR_EXCEED_STK_SIZE;
    }

    if(bf->bytes == 1){
        *((char*)bf->stack + n) -= i ;
    } else if(bf->bytes == 2){
        *((short*)bf->stack + n) -= i ;
    } else if(bf->bytes == 4){
        *((int*)bf->stack + n) -= i ;
    } else if(bf->bytes == 8){
        *((long*)bf->stack + n) -= i ;        
    }
    return BF_OK;
}

/* set the stack value at current stack ptr */
unsigned int BF_stack_set(BF_data *bf, long i){
    if(bf->bytes == 1){
        *((char*)bf->stack + bf->ptr) = i ;
    } else if(bf->bytes == 2){
        *((short*)bf->stack + bf->ptr) = i ;
    } else if(bf->bytes == 4){
        *((int*)bf->stack + bf->ptr) = i ;
    } else if(bf->bytes == 8){
        *((long*)bf->stack + bf->ptr) = i ;        
    }
    return BF_OK;
}

/* set the stack value at n */
unsigned int BF_stack_set_at(BF_data *bf, long i, size_t n){
        if(n > bf->size){
        return BF_ERR_STKPTR_EXCEED_STK_SIZE;
    }

    if(bf->bytes == 1){
        *((char*)bf->stack + n) = i ;
    } else if(bf->bytes == 2){
        *((short*)bf->stack + n) = i ;
    } else if(bf->bytes == 4){
        *((int*)bf->stack + n) = i ;
    } else if(bf->bytes == 8){
        *((long*)bf->stack + n) = i ;        
    }
    return BF_OK;
}

/* get the stack value at current stack ptr */
long BF_stack_get(BF_data *bf){
    if(bf->bytes == 1){
        return *((char*)bf->stack + bf->ptr);
    } else if(bf->bytes == 2){
        return *((short*)bf->stack + bf->ptr);
    } else if(bf->bytes == 4){
        return *((int*)bf->stack + bf->ptr);
    } else if(bf->bytes == 8){
        return *((long*)bf->stack + bf->ptr);        
    }
}

/* get the stack value at n */
long BF_stack_get_at(BF_data *bf, size_t n){
    if(n > bf->size){
        return BF_ERR_STKPTR_EXCEED_STK_SIZE;
    }

    if(bf->bytes == 1){
        return *((char*)bf->stack + n);
    } else if(bf->bytes == 2){
        return *((short*)bf->stack + n);
    } else if(bf->bytes == 4){
        return *((int*)bf->stack + n);
    } else if(bf->bytes == 8){
        return *((long*)bf->stack + n);        
    }
}

/* translate errors */
char* BF_read_error(unsigned int errn){
    switch (errn)
    {
        case BF_OK: return "No error";
        case BF_ERR_MAX_SIZE_EXCEED: return "Array size exceed max size" ;
        case BF_ERR_MIN_SIZE_EXCEED: return "Array size exceed min size" ;
        case BF_ERR_MAX_BYTE_EXCEED: return "Cell byte exceed max size" ;
        case BF_ERR_MIN_BYTE_EXCEED: return "Cell byte exceed min size" ;
        case BF_ERR_ALLOCATION_FAILED: return "Unable to allocate memory" ;
        case BF_ERR_INVALID_BYTE: return "Cell Byte must be a power of two" ;
        case BF_ERR_STKPTR_EXCEED_STK_SIZE: return "Stack pointer cannot exceed stack size" ;
        case BF_ERR_STKPTR_BELOW_ZERO: return "Stack pointer cannot be lesser than 0" ;
        case BF_ERR_OPEN_FILE: return "Cannot open the file.";

        default: return "Unknown error";
    }
};