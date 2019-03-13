#include "opcodes.h"
#include "setup.h"
#include "log.h"

/// #include "instructions.c"

int log_level = LOG_NORMAL;

/*
http://pages.cpsc.ucalgary.ca/~dsb/PDP11/InsSet.html

RTS Instruction Format: bits 15:3 = 13 most significant bits
Bits 15-3 always contain octal 00020, which is the opcode for RTS
Bits 2-0 specify any one of the general purpose registers.
The register specified by bits 2-0 must be the same register used as the link between the JSR causing the jump and the RTS returning control.

Condition Code Instruction Format 15:5 = 11 most significant bits
Bits 15-5 - the opcode
Bit 4 - the "operator" which indicates set or clear with the values 1 and 0 respectively. If set, any selected bit is set; if clear, any selected bit is cleared.
Bits 3-0 - the mask field. Each of these bits corresponds to one of the four condition code bits. When one of these bits is set, then the corresponding condition code bit is set or cleared depending on the state of the "operator" (bit 4).

Single-Operand Instructions: bits 15:6 = 10 most significant bits
Bit 15 indicates word or byte operation.
Bits 14-6 indicate the operation code, which specifies the operation to be performed
Bits 5-0 indicate the 3-bit addressing mode field and the 3-bit general register field. These two fields are referred to as the destination field.

Branch Instructions: bits 15:8 = 8 most significant bits
The high byte (bits 15-8) of the instruction is an opcode specifying the conditions to be tested.
The low byte (bits 7-0) of the instruction is signed offset value in

Jump and Subroutine Instructions: bits 15:9 = 7 most significant bits
Bits 15-9 are always octal 004, the opcode for JSR
Bits 8-6 specify the link register. Any general purpose register may be used in the link, except R6 (SP).
Bits 5-0 designate the destination field that consists of addressing mode and general register fields. This specifies the starting address of the subroutine.
Register R7 (the Program Counter) is frequently used for both the link and the destination. For example, you may use JSR R7, SUBR, which is coded 004767. R7 is the only register that can be used for both the link and destination, the other GPRs cannot. Thus, if the link is R5, any register except R5 can be used for one destination field.

Double-Operand Instructions: bits 15:12 = 4 most significant bits
Bit 15 indicates word or byte operation except when used with opcode 6, in which case it indicates an ADD or SUBtract instruction.
Bits 14-12 indicate the opcode, which specifies the operation to be done.
Bits 11-6 indicate the 3-bit addressing mode field and the 3-bit general register field. These two fields are referred to as the source field.
Bits 5-0 indicate the 3-bit addressing mode field and the 3-bit general register field. These two fields are referred to as the destination field.


Some double-operand instructions (ASH, ASHC, MUL, DIV) must have the destination operand only in a register. Bits 15-9 specify the opcode. Bits 8-6 specify the destination register. Bits 5-0 contain the source field. XOR has a similar format, except that the source is in a register specified by bits 8-6, and the destination field is specified by bits 5-0.

*/
uint16_t fetch_instruction(uint16_t memory_address)
{
    uint16_t *word = NULL;

    if (memory_address % 2)
    {
        log(LOG_ERROR, "Invalid instruction fetch from odd memory location %0.6o\n",  memory_address);
        exit;
    }

    word = (uint16_t *)memory + memory_address / 2;

    log(LOG_INFO, "(instruction fetch) ", memory_address);
    printf("2 %0.6o\n", memory_address);
    return *word;
}

/*
Full 16-bit words or 8-bit bytes of information can be transferred on the
bus between a master and a slave. The information can be instructions,
addresses, or data. 
*/


uint16_t data_read_word(uint16_t memory_address)
{
    uint16_t *word = NULL;
    if (memory_address % 2)
    {
        log(LOG_ERROR, "Invalid word size data read from odd memory location %0.6o\n",  memory_address);
        exit;
    }

    word = (uint16_t *)memory + memory_address / 2;

    log(LOG_INFO, "data read (word) ");
    printf("0 %0.6o\n", memory_address);
    return *word;
}


uint8_t data_read_byte(uint16_t memory_address)
{
    uint8_t *byte;
    byte = (uint8_t *)memory + memory_address;

    log(LOG_INFO, "data read (byte) ");
    printf("0 %0.6o\n", memory_address);
    return *byte;
}



uint16_t data_write_word(uint16_t memory_address, uint16_t data)
{
    uint16_t *word = NULL;
    if (memory_address % 2)
    {
        log(LOG_ERROR, "Invalid data word write to odd memory location %0.6o\n",  memory_address);
        exit;
    }

    word = (uint16_t *)memory + memory_address / 2;

    log(LOG_INFO, "data write (word) ");
    printf("1 %0.6o\n", memory_address);
    return *word = data;
}



uint8_t data_write_byte(uint16_t memory_address, uint8_t data)
{
    uint8_t *byte;
    byte = (uint8_t *)memory + memory_address;

    log(LOG_INFO, "data write (byte) ");
    printf("1 %0.6o\n", memory_address);
    return *byte = data;
}




