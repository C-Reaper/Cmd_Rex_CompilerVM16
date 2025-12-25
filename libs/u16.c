#include "/home/codeleaded/System/Static/Library/AlxCallStack.h"
#include "/home/codeleaded/System/Static/Library/AlxExternFunctions.h"
#include "../src/RexLang.h"
#include "../src/RexLangASM.h"

Token U16_U16_Handler_Ass(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_ExecuteAss(ll,a,b,op,"mov","ASS");
}

Token U16_U16_Handler_Asd(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_ExecuteAss(ll,a,b,op,"add","ASD");
}
Token U16_U16_Handler_Asu(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_ExecuteAss(ll,a,b,op,"sub","ASU");
}
Token U16_U16_Handler_Asm(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_ExecuteAss(ll,a,b,op,"imul","ASM");
}
Token U16_U16_Handler_Asv(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_ExecuteAss(ll,a,b,op,"idiv","ASV");
}

Token U16_U16_Handler_Add(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_Execute(ll,a,b,op,"add","ADD",RexLang_Function_Add);
}
Token U16_U16_Handler_Sub(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_Execute(ll,a,b,op,"sub","SUB",RexLang_Function_Sub);
}
Token U16_U16_Handler_Mul(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_Execute(ll,a,b,op,"imul","IMUL",RexLang_Function_Mul);
}
Token U16_U16_Handler_Div(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_Execute(ll,a,b,op,"idiv","IDIV",RexLang_Function_Div);
}

Token U16_U16_Handler_Mod(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_Execute(ll,a,b,op,"imod","IMOD",RexLang_Function_Mod);
}

Token U16_Handler_Neg(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    return RexLang_ExecuteSingle(ll,a,op,"neg","NEG",RexLang_Function_Neg);
}
Token U16_Handler_Inc(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    return RexLang_ExecuteAssSingle(ll,a,op,"inc","INC");
}
Token U16_Handler_Dec(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    return RexLang_ExecuteAssSingle(ll,a,op,"dec","DEC");
}

Token U16_U16_Handler_And(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_Execute(ll,a,b,op,"and","AND",RexLang_Function_And);
}
Token U16_U16_Handler_Or(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_Execute(ll,a,b,op,"or","OR",RexLang_Function_Or);
}
Token U16_U16_Handler_Xor(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_Execute(ll,a,b,op,"xor","XOR",RexLang_Function_Xor);
}
Token U16_Handler_Not(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    return RexLang_ExecuteSingle(ll,a,op,"not","NOT",RexLang_Function_Not);
}
Token U16_U16_Handler_Equ(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //return RexLang_ExecuteCmp(ll,a,b,op,"sete","EQU",RexLang_Function_Equ);
    return RexLang_ExecuteJmp(ll,a,b,op,"jz","EQU",RexLang_Function_Equ);
}
Token U16_U16_Handler_Neq(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //return RexLang_ExecuteCmp(ll,a,b,op,"setne","NEQ",RexLang_Function_Neq);
    return RexLang_ExecuteJmp(ll,a,b,op,"jnz","NEQ",RexLang_Function_Neq);
}
Token U16_U16_Handler_Les(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //return RexLang_ExecuteCmp(ll,a,b,op,"setl","LES",RexLang_Function_Les);
    return RexLang_ExecuteJmp(ll,a,b,op,"jn","LES",RexLang_Function_Les);
}
Token U16_U16_Handler_Grt(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //return RexLang_ExecuteCmp(ll,a,b,op,"setg","GRT",RexLang_Function_Grt);
    return RexLang_ExecuteJmp(ll,a,b,op,"jp","GRT",RexLang_Function_Grt);
}
Token U16_U16_Handler_Leq(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //return RexLang_ExecuteCmp(ll,a,b,op,"setle","LEQ",RexLang_Function_Leq);
    return RexLang_ExecuteJmp(ll,a,b,op,"jzn","LEQ",RexLang_Function_Leq);
}
Token U16_U16_Handler_Grq(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //return RexLang_ExecuteCmp(ll,a,b,op,"setge","GRQ",RexLang_Function_Grq);
    return RexLang_ExecuteJmp(ll,a,b,op,"jzp","GRQ",RexLang_Function_Grq);
}

Token U16_Handler_Adr(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("[U16]: ADR: &%s\n",a->str);
    
    if(a->tt==TOKEN_NUMBER){
        Environment_ErrorHandler(&ll->ev,"Adr -> Error: can not get address of const: %s!",a->str);
        return Token_Null();
    }else if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        RexLang_AddressReg(ll,a,RexLang_REG_A);
        RexLang_IntoSet(ll,&stack_t,RexLang_REG_A);
        return stack_t;
    }else{
        Environment_ErrorHandler(&ll->ev,"Adr: Error -> %s has no address!",a->str);
        return Token_Null();
    }
}

