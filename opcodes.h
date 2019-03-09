// PDP11 mnemonic to opcode mapping
// note that these are octal values
// they start with an extra 0 to specify base 8.
// Ordered as seen in the PDP11 Manual

//SINGLE OPERAND
//General
#define OP_CLR 00050
#define OP_CLRB 01050

#define OP_COM 00051
#define OP_COMB 01051

#define OP_INC 00052
#define OP_INCB 01052

#define OP_DEC 00053
#define OP_DECB 01053

#define OP_NEG 00054
#define OP_NEGB 01054

//Shift and Rotate
#define OP_ASR 00062
#define OP_ASRB 01062

#define OP_ASL 00063
#define OP_ASLB 01063

#define OP_ROR 00060
#define OP_RORB 01060

#define OP_ROL 00061
#define OP_ROLB 01061

#define OP_SWAB 00003

//Multiple Precision
#define OP_ADC 00055
#define OP_ADCB 01055

#define OP_SBC 00056
#define OP_SBCB 01056

//DOUBLE OPERAND
//General
#define OP_MOV 001
#define OP_MOVB 011

#define OP_CMP 002
#define OP_CMPB 012

#define OP_ADD 006
#define OP_SUB 016

//logical
#define OP_BIT 003
#define OP_BITB 013

#define OP_BIC 004
#define OP_BICB 014

#define OP_BIS 005
#define OP_BISB 015

//Register
#define OP_MUL 0070
#define OP_DIV 0071
#define OP_ASH 0072
#define OP_ASHC 0073
#define OP_XOR 0074
//#define OP_DD 00003

//Branch
#define OP_BR 00004
#define OP_BNE 00010
#define OP_BEQ 00014
#define OP_BPL 01000
#define OP_BHI 01010
#define OP_BVC 01020
#define OP_BVS 01024
#define OP_BCC 01030
#define OP_BCS 01034

//Signed Conditional Branches
#define OP_BGE 00020
#define OP_BLT 00024
#define OP_BGT 00030
#define OP_BLE 00034

//Unsigned Conditional Branches
#define OP_BMI 01004
#define OP_BLOS 01014
//#define OP_BHIS 01030
//#define OP_BLO 01034

//Jump
#define OP_JMP 00001 // bug same code as mov

//Misc.
#define OP_HALT 0000000


//Definitions for various modes
#define GEN_MODE_REGISTER 0
#define GEN_MODE_REGISTER_DEFERRED 1
#define GEN_MODE_AUTO_INCREMENT 2
#define GEN_MODE_AUTO_INCREMENT_DEFERRED 3
#define GEN_MODE_AUTO_DECREMENT 4
#define GEN_MODE_AUTO_DECREMENT_DEFERRED 5
#define GEN_MODE_INDEX 6
#define GEN_MODE_INDEX_DEFERRED 7

#define PC_MODE_IMMEDIATE 2
#define PC_MODE_ABSOLUTE 3
#define PC_MODE_RELATIVE 6
#define PC_MODE_RELATIVE_DEFERRED 7

