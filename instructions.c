#include "setup.h"
#include "log.h"
#include "opcodes.h"

//Contains code for execution of instructions

//SINGLE OPERAND
//General
int op_clr(uint16_t instruction)
{
    uint8_t dst = instruction & 077;
    log(LOG_INFO, "CLR function called\n");
	
    operand_value_write_word(dst, 0);
    psw.negative = 0;
    psw.zero = 1;
    psw.overflow = 0;
    psw.carry = 0;
    return 0;
}

int op_clrb(uint16_t instruction){
    uint8_t dst = instruction & 077;
    log(LOG_INFO, "CLRB function called\n");
	
    operand_value_write_byte(dst, 0);
    psw.negative = 0;
    psw.zero = 1;
    psw.overflow = 0;
    psw.carry = 0;
    return 0;
}


int op_com(uint16_t instruction){
    uint8_t dst = instruction & 077;
    log(LOG_INFO, "COM function called %d\n", dst);
    uint16_t value;
    value = operand_value_read_word(dst);
    value = (~value);
    operand_value_write_word(dst, value);

    //if msb of result is set 
    psw.negative = ((value >> 15) == 1);
    //set if result is 0
    psw.zero = (value == 0);
    psw.overflow = 0;
    psw.carry = 1;
	return 0;
}

int op_comb(uint16_t instruction){
	uint8_t dst = instruction & 077;
    log(LOG_INFO, "COM function called %d\n", dst);
    uint16_t value;
    value = operand_value_read_byte(dst);
    value = (~value);
    operand_value_write_byte(dst, value);

    //if msb of result is set 
    psw.negative = ((value >> 15) == 1);
    //set if result is 0
    psw.zero = (value == 0);
    psw.overflow = 0;
    psw.carry = 1;
	return 0;
	return 0;
}


int op_inc(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "INC function called\n", dst);
	int16_t value, val;
	val = operand_value_read_word(dst);
	value = (1 + value);
	operand_value_write_word(dst,value);

	//set if result is less than 0
	psw.negative = (value < 0);
	//set if result is 0
   	psw.zero = (value == 0);
	//set if dst held 077777 cleared otherwise
    psw.overflow = (val == 077777);
	return 0;
}

int op_incb(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "INCB function called\n", dst);
	int16_t value;
	value = operand_value_read_byte(dst);
	value = (1 + value);
	operand_value_write_byte(dst,value);

	//set if result is less than 0
	psw.negative = (value < 0);
	//set if result is 0
   	psw.zero = (value == 0);
	//set if dst held 077777 cleared otherwise
    psw.overflow = (value == 077777);
    psw.carry = 0;
	return 0;
}


int op_dec(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "DEC function called %d\n", dst);
    int16_t value, val;
    value = operand_value_read_word(dst) 
    val = value - 1;
	operand_value_write_word(dst,val);

	//set if result is less than 0
    psw.negative = (value < 0);
    //set if result is 0
    psw.zero = (value == 0);
    //set if dst held 100000 cleared otherwise
    psw.overflow = (value == 0100000);
    psw.carry = 0;
	return 0;
}


int op_decb(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "DECB function called %d\n", dst);
    int16_t value;
    value = operand_value_read_byte(dst) -1;
	operand_value_write_byte(dst, value);

	//set if result is less than 0
    psw.negative = (value < 0);
    //set if result is 0
    psw.zero = (value == 0);
    psw.overflow = (value == 0400); //FIXME jeff just made up that value, max 8 bit + 1
    psw.carry = 0;
	return 0;
}


int op_neg(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "NEG function called\n");
	uint16_t value, val, i;
	value = operand_value_read_word(dst);
	i = (~value);
	val = i + 1;
	operand_value_write_word(dst, val);
	
	//set if result is less than 0
    psw.negative = (value < 0);
    //set if result is 0
    psw.zero = (value == 0);
    //set if dst held 100000 cleared otherwise
    psw.overflow = (value == 0100000);
    psw.carry = (value != 0);

	return 0;
}


