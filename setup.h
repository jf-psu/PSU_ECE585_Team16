//Contains initial setup of psw struct and memory that was 
//previously contained in main.c
//Also includes function declarations for instructions.c 

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

#define K 1024 // 2^10
#define WORD_SIZE 2 // bytes
#define MEMORY_SIZE 32 * K * WORD_SIZE

typedef struct psw_t{
    unsigned int carry:1;
    unsigned int overflow:1;
    unsigned int zero:1;
    unsigned int negative:1;
    unsigned int trap:1;
    unsigned int priority:3;
    unsigned int reserved:4;
    unsigned int prev_mode:2;
    unsigned int curr_mode:2;
} psw_t;

// globals storing the entire machine state. 
char memory[MEMORY_SIZE];
uint16_t reg[8];
psw_t psw;


//Functions used throughout

uint16_t operand_value_write_word(uint8_t dst_field, uint16_t value);
uint16_t operand_value_read_word(uint8_t src_field);

uint8_t operand_value_write_byte(uint8_t dst_field, uint8_t value);
uint8_t operand_value_read_byte(uint8_t src_field);



int op_halt(uint16_t instruction);

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

//Branch Functions
int op_br(uint16_t instruction);
int op_bne(uint16_t instruction);
int op_beq(uint16_t instruction);
int op_bpl(uint16_t instruction);
int op_bmi(uint16_t instruction);
int op_bvc(uint16_t instruction);
int op_bvs(uint16_t instruction);
int op_bcc(uint16_t instruction);
int op_bcs(uint16_t instruction);

//Signed Conditional Branch
int op_bge(uint16_t instruction);
int op_blt(uint16_t instruction);
int op_bgt(uint16_t instruction);
int op_ble(uint16_t instruction);

//Unsigned conditional Branch
int op_bhi(uint16_t instruction);
int op_blos(uint16_t instruction);

//Jump
int op_jmp(uint16_t instruction);

//Halt
