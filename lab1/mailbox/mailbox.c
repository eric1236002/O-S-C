#include "mailbox.h"

void mailbox_call(unsigned int* message, unsigned int channel){
    //Combine the message address (upper 28 bits) with channel number (lower 4 bits)
    unsigned int message_addr =  ((unsigned int)((unsigned long)message) & 0xFFFFFFF0) | (channel & 0xF);
    //Check if Mailbox 0 status register’s full flag is set.
    while((*(volatile unsigned int*)(MAILBOX_STATUS) & MAILBOX_FULL));
    //If not, then you can write to Mailbox 1 Read/Write register
    *(volatile unsigned int*)(MAILBOX_WRITE) = message_addr;
    while(1)
    {
        //Check if Mailbox 0 status register’s empty flag is set.
        while ((*(volatile unsigned int*)MAILBOX_STATUS) & MAILBOX_EMPTY);
        //If not, then you can read from Mailbox 0 Read/Write register.
        unsigned int result = *(volatile unsigned int*)(MAILBOX_READ);
        //Check if the value is the same as you wrote in step 1
        if((result & 0xf) == channel)
        {
            return;
        }
    }
}

void get_board_revision(){
    unsigned int mailbox[7];
    mailbox[0] = 7 * 4; // buffer size in bytes
    mailbox[1] = REQUEST_CODE;
    // tags begin
    mailbox[2] = GET_BOARD_REVISION; // tag identifier
    mailbox[3] = 4; // maximum of request and response value buffer's length.
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0; // value buffer
    // tags end
    mailbox[6] = END_TAG;

    mailbox_call(mailbox, 8); // message passing procedure call, you should implement it following the 6 steps provided above.

//   printf("0x%x\n", mailbox[5]); // it should be 0xa020d3 for rpi3 b+
    uart_send_string("Board revision: ");
    uart_send_hex(mailbox[5]);
    uart_send_string("\n");
}

void get_memory(){
    unsigned int mailbox[8];
    mailbox[0] = 8 * 4; // buffer size in bytes
    mailbox[1] = REQUEST_CODE;
    // tags begin
    mailbox[2] = GET_ARM_MEMORY; // tag identifier
    mailbox[3] = 8; // maximum of request and response value buffer's length.
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0; // value buffer
    mailbox[6] = 0; // value buffer
    // tags end
    mailbox[7] = END_TAG;

    mailbox_call(mailbox, 8); // message passing procedure call, you should implement it following the 6 steps provided above.
    uart_send_string("ARM memory base address: ");
    uart_send_hex(mailbox[5]);
    uart_send_string("\n");
    uart_send_string("ARM memory size: ");
    uart_send_hex(mailbox[6]);
    uart_send_string("\n");
}