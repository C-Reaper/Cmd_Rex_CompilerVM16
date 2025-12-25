#include "/home/codeleaded/System/Static/Library/AlxCallStack.h"
#include "/home/codeleaded/System/Static/Library/AlxExternFunctions.h"
#include "/home/codeleaded/System/Static/Library/RexLang.h"
#include "/home/codeleaded/System/Static/Library/RexLangASM.h"

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

        const int offset_a = ll->stack - va_s->stack;
        const int offset_b = ll->stack - vb_s->stack;

        CStr location_a = CStr_Format("[%s + %d",RexLang_REG_SP_64,offset_a);
        CStr location_b = CStr_Format("[%s + %d",RexLang_REG_SP_64,offset_b);
        
        if(RexLang_DrefType(ll,va->typename)){
            RexLang_DrefIntoReg(ll,a,RexLang_REG_D_64);
            
            CStr setter = CStr_Format("[%s",RexLang_REG_D_64);
            CStr_Set(&location_a,setter);
            CStr_Free(&setter);
        }
        if(RexLang_DrefType(ll,vb->typename)){
            RexLang_DrefIntoReg(ll,b,RexLang_REG_C_64);
            
            CStr setter = CStr_Format("[%s",RexLang_REG_C_64);
            CStr_Set(&location_b,setter);
            CStr_Free(&setter);
        }

        int i = 0;
        for(;i<=realsize-8;i+=8){
            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s%s + %d]",RexLang_REG_A_64,RexLang_DREF_64,location_b,i);
            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s%s + %d],%s",RexLang_DREF_64,location_a,i,RexLang_REG_A_64);
        }
        for(;i<=realsize-4;i+=4){
            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s%s + %d]",RexLang_REG_A_32,RexLang_DREF_32,location_b,i);
            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s%s + %d],%s",RexLang_DREF_32,location_a,i,RexLang_REG_A_32);
        }
        for(;i<=realsize-2;i+=2){
            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s%s + %d]",RexLang_REG_A_16,RexLang_DREF_16,location_b,i);
            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s%s + %d],%s",RexLang_DREF_16,location_a,i,RexLang_REG_A_16);
        }
        for(;i<=realsize-1;i+=1){
            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s%s + %d]",RexLang_REG_A_L8,RexLang_DREF_8,location_b,i);
            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s%s + %d],%s",RexLang_DREF_8,location_a,i,RexLang_REG_A_L8);
        }

        CStr_Free(&location_b);
        CStr_Free(&location_a);
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
        RexLangVariable* va = (RexLangVariable*)Variable_Data(v);
        CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);

        if(RexLang_DrefType(ll,v->typename)){
            CStr type = CStr_Cpy(v->typename);
            type[CStr_Size(type) - 1] = '*';
            RexLang_Variable_Build_Decl(ll,stack_name,type);
            CStr_Free(&type);

            CStr location_a = RexLang_StackAtNT(ll,va->stack);
            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s",RexLang_REG_A_64,location_a);
            RexLang_IntoSet(ll,&stack_t,RexLang_REG_A_64);
        }else{
            CStr type = CStr_Concat(v->typename,"*");
            RexLang_Variable_Build_Decl(ll,stack_name,type);
            CStr_Free(&type);

            CStr location_a = RexLang_StackAtNT(ll,va->stack);
            RexLang_Indentation_Appendf(ll,&ll->text,"lea %s,%s",RexLang_REG_A_64,location_a);
            RexLang_IntoSet(ll,&stack_t,RexLang_REG_A_64);
        }
        
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

                        if(!RexLang_DrefType(ll,v->typename)){
                            RexLang_Variable_Build_Use(ll,stack_name,member->type,sv->stack - offset);
                        }else{
                            CStr type = CStr_Concat(member->type,"&");
                            RexLang_Variable_Build_Decl(ll,stack_name,type);
                            CStr_Free(&type);

                            CStr location_a = RexLang_Location(ll,a->str);
                            CStr location_s = RexLang_Location(ll,stack_name);

                            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s",RexLang_REG_A_64,location_a);
                            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s",location_s,RexLang_REG_A_64);
                            RexLang_Indentation_Appendf(ll,&ll->text,"add %s,%d",location_s,offset);

                            CStr_Free(&location_a);
                            CStr_Free(&location_s);
                        }

                        return Token_Move(TOKEN_STRING,stack_name);
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