// Decode 6-bit addressing mode field and write the appropriate value
uint16_t operand_value_write_word(uint8_t dst_field, uint16_t value)
{
    uint8_t mode = (dst_field >> 3) & 07; // bits 5-3
    uint8_t dst = dst_field & 07; // bits 2-0
    uint16_t addr_word, ptr;

    log(LOG_DEBUG, "operand_value_write_word(%0.2o, %0.6o) mode: %o reg: %o\n", dst_field, value, mode, dst);

    switch (mode)
    {
        case 0:// GEN_MODE_REGISTER:
            reg[dst] = value;            
            break;

        case 1:// GEN_MODE_REGISTER_DEFERRED:
            /* Register contains the address of the operand */
            data_write_word(reg[dst], value);
            break;

        case 2: // GEN_MODE_AUTO_INCREMENT / PC_MODE_IMMEDIATE
            /* The value is located in the second word of the instruction and is added to the contents of the register.
                the PC is used as a pointer to fetch the operand before being incremented by two to point to the next instruction.  */

            /* Register is used as a pointer to sequential data then incremented. contents of the general register is the address of the operand
            /* Contents of registers are stepped (by one for bytes, by two for words, always by two for R6 and R7 */
            data_write_word(reg[dst], value);
            reg[dst] += 2;
            break;
            
        case 3: // GEN_MODE_AUTO_INCREMENT_DEFERRED / PC_MODE_ABSOLUTE
            /*  For PC mode absoute, The contents of the location following the instruction are taken as the
                address of the operand. Immediate data is interpreted as an absolute address
                (i.e., an address that remains constant no matter where in memory the assembled instruction is executed).*/

            /* Register is first used as a pointer to a word containing the address of the operand, then incremented (always by 2; even for byte instructions). */
            /* The contents of register used as the address of the address of the operand. Operation is performed,  Contents of register incremented by 2. */
            addr_word = data_read_word(reg[dst]);
            reg[dst] += 2;

            if (dst == 7) {
                data_write_word(addr_word, value);
            }
            else {
                ptr = data_read_word(addr_word);
                data_write_word(ptr, value);
            }

            break;

        case 4:// GEN_MODE_AUTO_DECREMENT:
            /* The contents of the selected general register are decremented (by two for word instructions, by one for byte instructions)
               and then used as the address of the operand */
            reg[dst] -= 2;
            data_write_word(reg[dst], value);
            break;

        case 5:// GEN_MODE_AUTO_DECREMENT_DEFERRED:
            /* Register is decremented (always by two; even for byte instructions) and then used as a pointer to a word
               containing the address of the operand */
            reg[dst] -= 2;
            addr_word = data_read_word(reg[dst]);
            ptr = data_read_word(addr_word);
            data_write_word(ptr, value);
            break;

        case 6: // GEN_MODE_INDEX / PC_MODE_RELATIVE
            /*  contents of memory location immediately following instruction word are added to (PC) to produce address */
            /* The contents of the selected general register, and an index word following the instruction word, are summed to form the address of the operand.  */
            /* Index addressing instructions are of the form OPR X(Rn) where X is the indexed word and is located in the
            memory location following the instruction word and Rn is the selected general register. */
            addr_word = data_read_word(reg[7]);
            reg[7] += 2; // move PC past data operand
            ptr = (uint16_t)(reg[dst] + addr_word);
            data_write_word(ptr, value);
            break;

        case 7: // GEN_MODE_INDEX_DEFERRED / PC_MODE_RELATIVE_DEFERRED
            // second word of the instruction, when added to the PC, contains the address of the address of the operand                
            /* Value X (stored in a word following the instruction) and (Rn) are added and used as a pointer to a word containing the
            address of the operand. Neither X nor (Rn) are modified. */
            addr_word = data_read_word(reg[7]);
            reg[7] += 2; // move PC past data operand                
            addr_word = (uint16_t)(addr_word + reg[dst]);
            ptr = data_read_word(addr_word);
            data_write_word(ptr, value);    
            break;
    }

    return value;
}



