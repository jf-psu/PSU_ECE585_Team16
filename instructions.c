//Contains code for execution of instructions

//SINGLE OPERAND
//General
int op_clr(uint16_t instruction);
int op_clrb(uint16_t instruction);

int op_com(uint16_t instruction);
int op_comb(uint16_t instruction);

int op_inc(uint16_t instruction);
int op_incb(uint16_t instruction);

int op_dec(uint16_t instruction);
int op_decb(uint16_t instruction);

int op_neg(uint16_t instruction);
int op_negb(uint16_t instruction);

//Shift and Rotate
int op_asr(uint16_t instruction);
int op_asrb(uint16_t instruction);

int op_asl(uint16_t instruction);
int op_aslb(uint16_t instruction);

int op_ror(uint16_t instruction);
int op_rorb(uint16_t instruction);

int op_rol(uint16_t instruction);
int op_rolb(uint16_t instruction);

int op_swab(uint16_t instruction);

//Multiple Precision
int op_adc(uint16_t instruction);
int op_adcb(uint16_t instruction);

int op_sbc(uint16_t instruction);
int op_sbcb(uint16_t instruction);

//DOUBLE OPERAND
//General
int op_mov(uint16_t instruction);
int op_movb(uint16_t instruction);

int op_cmp(uint16_t instruction);
int op_cmpb(uint16_t instruction);

int op_add(uint16_t instruction);
int op_sub(uint16_t instruction);

//logical
int op_bit(uint16_t instruction);
int op_bitb(uint16_t instruction);

int op_bic(uint16_t instruction);
int op_bicb(uint16_t instruction);

int op_bis(uint16_t instruction);
int op_bisb(uint16_t instruction);

//Register
int op_mul(uint16_t instruction);
int op_div(uint16_t instruction);
int op_ash(uint16_t instruction);
int op_ashc(uint16_t instruction);
int op_xor(uint16_t instruction);

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
int op_jmp(uint16_t instruction);

//Halt

