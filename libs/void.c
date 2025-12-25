#include "/home/codeleaded/System/Static/Library/AlxCallStack.h"
#include "/home/codeleaded/System/Static/Library/AlxExternFunctions.h"
#include "/home/codeleaded/System/Static/Library/RexLang.h"
#include "/home/codeleaded/System/Static/Library/RexLangASM.h"

Token Void_Null_Handler_Cast(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("[Void]: CAST: %s\n",a->str);

    String ret = String_New();

    Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
    if(v){
        RexLangVariable* sv = (RexLangVariable*)Variable_Data(v);
        String_Appendf(&ret,"S:%d [&:%d,%d]",sv->stack,sv->destroy,sv->sizeonstack);
    }else{
        Environment_ErrorHandler(&ll->ev,"Cast -> Error!");
        String_Append(&ret,"ERROR");
    }

    CStr out = String_CStr(&ret);
    String_Free(&ret);
    return Token_Move(TOKEN_CONSTSTRING_DOUBLE,out);
}

Token Void_Handler_Cast(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    if(op->str==NULL) return Void_Null_Handler_Cast(ll,op,args);
    return Token_Null();
}
Token Void_Handler_Size(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    //printf("[Void]: SIZE: %s\n",a->str);
    return Token_Move(TOKEN_NUMBER,Number_Get(VOID_SIZE));
}

void Ex_Packer(ExternFunctionMap* Extern_Functions,Vector* funcs,Scope* s){//Vector<CStr>
    TypeMap_PushContained(&s->types,funcs,
        Type_Make(VOID_TYPE,sizeof(RexLangVariable),OperatorInterationMap_Make((OperatorInterater[]){
            OperatorInterater_Make((CStr[]){ NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_CAST,(Token(*)(void*,Token*,Vector*))Void_Handler_Cast),
                OperatorDefiner_New(TOKEN_INIT,(Token(*)(void*,Token*,Vector*))RexLang_Init),
                OperatorDefiner_New(TOKEN_REXLANG_SIZE,(Token(*)(void*,Token*,Vector*))Void_Handler_Size),
                //OperatorDefiner_New(TOKEN_DESTROY,NULL),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ VOID_TYPE,NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                //OperatorDefiner_New(TOKEN_REXLANG_ASS,(Token(*)(void*,Token*,Vector*))Void_Void_Handler_Ass),
                OPERATORDEFINER_END
            })),
            OPERATORINTERATER_END
        }),RexLang_Destroyer,RexLang_Cpyer)
    );
}