// Decode 6 bit addressing mode field and return the appropriate value
uint16_t operand_value_read_word(uint8_t src_field)
{
    uint8_t mode = (src_field >> 3) & 07; // bits 5-3
    uint8_t src = src_field & 07; // bits 2-0
    uint16_t value, addr_word, ptr;

    log(LOG_DEBUG, "operand_value_read_word(%0.2o) mode: %o reg: %o\n", src_field, mode, src);

    switch (mode)
    {
        case 0:// GEN_MODE_REGISTER:
            value = reg[src];
            break;

        case 1:// GEN_MODE_REGISTER_DEFERRED:
            /* Register contains the address of the operand */
            value = data_read_word(reg[src]);
            break;

        case 2: // GEN_MODE_AUTO_INCREMENT / PC_MODE_IMMEDIATE
            /* The value is located in the second word of the instruction and is added to the contents of the register.
                the PC is used as a pointer to fetch the operand before being incremented by two to point to the next instruction.  */

            /* Register is used as a pointer to sequential data then incremented. contents of the general register is the address of the operand
            /* FIXME Contents of registers are stepped (by one for bytes, by two for words, always by two for R6 and R7 */
            value = data_read_word(reg[src]);
            reg[src] += 2;
            break;
            
        case 3: // GEN_MODE_AUTO_INCREMENT_DEFERRED / PC_MODE_ABSOLUTE
            /*  The contents of the location following the instruction are taken as the
                address of the operand. Immediate data is interpreted as an absolute address
                (i.e., an address that remains constant no matter where in memory the assembled instruction is executed). */

            /* Register is first used as a pointer to a word containing the address of the operand, then incremented (always by 2; even for byte instructions). */
            /* The contents of register used as the address of the address of the operand. Operation is performed,  Contents of register incremented by 2. */
            addr_word = data_read_word(reg[src]);
            reg[src] += 2;

            if (src == 7) {
                data_read_word(addr_word);
            }
            else {
                ptr = data_read_word(addr_word);
                value = data_read_word(ptr);
            }
            break;

        case 4:// GEN_MODE_AUTO_DECREMENT:
            /* The contents of the selected general register are decremented (by two for word instructions, by one for byte instructions)
               and then used as the address of the operand */
            reg[src] -= 2;
            value = data_read_word(reg[src]);
            break;

        case 5:// GEN_MODE_AUTO_DECREMENT_DEFERRED:
            /* Register is decremented (always by two; even for byte instructions) and then used as a pointer to a word
               containing the address of the operand */
            reg[src] -= 2;
            addr_word = data_read_word(reg[src]);
            ptr = data_read_word(addr_word);
            value = data_read_word(ptr);
            break;

        case 6: // GEN_MODE_INDEX / PC_MODE_RELATIVE
            /*  contents of memory location immediately following instruction word are added to (PC) to produce address */
            /* The contents of the selected general register, and an index word following the instruction word, are summed to form the address of the operand.  */
            /* Index addressing instructions are of the form OPR X(Rn) where X is the indexed word and is located in the
            memory location following the instruction word and Rn is the selected general register. */
            addr_word = data_read_word(reg[7]);
            reg[7] += 2; // move PC past data operand
            ptr = (uint16_t)(reg[src] + addr_word);
            value = data_read_word(ptr);
            break;

        case 7: // GEN_MODE_INDEX_DEFERRED / PC_MODE_RELATIVE_DEFERRED
            // second word of the instruction, when added to the PC, contains the address of the address of the operand                
            /* Value X (stored in a word following the instruction) and (Rn) are added and used as a pointer to a word containing the
            address of the operand. Neither X nor (Rn) are modified. */
            addr_word = data_read_word(reg[src]);
            reg[src] += 2; // move PC past data operand                
            addr_word = (uint16_t)(addr_word + reg[src]);
            ptr = data_read_word(addr_word);
            value = data_read_word(ptr);    
            break;
    }

    return value;
}


uint8_t operand_value_write_byte(uint8_t dst_field, uint8_t value)
{
    uint8_t mode = (dst_field >> 3) & 07; // bits 5-3
    uint8_t dst = dst_field & 07; // bits 2-0
    uint16_t addr_word, ptr;

    log(LOG_DEBUG, "operand_value_write_byte(%0.2o, %0.3o) mode: %o reg: %o\n", dst_field, value, mode, dst);

    switch (mode)
    {
        case 0:// GEN_MODE_REGISTER:
            reg[dst] = (reg[dst] & 0177400) | value; // only modify the low byte of register
            break;

        case 1:// GEN_MODE_REGISTER_DEFERRED:
            /* Register contains the address of the operand */
            data_write_byte(reg[dst], value);
            break;

        case 2: // GEN_MODE_AUTO_INCREMENT / PC_MODE_IMMEDIATE
            /* The value is located in the second word of the instruction and is added to the contents of the register.
                the PC is used as a pointer to fetch the operand before being incremented by two to point to the next instruction.  */

            /* Register is used as a pointer to sequential data then incremented. contents of the general register is the address of the operand */

            data_write_byte(reg[dst], value);

            /* Contents of registers are stepped (by one for bytes, by two for words, always by two for R6 and R7 */
            if (dst == 6 || dst == 7)
                reg[dst] += 2;
            else
                reg[dst] += 1;

            break;
            
        case 3: // GEN_MODE_AUTO_INCREMENT_DEFERRED / PC_MODE_ABSOLUTE
            /*  The contents of the location following the instruction are taken as the
                address of the operand. Immediate data is interpreted as an absolute address
                (i.e., an address that remains constant no matter where in memory the assembled instruction is executed). */

            /* Register is first used as a pointer to a word containing the address of the operand, then incremented (always by 2; even for byte instructions). */
            /* The contents of register used as the address of the address of the operand. Operation is performed,  Contents of register incremented by 2. */
            addr_word = data_read_word(reg[dst]);
            reg[dst] += 2;

            if (dst == 7) {
                data_write_byte(addr_word, value);
            }
            else {
                ptr = data_read_word(addr_word);
                data_write_byte(ptr, value);
            }
            break;

        case 4:// GEN_MODE_AUTO_DECREMENT:
            /* The contents of the selected general register are decremented (by two for word instructions, by one for byte instructions)
               and then used as the address of the operand */
            if (dst == 6 || dst == 7)
                reg[dst] -= 2;
            else
                reg[dst] -= 1;

            data_write_byte(reg[dst], value);
            break;

        case 5:// GEN_MODE_AUTO_DECREMENT_DEFERRED:
            /* Register is decremented (always by two; even for byte instructions) and then used as a pointer to a word
               containing the address of the operand */
            reg[dst] -= 2;
            addr_word = data_read_word(reg[dst]);
            ptr = data_read_word(addr_word);
            data_write_byte(ptr, value);
            break;

        case 6: // GEN_MODE_INDEX / PC_MODE_RELATIVE
            /*  contents of memory location immediately following instruction word are added to (PC) to produce address */
            /* The contents of the selected general register, and an index word following the instruction word, are summed to form the address of the operand.  */
            /* Index addressing instructions are of the form OPR X(Rn) where X is the indexed word and is located in the
            memory location following the instruction word and Rn is the selected general register. */
            addr_word = data_read_word(reg[7]);
            reg[7] += 2; // move PC past data operand
            ptr = (uint16_t)(reg[dst] + addr_word);
            data_write_byte(ptr, value);
            break;

        case 7: // GEN_MODE_INDEX_DEFERRED / PC_MODE_RELATIVE_DEFERRED
            // second word of the instruction, when added to the PC, contains the address of the address of the operand                
            /* Value X (stored in a word following the instruction) and (Rn) are added and used as a pointer to a word containing the
            address of the operand. Neither X nor (Rn) are modified. */
            addr_word = data_read_word(reg[7]);
            reg[7] += 2; // move PC past data operand                
            addr_word = (uint16_t)(addr_word + reg[dst]);
            ptr = data_read_word(addr_word);
            data_write_byte(ptr, value);    
            break;
    }

    return value;
}



