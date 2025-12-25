#ifndef RexLangDEFINES_H
#define RexLangDEFINES_H

#include "/home/codeleaded/System/Static/Library/AlxParser.h"

#define TOKEN_REXLANG_LINECOMMENT        (TOKEN_START+0)
#define TOKEN_REXLANG_BLOCKCOMMENTSTART  (TOKEN_START+1)
#define TOKEN_REXLANG_BLOCKCOMMENTEND    (TOKEN_START+2)
#define TOKEN_REXLANG_RETURN             (TOKEN_START+3)
#define TOKEN_REXLANG_CONTINUE           (TOKEN_START+4)
#define TOKEN_REXLANG_BREAK              (TOKEN_START+5)
#define TOKEN_REXLANG_END                (TOKEN_START+6)
#define TOKEN_REXLANG_IF                 (TOKEN_START+7)
#define TOKEN_REXLANG_ELIF               (TOKEN_START+8)
#define TOKEN_REXLANG_ELSE               (TOKEN_START+9)
#define TOKEN_REXLANG_WHILE              (TOKEN_START+10)
#define TOKEN_REXLANG_FOR                (TOKEN_START+11)
#define TOKEN_REXLANG_IMPORT             (TOKEN_START+12)
#define TOKEN_REXLANG_CONST              (TOKEN_START+13)
#define TOKEN_REXLANG_STRUCT             (TOKEN_START+14)
#define TOKEN_REXLANG_IMPL               (TOKEN_START+15)
#define TOKEN_REXLANG_NAMESPACE          (TOKEN_START+16)
#define TOKEN_REXLANG_BOOLEAN            (TOKEN_START+17)
#define TOKEN_REXLANG_ASS                (TOKEN_START+18)
#define TOKEN_REXLANG_ADD                (TOKEN_START+19)
#define TOKEN_REXLANG_SUB                (TOKEN_START+20)
#define TOKEN_REXLANG_MUL                (TOKEN_START+21)
#define TOKEN_REXLANG_DIV                (TOKEN_START+22)
#define TOKEN_REXLANG_NEG                (TOKEN_START+23)
#define TOKEN_REXLANG_DRF                (TOKEN_START+24)
#define TOKEN_REXLANG_ADR                (TOKEN_START+25)
#define TOKEN_REXLANG_AND                (TOKEN_START+26)
#define TOKEN_REXLANG_OR                 (TOKEN_START+27)
#define TOKEN_REXLANG_XOR                (TOKEN_START+28)
#define TOKEN_REXLANG_NOT                (TOKEN_START+29)
#define TOKEN_REXLANG_LND                (TOKEN_START+30)
#define TOKEN_REXLANG_LOR                (TOKEN_START+31)
#define TOKEN_REXLANG_LOT                (TOKEN_START+32)
#define TOKEN_REXLANG_NEQ                (TOKEN_START+33)
#define TOKEN_REXLANG_EQU                (TOKEN_START+34)
#define TOKEN_REXLANG_LES                (TOKEN_START+35)
#define TOKEN_REXLANG_GRT                (TOKEN_START+36)
#define TOKEN_REXLANG_LEQ                (TOKEN_START+37)
#define TOKEN_REXLANG_GRQ                (TOKEN_START+38)
#define TOKEN_REXLANG_ACS                (TOKEN_START+39)
#define TOKEN_REXLANG_ARW                (TOKEN_START+40)
#define TOKEN_REXLANG_DDOT               (TOKEN_START+41)
#define TOKEN_REXLANG_DDDOT              (TOKEN_START+42)
#define TOKEN_REXLANG_SUBS               (TOKEN_START+43)
#define TOKEN_REXLANG_SUBSR              (TOKEN_START+44)
#define TOKEN_REXLANG_ASD                (TOKEN_START+45)
#define TOKEN_REXLANG_ASU                (TOKEN_START+46)
#define TOKEN_REXLANG_ASM                (TOKEN_START+47)
#define TOKEN_REXLANG_ASV                (TOKEN_START+48)
#define TOKEN_REXLANG_MOD                (TOKEN_START+49)
#define TOKEN_REXLANG_INC                (TOKEN_START+50)
#define TOKEN_REXLANG_DEC                (TOKEN_START+51)
#define TOKEN_REXLANG_CAL                (TOKEN_START+52)
#define TOKEN_REXLANG_ASMBY              (TOKEN_START+53)
#define TOKEN_REXLANG_REG                (TOKEN_START+54)
#define TOKEN_REXLANG_SIZE               (TOKEN_START+55)
#define TOKEN_REXLANG_PUB                (TOKEN_START+56)
#define TOKEN_REXLANG_DEFINE             (TOKEN_START+57)
#define TOKEN_REXLANG_SHL                (TOKEN_START+58)
#define TOKEN_REXLANG_SHR                (TOKEN_START+59)
#define TOKEN_REXLANG_NULL               (TOKEN_START+60)


