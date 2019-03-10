#include "setup.h"
#include "log.h"
#include "opcodes.h"

//Contains code for execution of instructions

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

    printf("0 %0.6o (data read)\n", memory_address);
    return *word;
}

uint8_t data_read_byte(uint16_t memory_address)
{
    uint8_t *byte;
    byte = (uint8_t *)memory + memory_address;

    printf("0 %0.6o (data read)\n", memory_address);
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

    printf("1 %0.6o (data write)\n", memory_address);
    return *word = data;
}

uint8_t data_write_byte(uint16_t memory_address, uint8_t data)
{
    uint8_t *byte;
    byte = (uint8_t *)memory + memory_address;

    printf("1 %0.6o (data write)\n", memory_address);
    return *byte = data;
}

// Decode 6 bit addressing mode field and return the appropriate value
uint16_t read_operand_value(uint8_t src_field, int byte)
{
    uint8_t mode = (src_field >> 3) & 07; // bits 5-3
    uint8_t src = src_field & 07; // bits 2-0
    uint16_t value, addr_word, ptr;
    log(LOG_DEBUG, "read_operand_value(%0.2o) mode: %o reg: %o\n", src_field, mode, src);

    switch (mode)
    {
        case 0:// GEN_MODE_REGISTER:
            if(byte)
				value = reg[src] & 0377; // only return the low byte
			else        
                value = reg[src];            
            break;

        case 1:// GEN_MODE_REGISTER_DEFERRED:
            /* Register contains the address of the operand */
            if(byte)
				value = data_read_byte(reg[src]);
			else
				value = data_read_word(reg[src]);
            break;

        case 2: // GEN_MODE_AUTO_INCREMENT / PC_MODE_IMMEDIATE
            /* The value is located in the second word of the instruction and is added to the contents of the register.
                the PC is used as a pointer to fetch the operand before being incremented by two to point to the next instruction.  */

            /* Register is used as a pointer to sequential data then incremented. contents of the general register is the address of the operand
            /* FIXME Contents of registers are stepped (by one for bytes, by two for words, always by two for R6 and R7 */
            value = data_read_word(reg[src]);
            if(byte && src != 6 && src != 7)
				reg[src] += 1;
            else
				reg[src] += 2;
            break;
            
        case 3: // GEN_MODE_AUTO_INCREMENT_DEFERRED / PC_MODE_ABSOLUTE
            /*  The contents of the location following the instruction are taken as the
                address of the operand. Immediate data is interpreted as an absolute address
                (i.e., an address that remains constant no matter where in memory the assembled instruction is executed). */

            /* Register is first used as a pointer to a word containing the address of the operand, then incremented (always by 2; even for byte instructions). */
            /* The contents of register used as the address of the address of the operand. Operation is performed,  Contents of register incremented by 2. */
            if(byte)
				addr_word = data_read_byte(reg[src]);
			else
				addr_word = data_read_word(reg[src]);
            reg[src] += 2;
            
            if(byte)
            {
				ptr = data_read_byte(addr_word);
				value = data_read_byte(ptr);
			}
			else
			{
				ptr = data_read_word(addr_word);
				value = data_read_word(ptr);
			}
            break;

        case 4:// GEN_MODE_AUTO_DECREMENT:
            /* The contents of the selected general register are decremented (by two for word instructions, by one for byte instructions)
               and then used as the address of the operand */
            if(byte && src != 6 && src != 7)
				reg[src] -= 1;
            else
				reg[src] -= 2;
            value = data_read_word(reg[src]);
            break;

        case 5:// GEN_MODE_AUTO_DECREMENT_DEFERRED:
            /* Register is decremented (always by two; even for byte instructions) and then used as a pointer to a word
               containing the address of the operand */
            reg[src] -= 2;
            if(byte)
            {
				addr_word = data_read_byte(reg[src]);
				ptr = data_read_byte(addr_word);
				value = data_read_byte(ptr);
			}
			else
			{
				addr_word = data_read_word(reg[src]);
				ptr = data_read_word(addr_word);
				value = data_read_word(ptr);
			}
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

// Decode 6-bit addressing mode field and write the appropriate value
uint16_t write_operand_value(uint8_t dst_field, uint16_t value, int byte)
{
    uint8_t mode = (dst_field >> 3) & 07; // bits 5-3
    uint8_t dst = dst_field & 07; // bits 2-0
    uint16_t addr_word, ptr;

    log(LOG_DEBUG, "write_operand_value(%0.2o, %0.6o) mode: %o reg: %o\n", dst_field, value, mode, dst);

    switch (mode)
    {
        case 0:// GEN_MODE_REGISTER:
            if(byte)
				reg[dst] = (reg[dst] & 0177400) | value; // only modify the low byte of register
			else
                reg[dst] = value;            
            break;

        case 1:// GEN_MODE_REGISTER_DEFERRED:
            /* Register contains the address of the operand */
            if(byte)
				data_write_byte(reg[dst], value);
			else
				data_write_word(reg[dst], value);
            break;

        case 2: // GEN_MODE_AUTO_INCREMENT / PC_MODE_IMMEDIATE
            /* The value is located in the second word of the instruction and is added to the contents of the register.
                the PC is used as a pointer to fetch the operand before being incremented by two to point to the next instruction.  */

            /* Register is used as a pointer to sequential data then incremented. contents of the general register is the address of the operand
            /* Contents of registers are stepped (by one for bytes, by two for words, always by two for R6 and R7 */
            if(byte)
            {
				data_write_byte(reg[dst], value);
				if(dst != 6 && dst != 7)
					reg[dst] += 1;
				else
					reg[dst] += 2;
			}
            else
				data_write_word(reg[dst], value);
				reg[dst] += 2;
            break;
            
        case 3: // GEN_MODE_AUTO_INCREMENT_DEFERRED / PC_MODE_ABSOLUTE
            /*  The contents of the location following the instruction are taken as the
                address of the operand. Immediate data is interpreted as an absolute address
                (i.e., an address that remains constant no matter where in memory the assembled instruction is executed). */

            /* Register is first used as a pointer to a word containing the address of the operand, then incremented (always by 2; even for byte instructions). */
            /* The contents of register used as the address of the address of the operand. Operation is performed,  Contents of register incremented by 2. */
            if(byte)
				addr_word = data_read_byte(reg[dst]);
			else
				addr_word = data_read_word(reg[dst]);
            reg[dst] += 2;
            
            if(byte)
            {
				ptr = data_read_byte(addr_word);
				data_write_byte(ptr, value);
			}
			else
			{
				ptr = data_read_word(addr_word);
				data_write_word(ptr, value);
			}
            
            
            //data_write_word(addr_word, value);
            break;

        case 4:// GEN_MODE_AUTO_DECREMENT:
            /* The contents of the selected general register are decremented (by two for word instructions, by one for byte instructions)
               and then used as the address of the operand */
            if(byte)
            {
				if(dst != 6 && dst != 7)
					reg[dst] -= 1;
				else
					reg[dst] -= 2;
				data_write_byte(reg[dst], value);
			}
            else
				reg[dst] -= 2;
				data_write_word(reg[dst], value);
            
            break;

        case 5:// GEN_MODE_AUTO_DECREMENT_DEFERRED:
            /* Register is decremented (always by two; even for byte instructions) and then used as a pointer to a word
               containing the address of the operand */
			reg[dst] -= 2;
			if(byte)
			{
				addr_word = data_read_byte(reg[dst]);
				ptr = data_read_byte(addr_word);
				data_write_byte(ptr, value);
			}
			else
			{
				addr_word = data_read_word(reg[dst]);
				ptr = data_read_word(addr_word);
				data_write_word(ptr, value);
			}
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

//SINGLE OPERAND
//General
int op_clr(uint16_t instruction)
{
	int byte = instruction >> 15;
    uint8_t dst = instruction & 077;
    log(LOG_INFO, "CLR function called\n"); //, mode: %o reg: %o\n", mode, reg_num);
	
    write_operand_value(dst, 0, byte);
    psw.negative = 0;
    psw.zero = 1;
    psw.overflow = 0;
    psw.carry = 0;
    return 0;
}

int op_clrb(uint16_t instruction){
	return 0;
}


int op_com(uint16_t instruction){
	return 0;
}

int op_comb(uint16_t instruction){
	return 0;
}


int op_inc(uint16_t instruction){
	return 0;
}

int op_incb(uint16_t instruction){
	return 0;
}


int op_dec(uint16_t instruction){
	return 0;
}

int op_decb(uint16_t instruction){
	return 0;
}


int op_neg(uint16_t instruction){
	return 0;
}

int op_negb(uint16_t instruction){
	return 0;
}


//Shift and Rotate
int op_asr(uint16_t instruction){
	return 0;
}

int op_asrb(uint16_t instruction){
	return 0;
}


int op_asl(uint16_t instruction){
	return 0;
}

int op_aslb(uint16_t instruction){
	return 0;
}


int op_ror(uint16_t instruction){
	return 0;
}

int op_rorb(uint16_t instruction){
	return 0;
}


int op_rol(uint16_t instruction){
	return 0;
}

int op_rolb(uint16_t instruction){
	return 0;
}


int op_swab(uint16_t instruction){
	return 0;
}


//Multiple Precision
int op_adc(uint16_t instruction){
	return 0;
}

int op_adcb(uint16_t instruction){
	return 0;
}


int op_sbc(uint16_t instruction){
	return 0;
}

int op_sbcb(uint16_t instruction){
	return 0;
}


//DOUBLE OPERAND
//General
int op_mov(uint16_t instruction)
{
	int byte = instruction >> 15;
    
    uint8_t src = (instruction >> 6) & 077; // bits 11-6
    uint8_t dst = instruction & 077; // bits 5-0
    uint16_t value;
    
    log(LOG_INFO, "MOV function called\n");
    log(LOG_INFO, "byte mode is: %d\n", byte);
    
    value = read_operand_value(src, byte);
    write_operand_value(dst, value, byte);
    
/*
    N: set if (src) <0; cleared otherwise
    Z: set if (src) = 0; cleared otherwise
    V: cleared
    C: not affected 
*/
    psw.negative = ((int8_t)value < 0);
    psw.zero = (src == 0);
    psw.overflow = 0;

    return 0;

}

int op_movb(uint16_t instruction){
    uint8_t src = (instruction >> 6) & 077; // bits 11-6
    uint8_t dst = instruction & 077; // bits 5-0
    uint16_t value;
    int byte = 1;
    log(LOG_INFO, "MOV function called\n");
    value = read_operand_value(src, byte);
    write_operand_value(dst, value, byte);
    
/*
    N: set if (src) <0; cleared otherwise
    Z: set if (src) = 0; cleared otherwise
    V: cleared
    C: not affected 
*/
    psw.negative = ((int8_t)value < 0);
    psw.zero = (src == 0);
    psw.overflow = 0;

    return 0;
}


int op_cmp(uint16_t instruction){
	return 0;
}

int op_cmpb(uint16_t instruction){
	return 0;
}


int op_add(uint16_t instruction)
{
    uint8_t src_mode = (instruction >> 9) & 07; // bits 9-11
    uint8_t src = (instruction >> 6) & 07; // bits 6-8
    uint8_t dst_mode = (instruction >> 3) & 07; // bits 3-5
    uint8_t dst = instruction & 07; // bits 0-2
    int32_t value;
    uint16_t src_value;
    int byte = instruction >> 15;
    log(LOG_INFO, "ADD function called\n");
    log(LOG_INFO, "byte mode is: %d\n", byte);
       
    int16_t src_val = read_operand_value(src, byte);
    int16_t dst_val = read_operand_value(dst, byte);
    value = src_val + dst_val;

    write_operand_value(dst, (uint16_t)value, byte);
    
/*
N: set if result <0; cleared otherwise
Z: set if result = 0; cleared otherwise
V: set if there was arithmetic overflow as a result of the oper·
ation; that is both operands were of the same sign and the
result was of the opposite sign; cleared otherwise
C: set if there was a carry from the most significant bit of the
result; cleared otherwise 
*/
    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.overflow = (((src_val < 0) && (dst_val < 0) && (value >=0)) || 
					((src_val >= 0) && (dst_val >= 0) && (value < 0)));

    psw.carry = (value >> 17 & 1 == 1);
    return 0;

}

int op_sub(uint16_t instruction){
	return 0;
}


//logical
int op_bit(uint16_t instruction){
	return 0;
}

int op_bitb(uint16_t instruction){
	return 0;
}


int op_bic(uint16_t instruction){
	return 0;
}

int op_bicb(uint16_t instruction){
	return 0;
}


int op_bis(uint16_t instruction){
	return 0;
}

int op_bisb(uint16_t instruction){
	return 0;
}


//Register
int op_mul(uint16_t instruction){
	return 0;
}

int op_div(uint16_t instruction){
	return 0;
}

int op_ash(uint16_t instruction){
	return 0;
}

int op_ashc(uint16_t instruction){
	return 0;
}

int op_xor(uint16_t instruction){
	return 0;
}


int op_br(uint16_t instruction)
{
	//Unconditional branch
    log(LOG_INFO, "BRANCH function called\n");

    int offset = instruction - 0400;
    printf("offset is %o\n", offset);
    reg[7] += (2* offset);
    log(LOG_DEBUG, "new number! %o\n", reg[7]);

    return 0;
}
int op_bne(uint16_t instruction)
{
	//Branches if Z bit is clear
    log(LOG_INFO, "Branch Not Equal function called\n");
	if(psw.zero == 0){
		int offset = instruction - 01000;
		log(LOG_DEBUG, "offset is! %o\n", offset);
		reg[7] += (2* offset);
		log(LOG_DEBUG, "new PC! %o\n", reg[7]);
	}
	else
		log(LOG_DEBUG, "Branch not taken!\n");
    return 0;
}

int op_beq(uint16_t instruction)
{
	//Branches if Z bit set
    log(LOG_INFO, "Branch Equals (Zero) function called\n");
	if(psw.zero == 1){
		int offset = instruction - 01400;
		log(LOG_DEBUG, "offset is! %o\n", offset);
		reg[7] += (2* offset);
		log(LOG_DEBUG, "new PC! %o\n", reg[7]);
	}
	else
		log(LOG_DEBUG, "Branch not taken!\n");
    return 0;
}
int op_bpl(uint16_t instruction)
{
	//Branches if N bit clear
    log(LOG_INFO, "Branch Plus function called\n");
	if(psw.negative == 0){
		int offset = instruction - 0100000;
		log(LOG_DEBUG, "offset is! %o\n", offset);
		reg[7] += (2* offset);
		log(LOG_DEBUG, "new PC! %o\n", reg[7]);
	}
	else
		log(LOG_DEBUG, "Branch not taken!\n");
		
    return 0;
}
int op_bmi(uint16_t instruction)
{
	//Branches if N is set
    log(LOG_INFO, "Branch Minus function called\n");
	if(psw.negative == 1){
		int offset = instruction - 0100400;
		log(LOG_DEBUG, "offset is! %o\n", offset);
		reg[7] += (2* offset);
		log(LOG_DEBUG, "new PC! %o\n", reg[7]);
	}
	else
		log(LOG_DEBUG, "Branch not taken!\n");
		
    return 0;
}
int op_bvc(uint16_t instruction)
{
	//Branches if V is clear
    log(LOG_INFO, "Branch Overflow Cleared called\n");
	if(psw.overflow == 0){
		int offset = instruction - 0102000;
		log(LOG_DEBUG, "offset is! %o\n", offset);
		reg[7] += (2* offset);
		log(LOG_DEBUG, "new PC! %o\n", reg[7]);
	}
	else
		log(LOG_DEBUG, "Branch not taken!\n");
		
    return 0;
}
int op_bvs(uint16_t instruction)
{
	//Branches if V is set
    log(LOG_INFO, "Branch Overflow Set called\n");
	if(psw.overflow == 1){
		int offset = instruction - 0102400;
		log(LOG_DEBUG, "offset is! %o\n", offset);
		reg[7] += (2* offset);
		log(LOG_DEBUG, "new PC! %o\n", reg[7]);
	}
	else
		log(LOG_DEBUG, "Branch not taken!\n");
		
    return 0;
}
int op_bcc(uint16_t instruction)
{
	//Branches if C is clear
    log(LOG_INFO, "Branch Carry Cleared called\n");
	if(psw.carry == 0){
		int offset = instruction - 0103000;
		log(LOG_DEBUG, "offset is! %o\n", offset);
		reg[7] += (2* offset);
		log(LOG_DEBUG, "new PC! %o\n", reg[7]);
	}
	else
		log(LOG_DEBUG, "Branch not taken!\n");
		
    return 0;
}
int op_bcs(uint16_t instruction)
{
	//Branches if C is set
    log(LOG_INFO, "Branch Carry Set called\n");
	if(psw.carry == 1){
		int offset = instruction - 0103400;
		log(LOG_DEBUG, "offset is! %o\n", offset);
		reg[7] += (2* offset);
		log(LOG_DEBUG, "new PC! %o\n", reg[7]);
	}
	else
		log(LOG_DEBUG, "Branch not taken!\n");
		
    return 0;
}
//Signed conditional branches
int op_bge(uint16_t instruction)
{
	//Branches if N and V are both set or if they are both clear
    log(LOG_INFO, "Branch if Greater or equal called\n");
	if(psw.negative == psw.overflow){
		int offset = instruction - 02000;
		log(LOG_DEBUG, "offset is! %o\n", offset);
		reg[7] += (2* offset);
		log(LOG_DEBUG, "new PC! %o\n", reg[7]);
	}
	else
		log(LOG_DEBUG, "Branch not taken!\n");
		
    return 0;
}
int op_blt(uint16_t instruction)
{
	//Branches if N xor V 1
    log(LOG_INFO, "Branch if Less Than\n");
	if(psw.negative ^ psw.overflow == 1){
		int offset = instruction - 02400;
		log(LOG_DEBUG, "offset is! %o\n", offset);
		reg[7] += (2* offset);
		log(LOG_DEBUG, "new PC! %o\n", reg[7]);
	}
	else
		log(LOG_DEBUG, "Branch not taken!\n");
		
    return 0;
}
int op_bgt(uint16_t instruction)
{
	//Branches if Z xor (V xor N) is Zero
    log(LOG_INFO, "Branch if Greater Than called\n");
	if(psw.zero^(psw.negative ^ psw.overflow) == 0){
		int offset = instruction - 03000;
		log(LOG_DEBUG, "offset is! %o\n", offset);
		reg[7] += (2* offset);
		log(LOG_DEBUG, "new PC! %o\n", reg[7]);
	}
	else
		log(LOG_DEBUG, "Branch not taken!\n");
		
    return 0;
}
int op_ble(uint16_t instruction)
{
	//Branches if Z xor (V xor N) is One
    log(LOG_INFO, "Branch if Less or equal called\n");
	if(psw.zero^(psw.negative ^ psw.overflow) == 1){
		int offset = instruction - 03400;
		log(LOG_DEBUG, "offset is! %o\n", offset);
		reg[7] += (2* offset);
		log(LOG_DEBUG, "new PC! %o\n", reg[7]);
	}
	else
		log(LOG_DEBUG, "Branch not taken!\n");
		
    return 0;
}

//Unsigned conditional branches
int op_bhi(uint16_t instruction)
{
	//Branches if both c and z are zero
    log(LOG_INFO, "Branch if higher called\n");
	if(psw.carry == 0 && psw.zero == 0){
		int offset = instruction - 0101000;
		log(LOG_DEBUG, "offset is! %o\n", offset);
		reg[7] += (2* offset);
		log(LOG_DEBUG, "new PC! %o\n", reg[7]);
	}
	else
		log(LOG_DEBUG, "Branch not taken!\n");
		
    return 0;
}
int op_blos(uint16_t instruction)
{
	//Branches if C or Z is set
    log(LOG_INFO, "Branch if lower or same\n");
	if(psw.carry || psw.zero == 1){
		int offset = instruction - 0101400;
		log(LOG_DEBUG, "offset is! %o\n", offset);
		reg[7] += (2* offset);
		log(LOG_DEBUG, "new PC! %o\n", reg[7]);
	}
	else
		log(LOG_DEBUG, "Branch not taken!\n");
		
    return 0;
}


//Jump
int op_jmp(uint16_t instruction){
	log(LOG_INFO, "Jump function called\n");
    return 0;
}

//Halt
int op_halt(uint16_t instruction)
{
    log(LOG_INFO, "HALT function called\n");
    return 0;
}