// Decode 6 bit addressing mode field and return the appropriate value
uint8_t operand_value_read_byte(uint8_t src_field)
{
    uint8_t mode = (src_field >> 3) & 07; // bits 5-3
    uint8_t src = src_field & 07; // bits 2-0
    uint8_t value;
    uint16_t addr_word, ptr;

    log(LOG_DEBUG, "operand_value_read_byte(%0.2o) mode: %o reg: %o\n", src_field, mode, src);

    switch (mode)
    {
        case 0:// GEN_MODE_REGISTER:
            value = reg[src] & 0377; // only return the low byte 
            break;

        case 1:// GEN_MODE_REGISTER_DEFERRED:
            /* Register contains the address of the operand */
            value = data_read_byte(reg[src]);
            break;

        case 2: // GEN_MODE_AUTO_INCREMENT / PC_MODE_IMMEDIATE
            /* The value is located in the second word of the instruction and is added to the contents of the register.
                the PC is used as a pointer to fetch the operand before being incremented by two to point to the next instruction.  */

            /* Register is used as a pointer to sequential data then incremented. contents of the general register is the address of the operand */
            
            value = data_read_byte(reg[src]);

            /* Contents of registers are stepped (by one for bytes, by two for words, always by two for R6 and R7 */
            if (src == 6 || src == 7)
                reg[src] += 2;
            else
                reg[src] += 1;

            break;
            
        case 3: // GEN_MODE_AUTO_INCREMENT_DEFERRED / PC_MODE_ABSOLUTE
            /*  The contents of the location following the instruction are taken as the
                address of the operand. Immediate data is interpreted as an absolute address
                (i.e., an address that remains constant no matter where in memory the assembled instruction is executed). */

            /* Register is first used as a pointer to a word containing the address of the operand, then incremented (always by 2; even for byte instructions). */
            /* The contents of register used as the address of the address of the operand. Operation is performed,  Contents of register incremented by 2. */
            addr_word = data_read_word(reg[src]);
            reg[src] += 2;

            if (src == 7) {
                value = data_read_byte(addr_word);
            }
            else {
                ptr = data_read_word(addr_word);
                value = data_read_byte(ptr);
            }
            break;

        case 4:// GEN_MODE_AUTO_DECREMENT:
            /* The contents of the selected general register are decremented (by two for word instructions, by one for byte instructions)
               and then used as the address of the operand */
            /* The contents of the selected general register are decremented (by two for word instructions, by one for byte instructions) */
            if (src == 6 || src == 7) // TODO this makes sense, but manual doesn't say word align SP and PC
                reg[src] -= 2;
            else
                reg[src] -= 1;

            value = data_read_byte(reg[src]);
            break;

        case 5:// GEN_MODE_AUTO_DECREMENT_DEFERRED:
            /* Register is decremented (always by two; even for byte instructions) and then used as a pointer to a word
               containing the address of the operand */
            reg[src] -= 2;
            addr_word = data_read_word(reg[src]);
            ptr = data_read_word(addr_word);
            value = data_read_byte(ptr);
            break;

        case 6: // GEN_MODE_INDEX / PC_MODE_RELATIVE
            /*  contents of memory location immediately following instruction word are added to (PC) to produce address */
            /* The contents of the selected general register, and an index word following the instruction word, are summed to form the address of the operand.  */
            /* Index addressing instructions are of the form OPR X(Rn) where X is the indexed word and is located in the
            memory location following the instruction word and Rn is the selected general register. */
            addr_word = data_read_word(reg[7]);
            reg[7] += 2; // move PC past data operand
            ptr = (uint16_t)(reg[src] + addr_word);
            value = data_read_byte(ptr);
            break;

        case 7: // GEN_MODE_INDEX_DEFERRED / PC_MODE_RELATIVE_DEFERRED
            // second word of the instruction, when added to the PC, contains the address of the address of the operand                
            /* Value X (stored in a word following the instruction) and (Rn) are added and used as a pointer to a word containing the
            address of the operand. Neither X nor (Rn) are modified. */
            addr_word = data_read_word(reg[src]);
            reg[src] += 2; // move PC past data operand                
            addr_word = (uint16_t)(addr_word + reg[src]);
            ptr = data_read_word(addr_word);
            value = data_read_byte(ptr);    
            break;
    }

    return value;
}

