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
    log(LOG_INFO, "COM function called\n");
    uint16_t old_value;
    old_value = operand_value_read_word(dst);
    old_value = (~old_value);
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
    log(LOG_INFO, "COMB function called\n");
    uint8_t old_value;
    old_value = operand_value_read_byte(dst);
    old_value = (~old_value);
    operand_value_write_byte(dst, old_value);

    //if msb of result is set 
    psw.negative = ((old_value >> 7) == 1);
    //set if result is 0
    psw.zero = (old_value == 0);
    psw.overflow = 0;
    psw.carry = 1;
	return 0;
}


int op_inc(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "INC function called\n");
	int16_t value, val;
	val = operand_value_read_word(dst);
	value = (1 + value);
	operand_value_write_word(dst,value);

	//set if result is less than 0
	psw.negative = (value < 0);
	//set if result is 0
   	psw.zero = (value == 0);
	//set if dst held 077777 cleared otherwise
    psw.overflow = (val == 0077777);
	return 0;
}

int op_incb(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "INCB function called\n");
	int16_t value, val;
	value = operand_value_read_byte(dst);
	val = (1 + value);
	operand_value_write_byte(dst,val);

	//set if result is less than 0
	psw.negative = (val < 0);
	//set if result is 0
   	psw.zero = (val == 0);
	//set if dst held 077777 cleared otherwise
    psw.overflow = (value == 0077777);
	return 0;
}


int op_dec(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "DEC function called\n");
    int16_t value, val;
    value = operand_value_read_word(dst);
    val = value - 1;
	operand_value_write_word(dst,val);

	//set if result is less than 0
    psw.negative = (val < 0);
    //set if result is 0
    psw.zero = (val == 0);
    //set if dst held 100000 cleared otherwise
    psw.overflow = (value == 0100000);
	return 0;
}


int op_decb(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "DECB function called\n");
    int16_t value, val;
    value = operand_value_read_byte(dst);
	val = value - 1;
	operand_value_write_byte(dst, val);

	//set if result is less than 0
    psw.negative = (val < 0);
    //set if result is 0
    psw.zero = (val == 0);
    psw.overflow = (value == 0100000); //FIXME jeff just made up that value, max 8 bit + 1
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
    psw.overflow = (value == 0100000);  // max 8 bit + 1
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
	log(LOG_INFO, "ASRB function called\n");
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
	log(LOG_INFO, "ASL function called\n");
	uint16_t value;
	value = operand_value_read_word(dst);
	value = (value & 0000001) | (value << 1);
	log(LOG_INFO, "Value %d\n", value);
	operand_value_write_word(dst, value);

    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.carry = ((value & 0100000) >> 15);
    psw.overflow = (psw.negative ^ psw.carry);
	return 0;
}

int op_aslb(uint16_t instruction){
	uint8_t dst = instruction & 077;
	log(LOG_INFO, "ASLB function called\n");
	uint16_t value;
	value = operand_value_read_byte(dst);
	value = ((value & 0000001) | (value << 1));
	log(LOG_INFO, "Value %d\n", value);
	operand_value_write_byte(dst, value);

    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.carry = ((value & 0100000) >> 15);
    psw.overflow = (psw.negative ^ psw.carry);
	return 0;
}


int op_ror(uint16_t instruction){
	uint8_t dst = instruction & 077;
	log(LOG_INFO, "ROR function called\n");
	uint16_t value;
	value = operand_value_read_word(dst);
	value = ((value >> 1) | (psw.carry << 15));
	operand_value_write_word(dst,value);

	psw.carry = (value & 0000001);
	psw.zero = (value == 0);
	psw.negative = (value < 0);
	psw.overflow = (psw.negative ^ psw.carry);
	return 0;
}

int op_rorb(uint16_t instruction){
	uint8_t dst = instruction & 077;
	log(LOG_INFO, "RORB function called\n");
	uint16_t value;
	value = operand_value_read_byte(dst);
	value = ((value >> 1) | (psw.carry << 15));
	operand_value_write_byte(dst,value);

	psw.carry = (value & 0000001);
	psw.zero = (value == 0);
	psw.negative = (value < 0);
	psw.overflow = (psw.negative ^ psw.carry);
	return 0;
}