int op_negb(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "NEGB function called\n");
	uint8_t value, val, i;
	value = operand_value_read_byte(dst);
	i = (~value);
	val = i + 1;
	operand_value_write_byte(dst, val);
	
	//set if result is less than 0
    psw.negative = (value < 0);
    //set if result is 0
    psw.zero = (value == 0);
    psw.overflow = (value == 0400);  // max 8 bit + 1
    psw.carry = (value != 0);

	return 0;
}


//Shift and Rotate
int op_asr(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "ASR function called\n");
	uint16_t value, val;
	val = operand_value_read_word(dst);
	value = (val & 0100000) | (val >> 1);
	log(LOG_INFO, "Value %d\n", value);
	operand_value_write_word(dst, value);

	//set if high order bit of the result is set(result < 0)
	psw.negative = (value < 0);
	//set if result = 0
	psw.zero = (value == 0);
	//loaded from the exclusive OR of the N-bit and C-bit
	psw.carry = (val & 0000001);
	//loaded from the low order bit of the destination
    psw.overflow = (psw.negative ^ psw.carry);

	return 0;
}

int op_asrb(uint16_t instruction){
	uint8_t dst = instruction & 077;
	log(LOG_INFO, "ASR function called\n");
	uint16_t value, val;
	val = operand_value_read_byte(dst);
	value = (val & 0100000) | (val >> 1);
	log(LOG_INFO, "Value %d\n", value);
	operand_value_write_byte(dst, value);

	//set if high order bit of the result is set(result < 0)
	psw.negative = (value < 0);
	//set if result = 0
	psw.zero = (value == 0);
	//loaded from the exclusive OR of the N-bit and C-bit
	psw.carry = (val & 0000001);
	//loaded from the low order bit of the destination
    psw.overflow = (psw.negative ^ psw.carry);
	return 0;
}


int op_asl(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "ASR function called\n");
	uint16_t value;
	value = operand_value_read_word(dst);
	value = (value & 0000001) | (value << 1);
	log(LOG_INFO, "Value %d\n", value);
	operand_value_write_word(dst, value);

    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.carry = (value & 010000) >> 15;
    psw.overflow = (psw.negative ^ psw.carry);
	return 0;
}

int op_aslb(uint16_t instruction){
	uint8_t dst = instruction & 077;
	log(LOG_INFO, "ASR function called\n");
	uint16_t value;
	value = operand_value_read_byte(dst);
	value = (value & 0000001) | (value << 1);
	log(LOG_INFO, "Value %d\n", value);
	operand_value_write_byte(dst, value);

    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.carry = (value & 010000) >> 15;;
    psw.overflow = (psw.negative ^ psw.carry);
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
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "ADC function called\n");
	uint16_t value, val;
	value = operand_value_read_word(dst);
	val = value + psw.carry;
	log(LOG_INFO, "Value %d\n", value);
	operand_value_write_word(dst, val);

    psw.negative = (val < 0);
    psw.zero = (val == 0);
    psw.carry = ((value == 017777) && (psw.carry));
    psw.overflow = ((value == 0077777) && (psw.carry));
	return 0;
}

int op_adcb(uint16_t instruction){
	return 0;
}


int op_sbc(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "ADC function called\n");
	uint16_t value, val;
	value = operand_value_read_word(dst);
	val = value - psw.carry;
	log(LOG_INFO, "Value %d\n", value);
	operand_value_write_word(dst, val);

    psw.negative = (val < 0);
    psw.zero = (val == 0);
    psw.carry = ((val == 0) && (psw.carry));
    psw.overflow = (value == 0100000);
	return 0;
}

int op_sbcb(uint16_t instruction){
	return 0;
}


//DOUBLE OPERAND
//General
int op_mov(uint16_t instruction)
{
    uint8_t src = (instruction >> 6) & 077; // bits 11-6
    uint8_t dst = instruction & 077; // bits 5-0
    uint16_t value;
    
    log(LOG_INFO, "MOV function called\n");

    value = operand_value_read_word(src);
    operand_value_write_word(dst, value);
    
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

    log(LOG_INFO, "MOVB function called\n");
    value = operand_value_read_byte(src);
    operand_value_write_byte(dst, value);
    
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

    int16_t src_val = operand_value_read_word(src);
    int16_t dst_val = operand_value_read_word(dst);
    value = src_val + dst_val;

    operand_value_write_word(dst, (uint16_t)value);
    
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