#define RexLang_TYPE                       "rex"
#define RexLang_INDENTATION                "    "
#define RexLang_INDENTATION_NONE           0
#define RexLang_INDENTATION_FUNCTION       1

#define RexLang_TYPE_NONE                  "void"
#define RexLang_SELF                       "self"
#define RexLang_BOOL                       ".BOOL"
#define RexLang_IF                         "IF"
#define RexLang_ELIF                       "ELIF"
#define RexLang_ELSE                       "ELSE"
#define RexLang_WHILE                      "WHILE"
#define RexLang_FOR                        "FOR"
#define RexLang_LOG                        "LOG"
#define RexLang_LOG_TRUE                   "TRUE"
#define RexLang_LOG_END                    "END"

#define RexLang_DREF_8                     "BYTE"
#define RexLang_DREF_16                    "WORD"
#define RexLang_DREF_32                    "DWORD"
#define RexLang_DREF_64                    "QWORD"

#define RexLang_REG_A_L8                   "al"
#define RexLang_REG_B_L8                   "bl"
#define RexLang_REG_C_L8                   "cl"
#define RexLang_REG_D_L8                   "dl"
#define RexLang_REG_A_H8                   "ah"
#define RexLang_REG_B_H8                   "bh"
#define RexLang_REG_C_H8                   "ch"
#define RexLang_REG_D_H8                   "dh"
#define RexLang_REG_SI_8                   "sil"
#define RexLang_REG_DI_8                   "dil"
#define RexLang_REG_SP_8                   "spl"
#define RexLang_REG_BP_8                   "bpl"
#define RexLang_REG_8_8                    "r8b"
#define RexLang_REG_9_8                    "r9b"
#define RexLang_REG_10_8                   "r10b"
#define RexLang_REG_11_8                   "r11b"
#define RexLang_REG_12_8                   "r12b"
#define RexLang_REG_13_8                   "r13b"
#define RexLang_REG_14_8                   "r14b"
#define RexLang_REG_15_8                   "r15b"

#define RexLang_REG_A_16                   "ax"
#define RexLang_REG_B_16                   "bx"
#define RexLang_REG_C_16                   "cx"
#define RexLang_REG_D_16                   "dx"
#define RexLang_REG_SI_16                  "si"
#define RexLang_REG_DI_16                  "di"
#define RexLang_REG_SP_16                  "sp"
#define RexLang_REG_BP_16                  "bp"
#define RexLang_REG_8_16                   "r8w"
#define RexLang_REG_9_16                   "r9w"
#define RexLang_REG_10_16                  "r10w"
#define RexLang_REG_11_16                  "r11w"
#define RexLang_REG_12_16                  "r12w"
#define RexLang_REG_13_16                  "r13w"
#define RexLang_REG_14_16                  "r14w"
#define RexLang_REG_15_16                  "r15w"

#define RexLang_REG_A_32                   "eax"
#define RexLang_REG_B_32                   "ebx"
#define RexLang_REG_C_32                   "ecx"
#define RexLang_REG_D_32                   "edx"
#define RexLang_REG_SI_32                  "esi"
#define RexLang_REG_DI_32                  "edi"
#define RexLang_REG_SP_32                  "esp"
#define RexLang_REG_BP_32                  "ebp"
#define RexLang_REG_8_32                   "r8d"
#define RexLang_REG_9_32                   "r9d"
#define RexLang_REG_10_32                  "r10d"
#define RexLang_REG_11_32                  "r11d"
#define RexLang_REG_12_32                  "r12d"
#define RexLang_REG_13_32                  "r13d"
#define RexLang_REG_14_32                  "r14d"
#define RexLang_REG_15_32                  "r15d"

#define RexLang_REG_A_64                   "rax"
#define RexLang_REG_B_64                   "rbx"
#define RexLang_REG_C_64                   "rcx"
#define RexLang_REG_D_64                   "rdx"
#define RexLang_REG_SI_64                  "rsi"
#define RexLang_REG_DI_64                  "rdi"
#define RexLang_REG_SP_64                  "rsp"
#define RexLang_REG_BP_64                  "rbp"
#define RexLang_REG_8_64                   "r8"
#define RexLang_REG_9_64                   "r9"
#define RexLang_REG_10_64                  "r10"
#define RexLang_REG_11_64                  "r11"
#define RexLang_REG_12_64                  "r12"
#define RexLang_REG_13_64                  "r13"
#define RexLang_REG_14_64                  "r14"
#define RexLang_REG_15_64                  "r15"

