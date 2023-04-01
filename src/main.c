#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "opt.h"

#include "bf.h"

static BF_data *bf;
void handle_signal(int sig){
    if(bf == NULL) exit(128+sig);

    BF_close(bf);
    exit(128+sig);
}


int main(int argc, char *argv[]){
    
    char opt[] = "hvb:n:e:";
    size_t block_size = BF_DEFAULT_BYTES, array_range = BF_DEFAULT_SIZE;

    int option, optind = 1; char *optarg = "", *eval = "";
    while((option = getopt(argc, argv, opt, &optarg, &optind)) != -1){
        switch(option) {
            case 'h':
                // help
                printf( "Usage : %s [options] file\n"
                "Yet an other brainfuck interpreter\n\n"
                "Options :\n"
                "  -h\tShow this help.\n"
                "  -v\tDisplay the interpreter version.\n"
                "  -n\tNumber of cell to use. Default to 30 000.\n"
                "  -b\tBlock size for each cell. Default to 1 byte.\n"
                "  -e\tExecute the string given and exit.\n", argv[0]);
                return 0;
            case 'v':
                // version
                printf("%s\n", BF_VERSION_NAME);
                return 0;
            case 'b':
                // block size
                block_size = atoi(optarg);
                if(!(block_size & (block_size - 1)) == 0 && block_size != 0){
                    printf(" %d is not a valid byte. It must be 1, 2, 4 or 8.\n", block_size);
                    return 1;
                }
                break;
            case 'n':
                // array size
                array_range = atoi(optarg);
                if(array_range > BF_MAX_SIZE){
                    puts("Array size must be lower.\n");
                    return 1;
                } else if(array_range < BF_MIN_SIZE){
                    puts("Array size must be greater.\n");
                    return 1;
                }
                break;
            case 'e':
                //eval
                eval = optarg;
                break;
        }
    }

    unsigned int res;

    bf = BF_new(array_range, block_size);

    #ifdef _WIN32
        int sigs[] = {SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM};

        for(size_t i = 0; i < 6; i++){
            signal(sigs[i], handle_signal);
        }
    #endif
    #ifdef __linux__
        for(size_t i = 0; i < SIGRTMAX; i++){
            // signal for window change is useless
            // SIGKILL and SIGSTOP are uncatchable
            if(i == SIGWINCH || i == SIGKILL || i == SIGSTOP) continue; 
            signal(i, handle_signal);
        }
    #endif
    
    if(bf->errn > 0){
        printf("An error occured : %s\n", BF_read_error(bf->errn));
        BF_close(bf);
        return 1;
    }
    if(eval != "") { // eval
        res = BF_do_string(bf, eval);
        if(res > 0){
            printf("An error occured : %s\n", BF_read_error(res));
            BF_close(bf);
            return 1;
        }
    } else if(argc <= optind) { // read for stdin
        puts(BF_VERSION_NAME);
        char buffer[1024];
        char *msg;
        
        if(bf->bytes == 1) msg = "array[%d] = unsigned(%hhu) signed(%ld)\n";
        else if(bf->bytes == 2) msg = "array[%d] = unsigned(%u) signed(%ld)\n";
        else if(bf->bytes == 4) msg = "array[%d] = unsigned(%hu) signed(%ld)\n";
        else if(bf->bytes == 8) msg = "array[%d] = unsigned(%lu) signed(%ld)\n";

        while(1){
            printf("> ");
            fgets(buffer, 1024, stdin);
            
            res = BF_do_string(bf, buffer);

            printf(msg, BF_stackptr_get(bf), (unsigned long)BF_stack_get(bf), BF_stack_get(bf));
        }
    } else { // read the given file
        char *file = argv[optind];
        res = BF_do_file(bf, file);
        printf("%d, %d, %d\n", BF_stackptr_get(bf), bf->size, bf->bytes);
        if(res > 0){
            printf("An error occured : %s\n", BF_read_error(res));
            BF_close(bf);
            return 1;
        }
    }

    BF_close(bf);
    return 0;
}