Token U16_Null_Handler_Cast(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("[U16]: CAST: %s\n",a->str);

    String ret = String_New();

    if(a->tt==TOKEN_NUMBER){
        String_AppendNumber(&ret,Number_Parse(a->str));
    }else{
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            RexLangVariable* sv = (RexLangVariable*)Variable_Data(v);
            String_Appendf(&ret,"T: %s,S:%d [&:%d,%d]",v->typename,sv->stack,sv->destroy,sv->sizeonstack);
        }else{
            Environment_ErrorHandler(&ll->ev,"Cast -> Error!");
            String_Append(&ret,"ERROR");
        }
    }

    CStr out = String_CStr(&ret);
    String_Free(&ret);
    return Token_Move(TOKEN_CONSTSTRING_DOUBLE,out);
}
Token U16_Handler_Cast(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    if(op->str==NULL)
        return U16_Null_Handler_Cast(ll,op,args);
    if(CStr_Cmp(op->str,U16_TYPE) || CStr_Cmp(op->str,I16_TYPE) ||
       CStr_Cmp(op->str,U8_TYPE) || CStr_Cmp(op->str,U16_TYPE) ||
       RexLang_PointerType(ll,op->str))
        return Int_Int_Handler_Cast(ll,op,args,op->str);
    return Token_Null();
}
Token U16_Handler_Size(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    //printf("[U16]: SIZE: %s\n",a->str);
    return Token_Move(TOKEN_NUMBER,Number_Get(U16_SIZE));
}

