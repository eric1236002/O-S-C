#include "fdt.h"

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

void initramfs_callback(const char *node_name, const char *property_name, const void *property_value)
{
    if(strcmp(node_name,"/chosen")==0 && strcmp(property_name,"linux,initrd-start")==0){
        initramfs_start =bswap32(*(uint32_t *)property_value);
        uart_send_string("initramfs start: ");
        uart_send_hex(initramfs_start);
        uart_send_string("\n");
    }
}

void fdt_traverse(void *fdt,fdt_callback callback)
{
    struct fdt_header *header = (struct fdt_header *)fdt;

    if(bswap32(header->magic) != FDT_MAGIC){
        uart_send_string("Invalid DTB magic number\n");
        return;
    }
    uart_send_string("DTB magic number is valid\n");
    char *struct_block = (char *)fdt + bswap32(header->off_dt_struct);
    char *strings_block = (char *)fdt + bswap32(header->off_dt_strings);
    char *current_node=NULL;
    char *current_property=NULL;
    char *current_value=NULL;
    uint32_t len=0;;
    uint32_t name_offset=0;
    while(1){
        uint32_t tag = bswap32(*(uint32_t *)struct_block);
        struct_block += 4;//skip tag
        if(tag == FDT_BEGIN_NODE){
            current_node = (char *)struct_block;//node name
            align(struct_block+1,4);//skip node name, add +1 to skip \0
        }else if(tag == FDT_PROP){
            len = bswap32(*(uint32_t *)(struct_block));
            struct_block += 4;
            name_offset = bswap32(*(uint32_t *)(struct_block));
            struct_block += 4;
            current_property = (char *)strings_block+name_offset;
            current_value = (char *)struct_block;

            if(callback){
                callback(current_node,current_property,current_value);
            }
            struct_block += len;
            align(struct_block,4);
        }else if(tag == FDT_END_NODE || tag == FDT_END){
            break;
        }else{
            uart_send_string("Unknown token!\n");
            break;
        }
    }
}