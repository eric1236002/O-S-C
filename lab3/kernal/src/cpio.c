#include "cpio.h"


void cpio_ls(unsigned long initramfs_start) {
    uart_send_string("\n\rListing files:\n");
    char *initrd_addr = (char *)initramfs_start;

    
    while (1) {
        struct cpio_newc_header *header = (struct cpio_newc_header *)initrd_addr;


        // check magic number
        char *magic = "070701";
        int magic_match = 1;
        for(int i = 0; i < 6; i++) {
            if(header->c_magic[i] != magic[i]) {
                magic_match = 0;
                break;
            }
        }
        
        if(!magic_match) {
            uart_send_string("Magic number mismatch\n\r");
            break;
        }



        int namesize = hextodec(header->c_namesize);
        int filesize = hextodec(header->c_filesize);

        
        char *name = initrd_addr + sizeof(struct cpio_newc_header);
        if(strcmp(name,"TRAILER!!!") == 0) {
            break;
        }
        uart_send_string(name);
        // calculate aligned offset
        unsigned int name_offset = sizeof(struct cpio_newc_header) + namesize;
        unsigned int file_offset = filesize;

        // align to 4 bytes
        name_offset = (name_offset + 3) & ~3;
        file_offset = (file_offset + 3) & ~3;

        // move to next header
        initrd_addr += name_offset + file_offset;
        
        uart_send_string("\n\r");
    }
}


void cpio_cat(char *filename,unsigned long initramfs_start) {
    char *initrd_addr = (char *)initramfs_start;

    
    while (1) {
        struct cpio_newc_header *header = (struct cpio_newc_header *)initrd_addr;


        // check magic number
        char *magic = "070701";
        int magic_match = 1;
        for(int i = 0; i < 6; i++) {
            if(header->c_magic[i] != magic[i]) {
                magic_match = 0;
                break;
            }
        }
        
        if(!magic_match) {
            uart_send_string("Magic number mismatch\n\r");
            break;
        }



        int namesize = hextodec(header->c_namesize);
        int filesize = hextodec(header->c_filesize);

        
        char *name = initrd_addr + sizeof(struct cpio_newc_header);
        if(strcmp(name,"TRAILER!!!") == 0) {
            uart_send_string("No such file\n\r");
            break;
        }
        // calculate aligned offset
        unsigned int name_offset = sizeof(struct cpio_newc_header) + namesize;
        unsigned int file_offset = filesize;

        // align to 4 bytes
        name_offset = (name_offset + 3) & ~3;
        file_offset = (file_offset + 3) & ~3;

        if(strcmp(name,filename) == 0) {
            char *filedata = initrd_addr + name_offset;
            uart_send_string(filedata);
            break;
        }
        // move to next header
        initrd_addr += name_offset + file_offset;
    }
}


void cpio_load_program(char *filename,unsigned long initramfs_start) {
    char *initrd_addr = (char *)initramfs_start;

    unsigned int program_addr = 0x200000;
    while (1) {
        struct cpio_newc_header *header = (struct cpio_newc_header *)initrd_addr;


        // check magic number
        char *magic = "070701";
        int magic_match = 1;
        for(int i = 0; i < 6; i++) {
            if(header->c_magic[i] != magic[i]) {
                magic_match = 0;
                break;
            }
        }
        
        if(!magic_match) {
            uart_send_string("Magic number mismatch\n\r");
            break;
        }



        unsigned int namesize = hextodec(header->c_namesize);
        unsigned int filesize = hextodec(header->c_filesize);

        
        char *name = initrd_addr + sizeof(struct cpio_newc_header);
        if(strcmp(name,"TRAILER!!!") == 0) {
            uart_send_string("No such file\n\r");
            break;
        }
        // calculate aligned offset
        unsigned int name_offset = sizeof(struct cpio_newc_header) + namesize;
        unsigned int file_offset = filesize;

        // align to 4 bytes
        name_offset = (name_offset + 3) & ~3;
        file_offset = (file_offset + 3) & ~3;

        if(strcmp(name,filename) == 0) {
            uart_send_string("Loading program : ");
            uart_send_string(filename);
            uart_send_string("\n\r");
            uart_send_int(file_offset);
            uart_send_string("\n\r");
            char *filedata = initrd_addr + name_offset;
            unsigned char *target = (unsigned char *)program_addr;
            while(file_offset--) {
                *target = *filedata;
                target++;
                filedata++;
                if(file_offset == 0) {
                    uart_send_string("Loading program success\n\r");
                    asm volatile("mov x0, 0x240      \n");
                    asm volatile("msr spsr_el1, x0   \n"); //spsr_el1 set to 0x240  //enable interrupt
                    asm volatile("msr elr_el1, %0    \n" ::"r"(program_addr)); //exception return address
                    asm volatile("msr sp_el0, %0    \n" ::"r"(program_addr + USTACK_SIZE)); //stack pointer.ensure the user program has enough stack space
                    asm volatile("bl core_timer_enable"); //enable core timer
                    asm volatile("eret   \n"); //switch to exception level 0
                    break;
                }
            }
        }
        // move to next header
        initrd_addr += name_offset + file_offset;
    }
}
