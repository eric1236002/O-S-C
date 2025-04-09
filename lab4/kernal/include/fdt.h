#ifndef FDT_H
#define FDT_H

#include <stdint.h>
#include <uart.h>
#include <utils.h>
#define FDT_MAGIC 0xd00dfeed
#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009
#define NULL 0


/* FDT Header */
struct fdt_header {
    uint32_t magic;             /* magic number 0xd00dfeed */
    uint32_t totalsize;         /* DTB file size     */
    uint32_t off_dt_struct;     /* device tree structure offset */
    uint32_t off_dt_strings;    /* strings block offset */
    uint32_t off_mem_rsvmap;    /* reserve memory block offset */
    uint32_t version;           /* DTB version */
    uint32_t last_comp_version; /* last compatible version */
    uint32_t boot_cpuid_phys;   /* boot CPU ID */
    uint32_t size_dt_strings;   /* strings block size */
    uint32_t size_dt_struct;    /* device tree structure size */
};

typedef void (*fdt_callback)(const char *node_name, const char *property_name, const void *property_value); 
/* callback function :use to traverse the dtb, 
this function will be a general function.
It can have different function name,but the parameter is same, and then it will return different response.
Like you can have a function to print the device, and then you can have a function to get the device info,
or you can have a function to get the initramfs address, etc.
Depend on the callback function what you send to the fdt_traverse, it will return different response.*/

void fdt_traverse(void *dtb_addr, fdt_callback callback);
void initramfs_callback(const char *node_name, const char *property_name, const void *property_value);
#endif

