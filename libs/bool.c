#if defined __linux__
#include "/home/codeleaded/System/Static/Library/AlxCallStack.h"
#include "/home/codeleaded/System/Static/Library/AlxExternFunctions.h"
#include "../src/RexLang.h"
#include "../src/RexLangASM.h"
#elif defined _WINE
#include "/home/codeleaded/System/Static/Library/AlxCallStack.h"
#include "/home/codeleaded/System/Static/Library/AlxExternFunctions.h"
#include "../src/RexLang.h"
#include "../src/RexLangASM.h"
#elif defined _WIN32
#include "F:/home/codeleaded/System/Static/Library/AlxCallStack.h"
#include "F:/home/codeleaded/System/Static/Library/AlxExternFunctions.h"
#include "../src/RexLang.h"
#include "../src/RexLangASM.h"
#endif

/*
-- Cmp operation: --
je true
mov c,0
jmp end
true:
mov c,1
end:

-- AND operation: --
jne false
jne false
true:
mov c,1
jmp end
false:
mov c,0
end:

-- OR operation: --
je true
jne false
true:
mov c,1
jmp end
false:
mov c,0
end:

-- Not operation: --
je false
true:
mov c,1
jmp end
false:
mov c,0
end:
*/

Token Bool_Bool_Handler_Ass(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_ExecuteAss(ll,a,b,op,"mov","ASS");
}
Token Bool_Bool_Handler_Lnd(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_Execute(ll,a,b,op,"and","LND",RexLang_Function_And);
}
Token Bool_Bool_Handler_Lor(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_Execute(ll,a,b,op,"or","LOR",RexLang_Function_Or);
}
Token Bool_Handler_Lot(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    return RexLang_ExecuteSingle(ll,a,op,"not","LOT",RexLang_Function_Not);
}
Token Bool_Bool_Handler_Equ(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //return RexLang_ExecuteCmp(ll,a,b,op,"sete","EQU",RexLang_Function_Equ);
    return RexLang_ExecuteJmp(ll,a,b,op,"jz","EQU",RexLang_Function_Equ);
}
Token Bool_Bool_Handler_Neq(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //return RexLang_ExecuteCmp(ll,a,b,op,"setne","NEQ",RexLang_Function_Neq);
    return RexLang_ExecuteJmp(ll,a,b,op,"jnz","NEQ",RexLang_Function_Neq);
}

Token Bool_Handler_Adr(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("[Bool]: ADR: &%s\n",a->str);
    
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

Token Bool_Null_Handler_Cast(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("[Bool]: CAST: %s\n",a->str);

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
Token Bool_Handler_Cast(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    if(op->str==NULL) return Bool_Null_Handler_Cast(ll,op,args);
    if(CStr_Cmp(op->str,I8_TYPE) || CStr_Cmp(op->str,I16_TYPE) ||
       CStr_Cmp(op->str,U8_TYPE) || CStr_Cmp(op->str,U16_TYPE))
        return Int_Int_Handler_Cast(ll,op,args,op->str);
    return Token_Null();
}
Token Bool_Handler_Size(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    //printf("[Bool]: SIZE: %s\n",a->str);
    return Token_Move(TOKEN_NUMBER,Number_Get(BOOL_SIZE));
}

void Ex_Packer(ExternFunctionMap* Extern_Functions,Vector* funcs,Scope* s){//Vector<CStr>
    TypeMap_PushContained(&s->types,funcs,
        Type_Make(BOOL_TYPE,sizeof(RexLangVariable),OperatorInterationMap_Make((OperatorInterater[]){
            OperatorInterater_Make((CStr[]){ NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_LOT, (Token(*)(void*,Token*,Vector*))Bool_Handler_Lot),
                OperatorDefiner_New(TOKEN_CAST,         (Token(*)(void*,Token*,Vector*))Bool_Handler_Cast),
                OperatorDefiner_New(TOKEN_INIT,         (Token(*)(void*,Token*,Vector*))RexLang_Init),
                OperatorDefiner_New(TOKEN_REXLANG_SIZE,(Token(*)(void*,Token*,Vector*))Bool_Handler_Size),
                //OperatorDefiner_New(TOKEN_DESTROY,NULL),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ BOOL_TYPE,NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ASS,(Token(*)(void*,Token*,Vector*))Bool_Bool_Handler_Ass),
                OperatorDefiner_New(TOKEN_REXLANG_LND,(Token(*)(void*,Token*,Vector*))Bool_Bool_Handler_Lnd),
                OperatorDefiner_New(TOKEN_REXLANG_LOR,(Token(*)(void*,Token*,Vector*))Bool_Bool_Handler_Lor),
                OperatorDefiner_New(TOKEN_REXLANG_EQU,(Token(*)(void*,Token*,Vector*))Bool_Bool_Handler_Equ),
                OperatorDefiner_New(TOKEN_REXLANG_NEQ,(Token(*)(void*,Token*,Vector*))Bool_Bool_Handler_Neq),
                OPERATORDEFINER_END
            })),
            OPERATORINTERATER_END
        }),RexLang_Destroyer,RexLang_Cpyer)
    );
}