int op_rol(uint16_t instruction){
	uint8_t dst = instruction & 077;
	log(LOG_INFO, "ROL function called\n");
	uint16_t value;
	value = operand_value_read_word(dst);
	value = (value << 1) | (psw.carry);
	operand_value_write_word(dst,value);

	psw.carry = ((value & 0100000) >> 15);
	psw.zero = (value == 0);
	psw.negative = (value < 0);
	psw.overflow = (psw.negative ^ psw.carry);
	return 0;
}

int op_rolb(uint16_t instruction){
	uint8_t dst = instruction & 077;
	log(LOG_INFO, "ROLB function called\n");
	uint16_t value;
	value = operand_value_read_byte(dst);
	value = (value << 1) | (psw.carry);
	operand_value_write_byte(dst,value);

	psw.carry = ((value & 0100000) >> 15);
	psw.zero = (value == 0);
	psw.negative = (value < 0);
	psw.overflow = (psw.negative ^ psw.carry);
	return 0;
}


int op_swab(uint16_t instruction){
	uint8_t dst = instruction & 077;
	log(LOG_INFO, "SWAB function called\n");
	uint16_t value;
	value = operand_value_read_byte(dst);
	value = (((value & 0177400) >> 8) | ((value & 0000377) << 8)) ;
	operand_value_write_byte(dst,value);

	psw.carry = 0;
	psw.zero = ((value & 0000377) == 0);
	psw.negative = ((value & 0000100) >> 7);
	psw.overflow = 0;
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
    psw.carry = ((value == 0177777) && (psw.carry));
    psw.overflow = ((value == 0077777) && (psw.carry));
	return 0;
}

int op_adcb(uint16_t instruction){
	uint8_t dst = instruction & 077;
	log(LOG_INFO, "ADCB function called\n");
	uint16_t value, val;
	value = operand_value_read_byte(dst);
	val = value + psw.carry;
	log(LOG_INFO, "Value %d\n", value);
	operand_value_write_byte(dst, val);

    psw.negative = (val < 0);
    psw.zero = (val == 0);
    psw.carry = ((value == 0177777) && (psw.carry));
    psw.overflow = ((value == 0077777) && (psw.carry));
	return 0;
}


int op_sbc(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "SBC function called\n");
	int16_t old_value, value;
	old_value = (int16_t)operand_value_read_word(dst);
	value = old_value - psw.carry;
	log(LOG_INFO, "Value %d\n", value);
	operand_value_write_word(dst, value);

    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.carry = !((old_value == 0) && (psw.carry));
    psw.overflow = (old_value == 0100000);  // bit 15 was set
	return 0;
}

