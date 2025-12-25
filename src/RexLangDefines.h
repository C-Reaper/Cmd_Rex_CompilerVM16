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

#define RexLang_ACCESS_NONE                0b0
#define RexLang_ACCESS_PUB                 0b1
#define RexLang_ACCESS_GLOBAL              0b10

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

#define RexLang_REG_A                      "r0"
#define RexLang_REG_B                      "r1"
#define RexLang_REG_C                      "r2"
#define RexLang_REG_D                      "r3"
#define RexLang_REG_SP                     "sp"
#define RexLang_REG_PC                     "pc"

#define RexLang_REG_I_A                    0
#define RexLang_REG_I_B                    1
#define RexLang_REG_I_C                    2
#define RexLang_REG_I_D                    3
#define RexLang_REG_I_SP                   4
#define RexLang_REG_I_PC                   5

char* RexLang_RT[] = { RexLang_REG_A,RexLang_REG_B,RexLang_REG_C,RexLang_REG_D,RexLang_REG_SP,RexLang_REG_PC };

#define BOOL_TYPE                           "bool"
#define I8_TYPE                             "i8"
#define I16_TYPE                            "i16"
#define U8_TYPE                             "u8"
#define U16_TYPE                            "u16"
#define VOID_TYPE                           "void"
#define NULL_TYPE                           VOID_TYPE"*"
#define STR_TYPE                            "u8*"
#define CHAR_TYPE                           U8_TYPE
#define POINTER_TYPE                        "?*"
#define DREF_TYPE                           "!&"
#define STRUCT_TYPE                         ".struct"

#define BOOL_SIZE                           1
#define I8_SIZE                             1
#define I16_SIZE                            2
#define U8_SIZE                             1
#define U16_SIZE                            2
#define VOID_SIZE                           0
#define STR_SIZE                            8
#define CHAR_SIZE                           1
#define POINTER_SIZE                        2
#define DREF_SIZE                           2


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