// Special Jump reads
uint16_t jump_read_word(uint8_t src_field)
{
    uint8_t mode = (src_field >> 3) & 07; // bits 5-3
    uint8_t src = src_field & 07; // bits 2-0
    uint16_t value, addr_word, ptr;

    log(LOG_DEBUG, "operand_value_read_word(%0.2o) mode: %o reg: %o\n", src_field, mode, src);

    switch (mode)
    {
        case 0:// GEN_MODE_REGISTER:
            value = reg[src];
            break;

        case 1:// GEN_MODE_REGISTER_DEFERRED:
            /* Register contains the address of the operand */
            value = data_read_word(reg[src]);
            break;

        case 2: // GEN_MODE_AUTO_INCREMENT / PC_MODE_IMMEDIATE
            /* The value is located in the second word of the instruction and is added to the contents of the register.
                the PC is used as a pointer to fetch the operand before being incremented by two to point to the next instruction.  */

            /* Register is used as a pointer to sequential data then incremented. contents of the general register is the address of the operand
            /* FIXME Contents of registers are stepped (by one for bytes, by two for words, always by two for R6 and R7 */
            value = data_read_word(reg[src]);
            reg[src] += 2;
            break;
            
        case 3: // GEN_MODE_AUTO_INCREMENT_DEFERRED / PC_MODE_ABSOLUTE
            /*  The contents of the location following the instruction are taken as the
                address of the operand. Immediate data is interpreted as an absolute address
                (i.e., an address that remains constant no matter where in memory the assembled instruction is executed). */

            /* Register is first used as a pointer to a word containing the address of the operand, then incremented (always by 2; even for byte instructions). */
            /* The contents of register used as the address of the address of the operand. Operation is performed,  Contents of register incremented by 2. */
            addr_word = data_read_word(reg[src]);
            reg[src] += 2;
            ptr = data_read_word(addr_word);
            value = ptr;
            break;

        case 4:// GEN_MODE_AUTO_DECREMENT:
            /* The contents of the selected general register are decremented (by two for word instructions, by one for byte instructions)
               and then used as the address of the operand */
            reg[src] -= 2;
            value = data_read_word(reg[src]);
            break;

        case 5:// GEN_MODE_AUTO_DECREMENT_DEFERRED:
            /* Register is decremented (always by two; even for byte instructions) and then used as a pointer to a word
               containing the address of the operand */
            reg[src] -= 2;
            addr_word = data_read_word(reg[src]);
            ptr = data_read_word(addr_word);
            value = ptr;
            break;

        case 6: // GEN_MODE_INDEX / PC_MODE_RELATIVE
            /*  contents of memory location immediately following instruction word are added to (PC) to produce address */
            /* The contents of the selected general register, and an index word following the instruction word, are summed to form the address of the operand.  */
            /* Index addressing instructions are of the form OPR X(Rn) where X is the indexed word and is located in the
            memory location following the instruction word and Rn is the selected general register. */
            addr_word = data_read_word(reg[7]);
            reg[7] += 2; // move PC past data operand
            ptr = (uint16_t)(reg[src] + addr_word);
            //log(LOG_INFO, "Pointer to %o\n", ptr);
            value = ptr;
            break;

        case 7: // GEN_MODE_INDEX_DEFERRED / PC_MODE_RELATIVE_DEFERRED
            // second word of the instruction, when added to the PC, contains the address of the address of the operand                
            /* Value X (stored in a word following the instruction) and (Rn) are added and used as a pointer to a word containing the
            address of the operand. Neither X nor (Rn) are modified. */
            addr_word = data_read_word(reg[src]);
            reg[src] += 2; // move PC past data operand                
            addr_word = (uint16_t)(addr_word + reg[src]);
            ptr = data_read_word(addr_word);
            value = ptr;   
            
            printf("RETURNING: %o", value); 
            break;
    }

    return value;
}

void print_regs()
{
    for (int i=0; i <= 7; i++)
        printf("R%d:\t%0.6o\n", i, reg[i]);

    printf("\n");
}


