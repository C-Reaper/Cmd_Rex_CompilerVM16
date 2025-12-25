#include "/home/codeleaded/System/Static/Library/AlxCallStack.h"
#include "/home/codeleaded/System/Static/Library/AlxExternFunctions.h"
#include "../src/RexLang.h"
#include "../src/RexLangASM.h"

Token Struct_Struct_Handler_Ass(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //printf("[Struct]: ASS: %s = %s\n",a->str,b->str);

    if(b->tt==TOKEN_STRING){
        Variable* va = Scope_FindVariable(&ll->ev.sc,a->str);
        RexLangVariable* va_s = (RexLangVariable*)Variable_Data(va);
        Variable* vb = Scope_FindVariable(&ll->ev.sc,b->str);
        RexLangVariable* vb_s = (RexLangVariable*)Variable_Data(vb);

        const int realsize = RexLang_TypeRealSize(ll,a);
        //int realsize = RexLang_TypeRealSize(ll,b);
        
        RexLang_AddressReg(ll,a,RexLang_REG_C);
        RexLang_AddressReg(ll,b,RexLang_REG_B);

        int i = 0;
        for(;i<=realsize-2;i+=2){
            RexLang_Indentation_Appendf(ll,&ll->text,"ld\t\t%s\t%s",RexLang_REG_A,RexLang_REG_B);
            RexLang_Indentation_Appendf(ll,&ll->text,"st\t\t%s\t%s",RexLang_REG_C,RexLang_REG_A);
            RexLang_Indentation_Appendf(ll,&ll->text,"add\t\t%s\t2",RexLang_REG_C);
            RexLang_Indentation_Appendf(ll,&ll->text,"add\t\t%s\t2",RexLang_REG_B);
        }
        for(;i<=realsize-1;i+=1){
            RexLang_Indentation_Appendf(ll,&ll->text,"ld1\t\t%s\t%s",RexLang_REG_A,RexLang_REG_B);
            RexLang_Indentation_Appendf(ll,&ll->text,"st1\t\t%s\t%s",RexLang_REG_C,RexLang_REG_A);
            RexLang_Indentation_Appendf(ll,&ll->text,"add1\t%s\t1",RexLang_REG_C);
            RexLang_Indentation_Appendf(ll,&ll->text,"add1\t%s\t1",RexLang_REG_B);
        }
    }else{
        Environment_ErrorHandler(&ll->ev,"Ass: Error -> %s is no struct type!",b->str);
        return Token_Null();
    }
    return Token_Cpy(a);
}
Token Struct_Handler_Adr(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    //printf("[Struct]: ADR: &%s\n",a->str);
    
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
        CStr type = NULL;

        if(RexLang_DrefType(ll,v->typename)){
            type = CStr_Cpy(v->typename);
            type[CStr_Size(type) - 1] = '*';
        }else{
            type = CStr_Concat(v->typename,"*");
        }

        RexLang_Variable_Build_Decl(ll,stack_name,type);
        CStr_Free(&type);

        RexLang_AddressReg(ll,a,RexLang_REG_A);
        RexLang_IntoSet(ll,&stack_t,RexLang_REG_A);
        return stack_t;
    }else{
        Environment_ErrorHandler(&ll->ev,"Adr: Error -> %s has no address!",a->str);
        return Token_Null();
    }
}
Token Struct_Handler_Acs(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //printf("[Struct]: ACS: %s.%s\n",a->str,b->str);
    
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            RexLangVariable* sv = (RexLangVariable*)Variable_Data(v);
            Type* t = Scope_FindType(&ll->ev.sc,v->typename);
            if(t){
                Member* member = MemberMap_Find(&t->related,b->str);
                if(member){
                    if(member->access || CStr_Cmp(a->str,RexLang_SELF)){
                        int offset = 0;
                        for(int i = 0;i<t->related.size;i++){
                            Member* m = (Member*)Vector_Get(&t->related,i);
                            if(CStr_Cmp(b->str,m->name)) break;
                            offset += RexLang_Size(ll,m->type);
                        }
                        
                        CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
                        Token stack_t = Token_Move(TOKEN_STRING,stack_name);

                        if(!RexLang_DrefType(ll,v->typename)){
                            CStr type = CStr_Concat(member->type,"&");
                            RexLang_Variable_Build_Decl(ll,stack_name,type);
                            CStr_Free(&type);

                            RexLang_AddressReg(ll,a,RexLang_REG_C);
                            RexLang_AddressReg(ll,b,RexLang_REG_B);
                            RexLang_Indentation_Appendf(ll,&ll->text,"ld\t\t%s\t%s",RexLang_REG_A,RexLang_REG_C);
                            RexLang_Indentation_Appendf(ll,&ll->text,"add\t\t%s\t%d",RexLang_REG_A,offset);
                            RexLang_Indentation_Appendf(ll,&ll->text,"st\t\t%s\t%s",RexLang_REG_B,RexLang_REG_A);
                        }else{
                            CStr type = CStr_Concat(member->type,"&");
                            RexLang_Variable_Build_Decl(ll,stack_name,type);
                            CStr_Free(&type);

                            RexLang_AddressReg(ll,a,RexLang_REG_C);
                            RexLang_AddressReg(ll,b,RexLang_REG_B);
                            RexLang_Indentation_Appendf(ll,&ll->text,"ld\t\t%s\t%s",RexLang_REG_A,RexLang_REG_C);
                            RexLang_Indentation_Appendf(ll,&ll->text,"ld\t\t%s\t%s",RexLang_REG_A,RexLang_REG_A);
                            RexLang_Indentation_Appendf(ll,&ll->text,"add\t\t%s\t%d",RexLang_REG_A,offset);
                            RexLang_Indentation_Appendf(ll,&ll->text,"st\t\t%s\t%s",RexLang_REG_B,RexLang_REG_A);
                        }
                        return stack_t;
                    }else{
                        Environment_ErrorHandler(&ll->ev,"Acs: Member %s isn't pub or non self %s tries to access!",b->str,a->str);
                        return Token_Null();
                    }
                }else{
                    Environment_ErrorHandler(&ll->ev,"Acs: Member %s not found!",b->str);
                    return Token_Null();
                }
            }else{
                Environment_ErrorHandler(&ll->ev,"Acs: Type %s not found!",v->typename);
                return Token_Null();
            }
        }else{
            Environment_ErrorHandler(&ll->ev,"Acs: 1. Arg: %s not found as a variable!",a->str);
            return Token_Null();
        }
    }else{
        Environment_ErrorHandler(&ll->ev,"Acs: 1. Arg: %s is not a variable type!",a->str);
        return Token_Null();
    }

    return Token_Null();
}