#define RexLang_REG_A                      0
#define RexLang_REG_B                      1
#define RexLang_REG_C                      2
#define RexLang_REG_D                      3
#define RexLang_REG_SI                     4
#define RexLang_REG_DI                     5
#define RexLang_REG_SP                     6
#define RexLang_REG_BP                     7
#define RexLang_REG_IP                     8
#define RexLang_REG_8                      9
#define RexLang_REG_9                      10
#define RexLang_REG_10                     11
#define RexLang_REG_11                     12
#define RexLang_REG_12                     13
#define RexLang_REG_13                     14
#define RexLang_REG_14                     15
#define RexLang_REG_15                     16

char* RexLang_RT_8[] = {
    RexLang_REG_A_L8,  RexLang_REG_B_L8,  RexLang_REG_C_L8,  RexLang_REG_D_L8,
    RexLang_REG_SI_8,  RexLang_REG_DI_8,  RexLang_REG_SP_8,  RexLang_REG_BP_8,
    RexLang_REG_8_8,   RexLang_REG_9_8,   RexLang_REG_10_8,  RexLang_REG_11_8,
    RexLang_REG_12_8,  RexLang_REG_13_8,  RexLang_REG_14_8,  RexLang_REG_15_8
};
char* RexLang_RT_16[] = {
    RexLang_REG_A_16,  RexLang_REG_B_16,  RexLang_REG_C_16,  RexLang_REG_D_16,
    RexLang_REG_SI_16, RexLang_REG_DI_16, RexLang_REG_SP_16, RexLang_REG_BP_16,
    RexLang_REG_8_16,  RexLang_REG_9_16,  RexLang_REG_10_16, RexLang_REG_11_16,
    RexLang_REG_12_16, RexLang_REG_13_16, RexLang_REG_14_16, RexLang_REG_15_16
};
char* RexLang_RT_32[] = {
    RexLang_REG_A_32,  RexLang_REG_B_32,  RexLang_REG_C_32,  RexLang_REG_D_32,
    RexLang_REG_SI_32, RexLang_REG_DI_32, RexLang_REG_SP_32, RexLang_REG_BP_32,
    RexLang_REG_8_32,  RexLang_REG_9_32,  RexLang_REG_10_32, RexLang_REG_11_32,
    RexLang_REG_12_32, RexLang_REG_13_32, RexLang_REG_14_32, RexLang_REG_15_32
};
char* RexLang_RT_64[] = {
    RexLang_REG_A_64,  RexLang_REG_B_64,  RexLang_REG_C_64,  RexLang_REG_D_64,
    RexLang_REG_SI_64, RexLang_REG_DI_64, RexLang_REG_SP_64, RexLang_REG_BP_64,
    RexLang_REG_8_64,  RexLang_REG_9_64,  RexLang_REG_10_64, RexLang_REG_11_64,
    RexLang_REG_12_64, RexLang_REG_13_64, RexLang_REG_14_64, RexLang_REG_15_64
};

#define BOOL_TYPE                           "bool"
#define I8_TYPE                             "i8"
#define I16_TYPE                            "i16"
#define I32_TYPE                            "i32"
#define I64_TYPE                            "i64"
#define U8_TYPE                             "u8"
#define U16_TYPE                            "u16"
#define U32_TYPE                            "u32"
#define U64_TYPE                            "u64"
#define F32_TYPE                            "f32"
#define F64_TYPE                            "f64"
#define VOID_TYPE                           "void"
#define NULL_TYPE                           VOID_TYPE"*"
#define STR_TYPE                            "i8*"
#define CHAR_TYPE                           I32_TYPE
#define POINTER_TYPE                        "?*"
#define DREF_TYPE                           "!&"
#define STRUCT_TYPE                         ".struct"

#define BOOL_SIZE                           1
#define I8_SIZE                             1
#define I16_SIZE                            2
#define I32_SIZE                            4
#define I64_SIZE                            8
#define U8_SIZE                             1
#define U16_SIZE                            2
#define U32_SIZE                            4
#define U64_SIZE                            8
#define F32_SIZE                            4
#define F64_SIZE                            8
#define VOID_SIZE                           0
#define STR_SIZE                            8
#define CHAR_SIZE                           4
#define POINTER_SIZE                        8
#define DREF_SIZE                           8


typedef struct RexLangVariable{
    int stack;
    int sizeonstack;
    char destroy;
    CStr global;
    void* parent;
} RexLangVariable;

RexLangVariable RexLangVariable_New(int stack,int sizeonstack,char destroy,void* parent){
    RexLangVariable sv;
    sv.stack = stack;
    sv.sizeonstack = sizeonstack;
    sv.destroy = destroy;
    sv.global = NULL;
    sv.parent = parent;
    return sv;
}
void RexLangVariable_Free(RexLangVariable* sv){
    CStr_Free(&sv->global);
}

#endif //!RexLangDEFINES_H