void test_regs()
{
    print_regs();
    reg[0] = 0000000;
    reg[1] = 0111111;
    reg[2] = 0122222;
    reg[3] = 0133333;
    reg[4] = 0144444;
    reg[5] = 0155555;
    reg[6] = 0166666;
    reg[7] = 0177777;
    print_regs();
    reg[0] = 0177777;
    reg[1] = 0166666;
    reg[2] = 0155555;
    reg[3] = 0144444;
    reg[4] = 0133333;
    reg[5] = 0122222;
    reg[6] = 0111111;
    reg[7] = 0000000;
    print_regs();    
}


void print_psw()
{
    char buf[125];
    //Current Mode (2), Previous Mode (2), Reserved (4), Priority (3), Trap (1), Negative (1), Zero (1), Overflow (1), Carry (1)
    //sprintf(buf, "Current Mode (%d), Previous Mode (%d), Reserved (%d), Priority (%d), Trap (%d), Negative (%d), Zero (%d), Overflow (%d), Carry (%d)\n",
    //psw.curr_mode, psw.prev_mode, psw.reserved, psw.priority, psw.trap, psw.negative, psw.zero, psw.overflow, psw.carry);
    //log(LOG_NORMAL, buf)


    printf("CM PM RSVD PRI TNZVC\n");
    //char buf2[255] = "%%0.2d %%0.4d %%0.3d ?????\n";
    //printf(buf2);
    //psw.curr_mode = 3;
    //printf("Current Mode (%d)", psw.curr_mode);    
    //sprintf(buf, buf2, psw.curr_mode, psw.prev_mode, psw.reserved, psw.priority);
    //sprintf(buf, buf2, 1, 2, 3, 4);

    /*
    buf[15] = (psw.trap == 1) ? 'T': 't';
    buf[16] = (psw.negative == 1) ? 'N': 'n';
    buf[17] = (psw.zero == 1) ? 'Z': 'z';
    buf[18] = (psw.overflow == 1) ? 'O': 'o';
    buf[19] = (psw.carry == 1) ? 'C': 'c';
    */

    buf[0] = '0' + ((psw.curr_mode & 2) >> 1);
    buf[1] = '0' + (psw.curr_mode & 1);
    buf[2] = ' ';
    buf[3] = '0' + ((psw.prev_mode & 2) >> 1);
    buf[4] = '0' + (psw.prev_mode & 1);
    buf[5] = ' ';
    buf[6] = '0' + ((psw.reserved & 8) >> 3);
    buf[7] = '0' + ((psw.reserved & 4) >> 2);
    buf[8] = '0' + ((psw.reserved & 2) >> 1);
    buf[9] = '0' + (psw.reserved & 1);
    buf[10] = ' ';
    buf[11] = '0' + ((psw.priority & 4) >> 2);
    buf[12] = '0' + ((psw.priority & 2) >> 1);
    buf[13] = '0' + (psw.priority & 1);
    buf[14] = ' ';
    buf[15] = '0' + psw.trap;
    buf[16] = '0' + psw.negative;
    buf[17] = '0' + psw.zero;
    buf[18] = '0' + psw.overflow;
    buf[19] = '0' + psw.carry;
    buf[20] = '\n';
    buf[21] = '\0';
    printf(buf);
}


void print_memory()
{
    uint16_t *word = NULL;
    uint8_t *hi_byte, *low_byte = NULL;
    
    printf("Contents of occupied memory locations (in octal):\nAddr    Word     Hi  Low\n");
    
    for(int word_offset=0; word_offset < MEMORY_SIZE / 2; word_offset++)
    {
        word = (uint16_t *)memory+word_offset;
        if (*word != 0) // only print locations that are not 0
        {
            low_byte = (uint8_t *)memory+(2*word_offset);
            hi_byte = (uint8_t *)memory+(2*word_offset)+1;
            printf("%0.6o: %0.6o\t %0.3o %0.3o\n", word_offset*2, *word, *hi_byte, *low_byte);
        }
    }
}


int test_psw()
{
    print_psw();
    psw.curr_mode = 0;
    psw.prev_mode = 0;
    psw.reserved = 0;
    psw.priority = 0;
    psw.trap = 0;
    psw.negative = 0;
    psw.zero = 1;
    psw.overflow = 0;
    psw.carry = 1;
    print_psw();
    psw.curr_mode = 3;
    psw.prev_mode = 3;
    psw.reserved = 15;
    psw.priority = 1;
    psw.trap = 1;
    psw.negative = 1;
    psw.zero = 1;
    psw.overflow = 1;
    psw.carry = 1;
    print_psw();
    psw.curr_mode = 2;
    psw.prev_mode = 2;
    psw.reserved = 0xA;
    psw.priority = 5;
    psw.trap = 0;
    psw.negative = 1;
    psw.zero = 0;
    psw.overflow = 1;
    psw.carry = 0;
    print_psw();
    psw.curr_mode = 1;
    psw.prev_mode = 1;
    psw.reserved = 5;
    psw.priority = 7;
    psw.trap = 1;
    psw.negative = 0;
    psw.zero = 1;
    psw.overflow = 0;
    psw.carry = 1;
    print_psw();    
}


int init_memory()
{
    for(int i=0; i <= MEMORY_SIZE; i++)
        memory[i]= 0;
}