Token Struct_Null_Handler_Cast(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("[Struct]: CAST: %s\n",a->str);

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
Token Struct_Handler_Cast(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    if(op->str==NULL) return Struct_Null_Handler_Cast(ll,op,args);
    return Token_Null();
}
Token Struct_Handler_Size(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    //printf("[Struct]: SIZE: %s\n",a->str);

    Type* t = Scope_FindType(&ll->ev.sc,a->str);
    if(t){
        int size = 0;
        for(int i = 0;i<t->related.size;i++){
            Member* m = (Member*)Vector_Get(&t->related,i);
            size += RexLang_Size(ll,m->type);
        }
        return Token_Move(TOKEN_NUMBER,Number_Get(size));
    }
    return Token_Move(TOKEN_NUMBER,Number_Get(0));
}

void Ex_Packer(ExternFunctionMap* Extern_Functions,Vector* funcs,Scope* s){//Vector<CStr>
    TypeMap_PushContained(&s->types,funcs,
        Type_Make(STRUCT_TYPE,sizeof(RexLangVariable),OperatorInterationMap_Make((OperatorInterater[]){
            OperatorInterater_Make((CStr[]){ NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ADR,(Token(*)(void*,Token*,Vector*))Struct_Handler_Adr),
                OperatorDefiner_New(TOKEN_CAST,(Token(*)(void*,Token*,Vector*))Struct_Handler_Cast),
                OperatorDefiner_New(TOKEN_INIT,(Token(*)(void*,Token*,Vector*))RexLang_Init),
                OperatorDefiner_New(TOKEN_REXLANG_SIZE,(Token(*)(void*,Token*,Vector*))Struct_Handler_Size),
                //OperatorDefiner_New(TOKEN_DESTROY,NULL),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ STRUCT_TYPE,NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ASS,(Token(*)(void*,Token*,Vector*))Struct_Struct_Handler_Ass),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ OPERATORINTERATER_DONTCARE,NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ACS,(Token(*)(void*,Token*,Vector*))Struct_Handler_Acs),
                OPERATORDEFINER_END
            })),
            OPERATORINTERATER_END
        }),RexLang_Destroyer,RexLang_Cpyer)
    );
}