int op_sbcb(uint16_t instruction){
    uint8_t dst = instruction & 077;
	log(LOG_INFO, "SBCB function called\n");
	int8_t old_value, value;
	old_value = (int8_t)operand_value_read_byte(dst);
	value = old_value - psw.carry;
	log(LOG_INFO, "Value %d\n", value);
	operand_value_write_byte(dst, value);

    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.carry = !((old_value == 0) && (psw.carry));
    psw.overflow = (old_value == 0200);  // bit 7 was set
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
/*
The MOVB to a register (unique among
byte instructions) extends the most significant bit of the low
order byte (sign extension). Otherwise MOVB operates on
bytes exactly as MOV operates on words. 
*/
int op_movb(uint16_t instruction)
{
    uint8_t src = (instruction >> 6) & 077; // bits 11-6
    uint8_t dst = instruction & 077; // bits 5-0
    int16_t full_value = 0;
	uint8_t value;
	
	value = operand_value_read_byte(src);
	//extend sign out
	full_value = (((value & 0200) << 8) | value);
    log(LOG_INFO, "MOVB function called\n");
    operand_value_write_word(dst, full_value);
    
/*
    N: set if (src) <0; cleared otherwise
    Z: set if (src) = 0; cleared otherwise
    V: cleared
    C: not affected 
*/
    psw.negative = ((int8_t)value < 0);
    psw.zero = (value == 0);
    psw.overflow = 0;

    return 0;
}


int op_cmp(uint16_t instruction)
{
	uint8_t src = (instruction >> 6) & 077; // bits 6-11
    uint8_t dst = (instruction) & 077; // bits 0-5
	uint16_t value;
	log(LOG_INFO, "CMP function called\n");
	
	int16_t src_val = operand_value_read_word(src);
    int16_t dst_val = operand_value_read_word(dst);
	log(LOG_INFO,"Source value is %d, Dest value is %d\n", src_val, dst_val);
	
	value = src_val - dst_val;
	
	log(LOG_INFO,"result is %d\n", value);
    
	/*
	N: set if result <0; cleared otherwise
	Z: set if result = 0; cleared otherwise
	V: set if there was arithmetic overflow; that is, operands were
	of opposite signs and the sign of the destination was the
	same as the sign of the result; cleared otherwise
	C: cleared if there was a carry from the most significant bit of
	the result; set otherwise 
	*/
    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.overflow = (((src_val < 0) && (dst_val < 0) && (value >=0)) || 
					((src_val >= 0) && (dst_val >= 0) && (value < 0)));
	psw.carry = (value >> 17 & 1 == 1);
	
	return 0;
}

int op_cmpb(uint16_t instruction)
{
	uint8_t src = (instruction >> 6) & 077; // bits 6-11
    uint8_t dst = (instruction) & 077; // bits 0-5
	uint16_t value;
	log(LOG_INFO, "CMPB function called\n");
	
	int16_t src_val = operand_value_read_byte(src);
    int16_t dst_val = operand_value_read_byte(dst);
	log(LOG_INFO,"Source value is %d, Dest value is %d\n", src_val, dst_val);
	
	value = src_val - dst_val;
	
	log(LOG_INFO,"result is %d\n", value);
    
	/*
	N: set if result <0; cleared otherwise
	Z: set if result = 0; cleared otherwise
	V: set if there was arithmetic overflow; that is, operands were
	of opposite signs and the sign of the destination was the
	same as the sign of the result; cleared otherwise
	C: cleared if there was a carry from the most significant bit of
	the result; set otherwise 
	*/
    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.overflow = (((src_val < 0) && (dst_val < 0) && (value >=0)) || 
					((src_val >= 0) && (dst_val >= 0) && (value < 0)));
	psw.carry = (value >> 17 & 1 == 1);
	
	return 0;
}


int op_add(uint16_t instruction)
{
    uint8_t src = (instruction >> 6) & 077; // bits 6-8
    uint8_t dst = instruction & 077; // bits 0-2
    int32_t value;
    uint16_t src_value;
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


int op_sub(uint16_t instruction)
{
    uint8_t src = (instruction >> 6) & 077; // bits 6-8
    uint8_t dst = instruction & 077; // bits 0-2
    int32_t value;
    uint16_t src_value;
    log(LOG_INFO, "SUB function called\n");

    int16_t src_val = operand_value_read_word(src);
    int16_t dst_val = operand_value_read_word(dst);
    value = dst_val - src_val;

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

    psw.carry = !(value >> 17 & 1 == 1);
    return 0;
}


//logical
int op_bit(uint16_t instruction)
{
	uint8_t src = (instruction >> 6) & 077; // bits 6-11
    uint8_t dst = (instruction) & 077; // bits 0-5
    int16_t value;
    
    log(LOG_INFO, "BIT function called\n");
    
    //Read values for source and destination
    int16_t src_val = operand_value_read_word(src);
    int16_t dst_val = operand_value_read_word(dst);
    log(LOG_INFO,"Source value is %d, Dest value is %d\n", src_val, dst_val);
    
    value = src_val & dst_val;
	log(LOG_INFO,"result is %o\n", value);
	operand_value_write_word(dst, value);
	/*  N: set if high-order bit of result set: cleared otherwise
		Z: set if result = 0; cleared otherwise
		V: cleared
		C: not affected 
	*/
    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.overflow = 0;

	return 0;
}


int op_bitb(uint16_t instruction)
{
	uint8_t src = (instruction >> 6) & 077; // bits 6-11
    uint8_t dst = (instruction) & 077; // bits 0-5
    int16_t value;
    
    log(LOG_INFO, "BITB function called\n");
    
    //Read values for source and destination
    int16_t src_val = operand_value_read_byte(src);
    int16_t dst_val = operand_value_read_byte(dst);
    log(LOG_INFO,"Source value is %d, Dest value is %d\n", src_val, dst_val);
    
    value = src_val & dst_val;
    
	log(LOG_INFO,"result is %o\n", value);
	operand_value_write_byte(dst, value);
	/*
	N: set if high-order bit of result set: cleared otherwise
	Z: set if result = 0; cleared otherwise
	V: cleared
	C: not affected 
	*/
    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.overflow = 0;

	return 0;
}


int op_bic(uint16_t instruction)
{
	uint8_t src = (instruction >> 6) & 077; // bits 6-11
    uint8_t dst = (instruction) & 077; // bits 0-5
    int16_t value;
    
    log(LOG_INFO, "BIC function called\n");
    
    //Read values for source and destination
    int16_t src_val = operand_value_read_word(src);
    int16_t dst_val = operand_value_read_word(dst);
    log(LOG_INFO,"Source value is %o, Dest value is %o\n", src_val, dst_val);
    
    value = ~(src_val) & dst_val;
	log(LOG_INFO,"result is %o\n", value);
    operand_value_write_word(dst, value);
	/*
	N: set if high order bit of result set; cleared otherwise
	Z: set if result = 0; cleared otherwise
	V: cleared
	C: not affected 
	*/
    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.overflow = 0;

	return 0;
}


int op_bicb(uint16_t instruction)
{
	uint8_t src = (instruction >> 6) & 077; // bits 6-11
    uint8_t dst = (instruction) & 077; // bits 0-5
    int16_t value;
    
    log(LOG_INFO, "BIC function called\n");
    
    //Read values for source and destination
    int16_t src_val = operand_value_read_byte(src);
    int16_t dst_val = operand_value_read_byte(dst);
    log(LOG_INFO,"Source value is %o, Dest value is %o\n", src_val, dst_val);
    
    value = ~(src_val) & dst_val;
	log(LOG_INFO,"result is %o\n", value);
    operand_value_write_byte(dst, value);
	/*
	N: set if high order bit of result set; cleared otherwise
	Z: set if result = 0; cleared otherwise
	V: cleared
	C: not affected 
	*/
    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.overflow = 0;

	return 0;
}


int op_bis(uint16_t instruction)
{
	uint8_t src = (instruction >> 6) & 077; // bits 6-11
    uint8_t dst = (instruction) & 077; // bits 0-5
    int16_t value;
    
    log(LOG_INFO, "BIS function called\n");
    
    //Read values for source and destination
    int16_t src_val = operand_value_read_word(src);
    int16_t dst_val = operand_value_read_word(dst);
    log(LOG_INFO,"Source value is %d, Dest value is %d\n", src_val, dst_val);
    
    value = src_val | dst_val;
	log(LOG_INFO,"result is %o\n", value);
    operand_value_write_word(dst, value);
	/*
	N: set if high-order bit of result set. cleared otherwise
	Z: set if result = 0: cleared otherwise
	V: cleared
	C: not affected 
	*/
    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.overflow = 0;

	return 0;
}


int op_bisb(uint16_t instruction)
{
	uint8_t src = (instruction >> 6) & 077; // bits 6-11
    uint8_t dst = (instruction) & 077; // bits 0-5
    int16_t value;
    
    log(LOG_INFO, "BIS function called\n");
    
    //Read values for source and destination
    int16_t src_val = operand_value_read_byte(src);
    int16_t dst_val = operand_value_read_byte(dst);
    log(LOG_INFO,"Source value is %d, Dest value is %d\n", src_val, dst_val);
    
    value = src_val | dst_val;
	log(LOG_INFO,"result is %o\n", value);
    operand_value_write_byte(dst, value);
	/*
	N: set if high-order bit of result set. cleared otherwise
	Z: set if result = 0: cleared otherwise
	V: cleared
	C: not affected 
	*/
    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.overflow = 0;

	return 0;
}


//Register
/*
The contents of the destination register and source taken as
two's complement integers are multiplied and stored in the
destination register and the succeeding register (if R is even).
If R is odd only the low order product is stored. 
 */
int op_mul(uint16_t instruction)
{
    uint8_t dst_reg = (instruction >> 6) & 07; // bits 6-8
    uint8_t src_reg = instruction & 07; // bits 0-2
    //uint8_t dst = (instruction >> 6) & 077; // bits 6-11
    uint8_t src = (instruction) & 077; // bits 0-5
    int32_t value;
    uint16_t src_value;
    
    log(LOG_INFO, "MUL function called\n");
       
	//call read/write operand with byte = 0 since there are no byte modes
    int16_t src_val = operand_value_read_word(src);
    int16_t dst_val = operand_value_read_word(dst_reg);
    log(LOG_INFO,"Source value is %d, Dest value is %d\n", src_val, dst_val);
    value = src_val * dst_val;
	log(LOG_INFO,"result is %d\n", value);
	
	if(dst_reg % 2)
	{
		log(LOG_INFO,"Odd destination register %d\n", dst_reg);
		operand_value_write_word(dst_reg, value);
	}
	else
	{
		log(LOG_INFO,"Even destination register %d\n", dst_reg);
		operand_value_write_word(dst_reg, value);
		operand_value_write_word(dst_reg|1, value >> 16);
    }
	/*
	N: set if product is <0: cleared otherwise
	Z: set if product is 0; cleared otherwise
	V: cleared
	C: set if the result is less than-2^15 or greater than or equal to
	2^15-1: 
	*/
    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.overflow = 0;
	if((value < -2^15) || (value >= 2^15 -1))
		psw.carry = 0;
	
    return 0;
}

/*
The 32-bit two's complement integer in R and Rv1 is divided
by the source operand. The quotient is left in R; the remainder
in Rv1. Division will be performed so that the remainder
is of the same sign as the dividend. R must be even. 
 */ 
int op_div(uint16_t instruction){
    uint8_t dst_reg = (instruction >> 6) & 07; // bits 6-8
    uint8_t src_reg = instruction & 07; // bits 0-2
    //uint8_t dst = (instruction >> 6) & 077; // bits 6-11
    uint8_t src = (instruction) & 077; // bits 0-5
    int32_t value, rem;
    uint16_t src_value;
    
    log(LOG_INFO, "DIV function called\n");
    if(dst_reg % 2)
	{
		log(LOG_INFO,"Odd destination register '%d' not allowed\n", dst_reg);
		return 0;
	}
	//Value to be divided is held in two registers
    int16_t src_val1 = operand_value_read_word(src);
    int16_t src_val2 = operand_value_read_word(src|1);
    int32_t src_val = (src_val1 << 16) + src_val2;
    int16_t dst_val = operand_value_read_word(dst_reg);
    
    log(LOG_INFO,"Source value is %d, register value is %d\n", src_val, dst_val);
    if(!src_val || abs(dst_val) > abs(src_val))
    {
		if(!src_val){
			psw.carry = 1;
			psw.overflow = 1;
			log(LOG_INFO,"Divide by 0 attempted! Cannot complete instruction!\n");
		}
		if(abs(dst_val) > abs(src_val)){
			log(LOG_INFO,"Quotient will exceed 15 bits! Abort!\n");
			psw.overflow = 1;
		}
		return 0;
	}
    value = dst_val / src_val;
	rem = dst_val % src_val;
	
	operand_value_write_word(dst_reg, value);
	operand_value_write_word(dst_reg|1, rem);
	log(LOG_INFO,"Quotient is %d\n", value);
	log(LOG_INFO,"Quotient is %d\n", rem);
	/*
	N: set if quotient <0; cleared otherwise
	Z: set if quotient = 0; cleared otherwise
	V: set if source = 0 or if the absolute value of the register is
	larger than the absolute value of the source. (In this case the
	instruction is aborted because the quotient would exceed 15
	bits.)
	C: set if divide 0 attempted; cleared otherwise 
	*/
    psw.negative = (value < 0);
    psw.zero = (value == 0);
	psw.carry = 0;
	
    return 0;
}
/*
The contents of the register are shifted right or left the number
of times specified by the shift count. The shift count is
taken as the low order 6 bits of the source operand. This
number ranges from -32 to + 31. Negative is a a right shift
and positive is a left shift. 
*/
int op_ash(uint16_t instruction){
	uint8_t dst_reg = (instruction >> 6) & 07; // bits 6-8
    uint8_t src_reg = instruction & 07; // bits 0-2
    //uint8_t dst = (instruction >> 6) & 077; // bits 6-11
    uint8_t src = (instruction) & 077; // bits 0-5
    uint8_t carry;
    int16_t value;
    
    int16_t src_val = operand_value_read_word(src); //number of bits to shift
    int16_t dst_val = operand_value_read_word(dst_reg); //register val to be shifted
    
    src_val = src_val & 077; //lower 6 bits of src value for shift
    if(src_val > 0){
		//shift left
		carry = ((dst_val << src_val - 1) >> 15) & 01;
		value = value = dst_val << src_val;
	}
	else{
		//shift right
		carry = (dst_val >> abs(src_val) - 1) & 01;
		value = dst_val >> abs(src_val);
	}
	
    operand_value_write_word(dst_reg, value);
    /*
	N: set if result <0; cleared otherwise
	Z: set if result = 0; cleared otherwise
	V: set if sign of register changed during shift; cleared otherwise
	C: loaded from last bit shifted out of register 
	*/
	
    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.overflow = (value << 15 != dst_val << 15);
	psw.carry = carry;
	return 0;
}

/*
The contents of the register and the register ORed with one
are treated as one 32 bit word, R + 1 (bits 0-15) and R (bits
16-31) are shifted right or left the number of times specified
by the shift count The shift count is taken as the low order 6
bits of the source operand. This number ranges from -32 to
+ 31. Negative is a right shift and positive is a left shift
When the register chosen is an odd number the register
and the register OR'ed with one are the same. In this case the
right shift becomes a rotate (for up to a shift of 16). The 16
bit word is rotated right the number of bits specified by the
shift count 
 */
int op_ashc(uint16_t instruction){
	uint8_t dst_reg = (instruction >> 6) & 07; // bits 6-8
    uint8_t src_reg = instruction & 07; // bits 0-2
    //uint8_t dst = (instruction >> 6) & 077; // bits 6-11
    uint8_t src = (instruction) & 077; // bits 0-5
    uint8_t carry;
    int16_t value;
    
    int16_t src_val = operand_value_read_word(src); //number of bits to shift
    int16_t dst_val = operand_value_read_word(dst_reg); //register val to be shifted
    
    src_val = src_val & 077; //lower 6 bits of src value for shift
    if(src_val > 0){
		//shift left
		carry = ((dst_val << src_val - 1) >> 15) & 01;
		value = value = dst_val << src_val;
	}
	else{
		//shift right
		carry = (dst_val >> abs(src_val) - 1) & 01;
		value = dst_val >> abs(src_val);
	}
	
    operand_value_write_word(dst_reg, value);
    /*
	N: set if result <0; cleared otherwise
	Z: set if result = 0; cleared otherwise
	V: set if sign bit changes during the shift; cleared otherwise
	C: loaded with high order bit when left Shift ; loaded with low
	order bit when right shift (loaded with the last bit shifted out
	of the 32-bit operand) 
	*/
	
    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.overflow = (value << 15 != dst_val << 15);
	psw.carry = carry;
	return 0;
}

/*
The exclusive OR of the register and destination operand is
stored in the destination address. Contents of register are
unaffected. Assembler format is: XOR R.D 
*/
int op_xor(uint16_t instruction){
	uint8_t r_reg = (instruction >> 6) & 07; // bits 6-8
    uint8_t dst_reg = instruction & 07; // bits 0-2
    //uint8_t dst = (instruction >> 6) & 077; // bits 6-11
    uint8_t dst = (instruction) & 077; // bits 0-5
    int16_t value;
    
    int16_t reg_val = operand_value_read_word(r_reg); //register value
    int16_t dst_val = operand_value_read_word(dst); //destination val
    
    value = reg_val ^ dst_val;
    
    operand_value_write_word(dst, value);
    /*
	N: set if the result <0: cleared otherwise
	Z set if result = 0: cleared otherwise
	V: cleared
	C: unaffected 
    */
    psw.negative = (value < 0);
    psw.zero = (value == 0);
    psw.overflow = 0;
	return 0;
    
}

//BRANCHES
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
	uint8_t dst = (instruction) & 077; // bits 0-5
    uint8_t dst_mode = (instruction >> 3) & 07; // bits 3-5
	log(LOG_INFO, "Jump function called\n");
	
	//Illegal instruction if destination mode is 0 (cannot pass control
	//to a register)
	if(dst_mode == 0)
	{
		log(LOG_INFO, "Destination mode 0! Cannot pass control to register!\n");
		return 0;
	}
	
	//int16_t dst_val = operand_value_read_word(dst);
	//log(LOG_INFO, "Destination is: %o\n", dst);
	//log(LOG_INFO, "Destination Value is: %o\n", dst_val);
	//log(LOG_INFO, "register Value is: %o\n", reg[7]);
	
	uint16_t addr_word = data_read_word(reg[7]);
	reg[7] += 2; // move PC past data operand
	uint16_t ptr = (reg[7] + addr_word);
	log(LOG_INFO, "POINTER IS :%o\n",ptr);
	//uint16_t value = data_read_word(ptr);
    reg[7] = (ptr);
    return 0;
}

//Halt
int op_halt(uint16_t instruction)
{
    log(LOG_INFO, "HALT function called\n");
    return 0;
}