int init_cpu(uint16_t start_pc)
{
    reg[0] = 0;
    reg[1] = 0;
    reg[2] = 0;
    reg[3] = 0;
    reg[4] = 0;
    reg[5] = 0;    
    reg[6] = 0;
    reg[7] = start_pc;

    psw.curr_mode = 0;
    psw.prev_mode = 0;
    psw.reserved = 0;
    psw.priority = 0;
    psw.trap = 0;
    psw.negative = 0;
    psw.zero = 0;
    psw.overflow = 0;
    psw.carry = 0;    
}

/*
initializes a function pointer then iterates through each possible opcode
When valid opcode is found, function pointer is set to point to corresponding 
opcode function. Opcode Function is then called with the instruction as an arg.
*/
int decode_and_execute(uint16_t instruction)
{

    int (*decoder)(uint16_t) = NULL;    //function pointer to actual decoder

    log(LOG_INFO, "checking for opcode in (octal): %0.6o \n", instruction);


    
    if (instruction == 0000000)
        decoder = op_halt; 
	//SINGLE OPERAND
    log(LOG_DEBUG, "instruction >> 6: %o\n", instruction >> 6);
    // TODO OP_COM and below were masking they byte bit, make sure they still work
    // (instruction >> 6) & 00077)// 10 bit op codes (finding just base)
    switch ((instruction >> 6))
    {
		//General
        case OP_CLR: 
            decoder = op_clr; 
            break;
        case OP_CLRB: 
            decoder = op_clrb;
            break;
        case OP_COM: 
            decoder = op_com; 
            break;
        case OP_INC: 
            decoder = op_inc; 
            break;
        case OP_DEC: 
            decoder = op_dec; 
            break;
        case OP_NEG: 
            decoder = op_neg; 
            break;
        case OP_INCB:
            decoder = op_incb;
            break;
              
        //Shift and Rotate
        case OP_ASR: 
            decoder = op_asr; 
            break;
        case OP_ASL: 
            decoder = op_asl; 
            break;
        case OP_ROR: 
            decoder = op_ror; 
            break;
        case OP_SWAB:
			decoder = op_swab; 
            break;
        //Multiple Precision
        case OP_ADC: 
            decoder = op_adc; 
            break;
        case OP_SBC: 
            decoder = op_sbc; 
            break;
    }



    //DOUBLE OPERAND
    log(LOG_DEBUG, "(instruction >> 12)& 007: %o\n", (instruction >> 12)& 007);
    switch ((instruction >> 12)& 007) // 3 bit op codes
    {
		//General
        case OP_MOV: 
            decoder = op_mov;
            break;
        case OP_CMP: 
            decoder = op_cmp;
            break;
        case OP_ADD:
            decoder = op_add;
            break;
        case OP_SUB:
            decoder = op_sub;
            break;
        //Logical
        case OP_BIT: 
            decoder = op_bit;
            break;
        case OP_BIC: 
            decoder = op_bic;
            break;
        case OP_BIS: 
            decoder = op_bis;
            break;
    }
    
    log(LOG_DEBUG, "instruction >> 9: %o\n", instruction >> 9);
    switch (instruction >> 9) // 6 bit op codes
    {
		//Register
        case OP_MUL: 
            decoder = op_mul;
            break;
        case OP_DIV: 
            decoder = op_div;
            break;
        case OP_ASH: 
            decoder = op_ash;
            break;
        case OP_ASHC: 
            decoder = op_ashc;
            break;
        case OP_XOR: 
            decoder = op_xor;
            break;           
    }
    
    log(LOG_DEBUG, "(instruction >> 8)<<2: %o\n", (instruction >> 8)<<2);
    switch ((instruction >> 8)<<2)   // Branch opcodes (opcode + offset)
    {
		//BRANCHES
        case OP_BR: 
            decoder = op_br;
            break;
        case OP_BNE: 
            decoder = op_bne;
            break;
        case OP_BEQ: 
            decoder = op_beq;
            break;
        case OP_BPL: 
            decoder = op_bpl;
            break;
        case OP_BMI: 
            decoder = op_bmi;
            break;
        case OP_BVC: 
            decoder = op_bvc;
            break;
        case OP_BVS: 
            decoder = op_bvs;
            break;
        case OP_BCC: 
            decoder = op_bcc;
            break;
        case OP_BCS: 
            decoder = op_bcs;
            break;
        //SIGNED CONDTIONAL BRANCHES
        case OP_BGE: 
            decoder = op_bge;
            break;
        case OP_BLT: 
            decoder = op_blt;
            break;
        case OP_BGT: 
            decoder = op_bgt;
            break;
        case OP_BLE: 
            decoder = op_ble;
            break;
        //UNSIGNED CONDTIONAL BRANCHES
        case OP_BHI: 
            decoder = op_bhi;
            break;
        case OP_BLOS: 
            decoder = op_blos;
            break;
    }
    if (decoder)
        return(decoder(instruction));

    log(LOG_ERROR, "Unknown instruction %0.6o\n", instruction);
    return E_INVALID_OP_CODE;
}

