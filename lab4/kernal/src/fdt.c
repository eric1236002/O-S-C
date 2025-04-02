#include "fdt.h"

unsigned long initramfs_start = 0;

/*
struct of DTB

header

memory reserve(align 8)

struct block(align 4)

strings block(no align)

*/

uint32_t bswap32(uint32_t x) {
    return __builtin_bswap32(x);
}

void fdt_traverse(void *fdt,fdt_callback callback)
{
    struct fdt_header *header = (struct fdt_header *)fdt;

    if(bswap32(header->magic) != FDT_MAGIC){
        uart_send_string("\n\rInvalid DTB magic number\n");
        return;
    }
    uart_send_string("\n\rDTB magic number is valid");
    char *struct_block = (char *)fdt + bswap32(header->off_dt_struct);
    char *strings_block = (char *)fdt + bswap32(header->off_dt_strings);
    char *current_node=NULL;
    char *current_property=NULL;
    char *current_value=NULL;
    uint32_t len=0;;
    uint32_t name_offset=0;
    while(1){
        uint32_t tag = bswap32(*(uint32_t *)struct_block);
        uart_send_string("\n\rStruct block: ");
        uart_send_hex(struct_block);
        for(int i=0;i<8;i++){
            uart_send_hex( bswap32(*(uint32_t *)(struct_block+i*4)));
        }
        uart_send_string("\n\rTag: ");
        uart_send_hex(tag);
        struct_block += 4;//skip tag
        // uart_send_string("\n\rAfter  struct_block: ");
        // uart_send_hex(struct_block);


        
        if(tag == FDT_BEGIN_NODE){
            current_node = (char *)struct_block;//node name
            uart_send_string("\n\r=====================Node name: ");
            uart_send_string(current_node);
            // uart_send_string("\n\r");
            // uart_send_string("string_len: ");
            // uart_send_hex(string_len(current_node));
            len = align(string_len(current_node)+1,4);//skip node name, add +1 to skip \0
            // uart_send_string("\n\rLen: ");
            // uart_send_hex(len);
            // uart_send_string("\n\rAfter len Struct block: ");
            uart_send_hex(struct_block);
            struct_block += len;
        }else if(tag == FDT_PROP){
            uart_send_string("\n\rProperty tag\n");
            len = bswap32(*(uint32_t *)(struct_block));
            // uart_send_string("\n\rLen: ");
            // uart_send_hex(len);
            struct_block += 4;
            name_offset = bswap32(*(uint32_t *)(struct_block));
            struct_block += 4;

            current_property = (char *)strings_block+name_offset;
            uart_send_string("\n\rCurrent property: ");
            uart_send_string(current_property);
            current_value = (char *)struct_block;
            uart_send_string("\n\rCurrent value: ");
            uart_send_string(current_value);
            // if(strcmp(current_property,"linux,initrd-start")==0){
            //     initramfs_start = *(unsigned long *)current_value;
            //     uart_send_string("\n\rInitramfs start: ");
            //     uart_send_hex(initramfs_start);
            //     return;
            // }

            if(callback){
                callback(current_node,current_property,current_value);
            }
            struct_block += align(len,4);
        }else if(tag == FDT_NOP){
            uart_send_string("\n\rNOP tag, ignoring\n");
            continue;
        }else if(tag == FDT_END_NODE){
            uart_send_string("\n\rEnd node tag\n");
        }else if(tag == FDT_END){
            uart_send_string("\n\rEnd of DTB\n");
            return;
        }else{
            uart_send_string("\n\rUnknown token: ");
            uart_send_hex(tag);
            uart_send_string("\n");
            return;
        }
    }
}