void Ex_Packer(ExternFunctionMap* Extern_Functions,Vector* funcs,Scope* s){//Vector<CStr>
    TypeMap_PushContained(&s->types,funcs,
        Type_Make(U16_TYPE,sizeof(RexLangVariable),OperatorInterationMap_Make((OperatorInterater[]){
            OperatorInterater_Make((CStr[]){ NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_NEG,(Token(*)(void*,Token*,Vector*))U16_Handler_Neg),
                OperatorDefiner_New(TOKEN_REXLANG_INC,(Token(*)(void*,Token*,Vector*))U16_Handler_Inc),
                OperatorDefiner_New(TOKEN_REXLANG_DEC,(Token(*)(void*,Token*,Vector*))U16_Handler_Dec),
                OperatorDefiner_New(TOKEN_REXLANG_NOT,(Token(*)(void*,Token*,Vector*))U16_Handler_Not),
                OperatorDefiner_New(TOKEN_REXLANG_ADR,(Token(*)(void*,Token*,Vector*))U16_Handler_Adr),
                OperatorDefiner_New(TOKEN_CAST,(Token(*)(void*,Token*,Vector*))U16_Handler_Cast),
                OperatorDefiner_New(TOKEN_INIT,(Token(*)(void*,Token*,Vector*))RexLang_Init),
                OperatorDefiner_New(TOKEN_REXLANG_SIZE,(Token(*)(void*,Token*,Vector*))U16_Handler_Size),
                //OperatorDefiner_New(TOKEN_DESTROY,NULL),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ I8_TYPE,NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ASS,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Ass),
                OperatorDefiner_New(TOKEN_REXLANG_ASD,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asd),
                OperatorDefiner_New(TOKEN_REXLANG_ASU,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asu),
                OperatorDefiner_New(TOKEN_REXLANG_ASM,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asm),
                OperatorDefiner_New(TOKEN_REXLANG_ASV,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asv),
                OperatorDefiner_New(TOKEN_REXLANG_ADD,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Add),
                OperatorDefiner_New(TOKEN_REXLANG_SUB,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Sub),
                OperatorDefiner_New(TOKEN_REXLANG_MUL,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Mul),
                OperatorDefiner_New(TOKEN_REXLANG_DIV,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Div),
                OperatorDefiner_New(TOKEN_REXLANG_MOD,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Mod),
                OperatorDefiner_New(TOKEN_REXLANG_AND,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_And),
                OperatorDefiner_New(TOKEN_REXLANG_OR,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Or),
                OperatorDefiner_New(TOKEN_REXLANG_XOR,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Xor),
                OperatorDefiner_New(TOKEN_REXLANG_EQU,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Equ),
                OperatorDefiner_New(TOKEN_REXLANG_NEQ,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Neq),
                OperatorDefiner_New(TOKEN_REXLANG_LES,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Les),
                OperatorDefiner_New(TOKEN_REXLANG_GRT,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Grt),
                OperatorDefiner_New(TOKEN_REXLANG_LEQ,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Leq),
                OperatorDefiner_New(TOKEN_REXLANG_GRQ,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Grq),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ I16_TYPE,NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ASS,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Ass),
                OperatorDefiner_New(TOKEN_REXLANG_ASD,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asd),
                OperatorDefiner_New(TOKEN_REXLANG_ASU,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asu),
                OperatorDefiner_New(TOKEN_REXLANG_ASM,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asm),
                OperatorDefiner_New(TOKEN_REXLANG_ASV,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asv),
                OperatorDefiner_New(TOKEN_REXLANG_ADD,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Add),
                OperatorDefiner_New(TOKEN_REXLANG_SUB,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Sub),
                OperatorDefiner_New(TOKEN_REXLANG_MUL,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Mul),
                OperatorDefiner_New(TOKEN_REXLANG_DIV,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Div),
                OperatorDefiner_New(TOKEN_REXLANG_MOD,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Mod),
                OperatorDefiner_New(TOKEN_REXLANG_AND,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_And),
                OperatorDefiner_New(TOKEN_REXLANG_OR,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Or),
                OperatorDefiner_New(TOKEN_REXLANG_XOR,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Xor),
                OperatorDefiner_New(TOKEN_REXLANG_EQU,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Equ),
                OperatorDefiner_New(TOKEN_REXLANG_NEQ,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Neq),
                OperatorDefiner_New(TOKEN_REXLANG_LES,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Les),
                OperatorDefiner_New(TOKEN_REXLANG_GRT,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Grt),
                OperatorDefiner_New(TOKEN_REXLANG_LEQ,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Leq),
                OperatorDefiner_New(TOKEN_REXLANG_GRQ,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Grq),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ U8_TYPE,NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ASS,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Ass),
                OperatorDefiner_New(TOKEN_REXLANG_ASD,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asd),
                OperatorDefiner_New(TOKEN_REXLANG_ASU,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asu),
                OperatorDefiner_New(TOKEN_REXLANG_ASM,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asm),
                OperatorDefiner_New(TOKEN_REXLANG_ASV,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asv),
                OperatorDefiner_New(TOKEN_REXLANG_ADD,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Add),
                OperatorDefiner_New(TOKEN_REXLANG_SUB,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Sub),
                OperatorDefiner_New(TOKEN_REXLANG_MUL,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Mul),
                OperatorDefiner_New(TOKEN_REXLANG_DIV,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Div),
                OperatorDefiner_New(TOKEN_REXLANG_MOD,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Mod),
                OperatorDefiner_New(TOKEN_REXLANG_AND,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_And),
                OperatorDefiner_New(TOKEN_REXLANG_OR,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Or),
                OperatorDefiner_New(TOKEN_REXLANG_XOR,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Xor),
                OperatorDefiner_New(TOKEN_REXLANG_EQU,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Equ),
                OperatorDefiner_New(TOKEN_REXLANG_NEQ,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Neq),
                OperatorDefiner_New(TOKEN_REXLANG_LES,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Les),
                OperatorDefiner_New(TOKEN_REXLANG_GRT,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Grt),
                OperatorDefiner_New(TOKEN_REXLANG_LEQ,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Leq),
                OperatorDefiner_New(TOKEN_REXLANG_GRQ,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Grq),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ U16_TYPE,NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ASS,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Ass),
                OperatorDefiner_New(TOKEN_REXLANG_ASD,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asd),
                OperatorDefiner_New(TOKEN_REXLANG_ASU,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asu),
                OperatorDefiner_New(TOKEN_REXLANG_ASM,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asm),
                OperatorDefiner_New(TOKEN_REXLANG_ASV,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Asv),
                OperatorDefiner_New(TOKEN_REXLANG_ADD,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Add),
                OperatorDefiner_New(TOKEN_REXLANG_SUB,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Sub),
                OperatorDefiner_New(TOKEN_REXLANG_MUL,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Mul),
                OperatorDefiner_New(TOKEN_REXLANG_DIV,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Div),
                OperatorDefiner_New(TOKEN_REXLANG_MOD,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Mod),
                OperatorDefiner_New(TOKEN_REXLANG_AND,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_And),
                OperatorDefiner_New(TOKEN_REXLANG_OR,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Or),
                OperatorDefiner_New(TOKEN_REXLANG_XOR,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Xor),
                OperatorDefiner_New(TOKEN_REXLANG_EQU,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Equ),
                OperatorDefiner_New(TOKEN_REXLANG_NEQ,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Neq),
                OperatorDefiner_New(TOKEN_REXLANG_LES,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Les),
                OperatorDefiner_New(TOKEN_REXLANG_GRT,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Grt),
                OperatorDefiner_New(TOKEN_REXLANG_LEQ,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Leq),
                OperatorDefiner_New(TOKEN_REXLANG_GRQ,(Token(*)(void*,Token*,Vector*))U16_U16_Handler_Grq),
                OPERATORDEFINER_END
            })),
            OPERATORINTERATER_END
        }),RexLang_Destroyer,RexLang_Cpyer)
    );
}