//Fetches the instruction at the current PC then calls decode and execute
//Stops simulation if unknown instruction or HALT is encountered
int run_simulation()
{

    int result;
    //http://pages.cpsc.ucalgary.ca/~dsb/PDP11/AddrModes.html
    //INC R3	005203	Add one to the contents of R3
    //ADD R2,R4	060204	Add the contents of R2 to the contents of R4, replacing the original contents of R4 with the sum.
    //CLR (R5)	005015	The Contents of the location specified in R5 are cleared.
    //CLR (R5)+	005025	Contents of R5 are used as the address of the operand. Clear selected operand and then increment the contents of R5 by two.
    //INC @(R2)+	005232	Contents of R2 are used as the address of the operand. The operand is increased by one, contents of R2 are incremented by two.
    //INCB -(R0)	105240	The contents of R0 are decremented by one, then used as the address of the operand. The operand byte is increased by one.
    //COM @-(R0)	005150	The contents of R0 are decremented by two and then used as the address of the operand. The operand is one's complemented.
    //CLR 200(R4)	005064 000200	The address of the operand is determined by adding 200 to the contents of R4. The resulting location is then cleared.
    uint16_t opcode, pc; /// = 0005015;  //CLR (R5)	005015

    while (1)
    {
        opcode = fetch_instruction(reg[7]);
        reg[7] += 2;  // move PC to next instruction

        result = decode_and_execute(opcode);

        if (log_level >= LOG_INFO)
        {
            print_psw();
            print_regs();
        }

        if (log_level >= LOG_DEBUG)
            print_memory();

        if (result == E_INVALID_OP_CODE)
        {
            log(LOG_ERROR, "Ending simulation due to invalid opcode");        
            break;
        }

        if (opcode == OP_HALT)
        {
            log(LOG_INFO, "Ending simulation due to halt instruction");
            break;
        }
    }

}

/*
Most instructions allocate six bits to specify an operand. Three bits
select one of eight addressing modes, and three bits select one of the
eight general registers

 a series of lines, each representing a 16-bit octal value. Each
line begins with either a @, – ,or * character. Lines beginning with an -
indicate an octal value to be loaded at the current load address. The
starting load address defaults to 0 and is incremented by two (one word)
after each load

Any line that begins with an @ indicates a change to the current load
address. There will be at most one line beginning with * to indicate the
starting address of the program. Youur simulator should use this if present,
or allow the user to otherwise input the starting address

*/


int main(int argc, char *argv[])
{
    uint16_t mem_load_address = 0000000;
    uint16_t proc_start_address = 0000000;
    uint16_t word_offset = 0;
    uint16_t *word = NULL;
    uint8_t *hi_byte = NULL;
    uint8_t *low_byte = NULL;    
    
    FILE *fp;
    char *file_name;
    int result;


    log_level = LOG_NONE;//LOG_DEBUG; //LOG_INFO; //LOG_NONE;


    switch (argc) // NOTE using fall through in the cases, so watch out
    {
        case 4: // file name, start address, and debug level
            log_level = strtol(argv[3], NULL, 10);

        case 3: // file name and start address
            proc_start_address = strtol(argv[2], NULL, 8);

        case 2: // file name only
            file_name = argv[1];
            break;

        default:
            printf("PDP11 Instruction Set Simulator\n");
            printf("Usage: %s file.ascii [starting adress] [debug level]\n", argv[0]);
            exit(-1);
            break;
    }

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        fprintf(stderr, "Can not open file %s\n", file_name);
        exit(1);
    }

    #define LINE_MAX 32
    char line[LINE_MAX];
    int i = 1;
    int test;


    init_memory();

    while (fgets(line, LINE_MAX, fp) != NULL) {
        log(LOG_DEBUG, "Input line %d: %s", i, line);
        switch (line[0])
        {
            case '-':
                word = (uint16_t *)memory+word_offset;
                *word = strtol(line+1, NULL, 8);

                
                low_byte = (uint8_t *)memory+(2*word_offset);
                hi_byte = (uint8_t *)memory+(2*word_offset)+1;
                log(LOG_DEBUG, "%0.6o: %0.6o\t %0.3o %0.3o\n", word_offset, *word, *hi_byte, *low_byte);
                
                word_offset += 1;
                break;

            case '*': //  set PC to value
                if (proc_start_address == 0) // protect override from command line
                {
                    proc_start_address = strtol(line+1, NULL, 8);
                    log(LOG_INFO, "Processor start address set to %0.6o\n", proc_start_address);
                }
                else
                {
                    log(LOG_INFO, "File requested start address %0.6o overriden by command line.\n", proc_start_address);
                }
                
                
                break;

            case '@': //@ set start memory address
                mem_load_address = strtol(line+1, NULL, 8);
                log(LOG_INFO, "Memory load starting from adress %0.6o\n", mem_load_address);
                break;
        
            default:
                break;
        }
        i +=1;
    }

    fclose(fp);

    if (log_level >= LOG_DEBUG)
        print_memory();

    log(LOG_INFO, "Processor start address %0.6o\n", proc_start_address);

    init_cpu(proc_start_address);

    result = run_simulation();

    return result;

}
