#include "/home/codeleaded/System/Static/Library/AlxCallStack.h"
#include "/home/codeleaded/System/Static/Library/AlxExternFunctions.h"
#include "../src/RexLang.h"
#include "../src/RexLangASM.h"

Token Pointer_Pointer_Handler_Ass(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("[Pointer]: ASS: %s = %s\n",a->str,b->str);

    if(b->tt==TOKEN_NUMBER){
        RexLang_IntoSet(ll,a,b->str);
    }else if(b->tt==TOKEN_CONSTSTRING_DOUBLE){
        CStr name = RexLang_BuildConstStr(ll,b->str);
        RexLang_IntoSet(ll,a,name);
        CStr_Free(&name);
    }else if(b->tt==TOKEN_FUNCTIONPOINTER){
        CStr name = RexLang_FunctionName(ll,b->str);
        RexLang_IntoSet(ll,a,name);
        CStr_Free(&name);
    }else if(b->tt==TOKEN_REXLANG_NULL){
        RexLang_IntoSet(ll,a,"0");
    }else if(b->tt==TOKEN_STRING){
        int realsize_a = RexLang_TypeRealSize(ll,a);
        int realsize_b = RexLang_TypeRealSize(ll,b);
        RexLang_IntoReg(ll,b,RexLang_REG_A);
        RexLang_IntoSet(ll,a,RexLang_REG_A);
    }else{
        Environment_ErrorHandler(&ll->ev,"Ass: Error -> %s has no pointer type!",b->str);
        return Token_Null();
    }
    return Token_Cpy(a);
}
Token Pointer_Pointer_Handler_Add(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_Execute(ll,a,b,op,"add","ADD",RexLang_Function_Add);
}
Token Pointer_Pointer_Handler_Sub(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    return RexLang_Execute(ll,a,b,op,"sub","SUB",RexLang_Function_Sub);
}

Token Pointer_Pointer_Handler_Equ(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //return RexLang_ExecuteCmp(ll,a,b,op,"sete","EQU",RexLang_Function_Equ);
    return RexLang_ExecuteJmp(ll,a,b,op,"jz","EQU",RexLang_Function_Equ);
}
Token Pointer_Pointer_Handler_Neq(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //return RexLang_ExecuteCmp(ll,a,b,op,"setne","NEQ",RexLang_Function_Neq);
    return RexLang_ExecuteJmp(ll,a,b,op,"jnz","NEQ",RexLang_Function_Neq);
}
Token Pointer_Pointer_Handler_Les(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //return RexLang_ExecuteCmp(ll,a,b,op,"setl","LES",RexLang_Function_Les);
    return RexLang_ExecuteJmp(ll,a,b,op,"jn","LES",RexLang_Function_Les);
}
Token Pointer_Pointer_Handler_Grt(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //return RexLang_ExecuteCmp(ll,a,b,op,"setg","GRT",RexLang_Function_Grt);
    return RexLang_ExecuteJmp(ll,a,b,op,"jp","GRT",RexLang_Function_Grt);
}
Token Pointer_Pointer_Handler_Leq(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //return RexLang_ExecuteCmp(ll,a,b,op,"setle","LEQ",RexLang_Function_Leq);
    return RexLang_ExecuteJmp(ll,a,b,op,"jzn","LEQ",RexLang_Function_Leq);
}
Token Pointer_Pointer_Handler_Grq(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //return RexLang_ExecuteCmp(ll,a,b,op,"setge","GRQ",RexLang_Function_Grq);
    return RexLang_ExecuteJmp(ll,a,b,op,"jzp","GRQ",RexLang_Function_Grq);
}

Token Pointer_Handler_Adr(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    //printf("[Pointer]: ADR: &%s\n",a->str);
    
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
Token Pointer_Handler_Drf(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    //printf("[Pointer]: DRF: *%s\n",a->str);
    
    if(a->tt==TOKEN_STRING){
        CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        
        if(RexLang_DrefType(ll,v->typename)){
            CStr type = CStr_PopOff(v->typename);
            type[CStr_Size(type) - 1] = '&';
            RexLang_Variable_Build_Decl(ll,stack_name,type);
            CStr_Free(&type);
            
            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
            CStr location_a = RexLang_Location(ll,RexLang_REG_D,a->str);
            CStr location_s = RexLang_Location(ll,RexLang_REG_D,a->str);
            RexLang_Indentation_Appendf(ll,&ll->text,"ld %s\t%s",RexLang_REG_A,location_a);
            RexLang_Indentation_Appendf(ll,&ll->text,"st %s\t%s",location_s,RexLang_REG_A);
            return stack_t;
        }else{
            CStr type = CStr_Cpy(v->typename);
            type[CStr_Size(type) - 1] = '&';
            RexLang_Variable_Build_Decl(ll,stack_name,type);
            CStr_Free(&type);
            
            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
            
            CStr location_a = RexLang_Location(ll,RexLang_REG_D,a->str);
            CStr location_s = RexLang_Location(ll,RexLang_REG_D,stack_name);
            RexLang_Indentation_Appendf(ll,&ll->text,"ld %s\t%s",RexLang_REG_A,location_a);
            RexLang_Indentation_Appendf(ll,&ll->text,"st %s\t%s",location_s,RexLang_REG_A);
            CStr_Free(&location_a);
            CStr_Free(&location_s);
            return stack_t;
        }
    }else{
        Environment_ErrorHandler(&ll->ev,"Drf: Error -> %s has no dref op!",a->str);
        return Token_Null();
    }
}
Token Pointer_Handler_Arw(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //printf("[Pointer]: ARW: %s->%s\n",a->str,b->str);
    
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        
        if(v){
            CStr structtype = CStr_PopOff(v->typename);
            Type* t = Scope_FindType(&ll->ev.sc,structtype);
            CStr_Free(&structtype);
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
                            CStr type = CStr_Concat(member->type,"&");
                            RexLang_Variable_Build_Decl(ll,stack_name,type);
                            CStr_Free(&type);

                            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
                            CStr location_a = RexLang_Location(ll,RexLang_REG_D,a->str);
                            CStr location_s = RexLang_Location(ll,RexLang_REG_D,stack_name);

                            RexLang_Indentation_Appendf(ll,&ll->text,"ld %s\t%s",RexLang_REG_A,location_a);
                            RexLang_Indentation_Appendf(ll,&ll->text,"add %s\t%d",RexLang_REG_A,offset);
                            RexLang_Indentation_Appendf(ll,&ll->text,"st %s\t%s",location_s,RexLang_REG_A);

                            CStr_Free(&location_a);
                            CStr_Free(&location_s);
                        }else{
                            CStr type = CStr_Concat(member->type,"&");
                            RexLang_Variable_Build_Decl(ll,stack_name,type);
                            CStr_Free(&type);

                            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
                            CStr location_a = RexLang_Location(ll,RexLang_REG_D,a->str);
                            CStr location_s = RexLang_Location(ll,RexLang_REG_D,stack_name);

                            RexLang_Indentation_Appendf(ll,&ll->text,"ld %s\t%s",RexLang_REG_A,location_a);
                            RexLang_Indentation_Appendf(ll,&ll->text,"ld %s\t%s",RexLang_REG_A,RexLang_REG_A);
                            RexLang_Indentation_Appendf(ll,&ll->text,"add %s\t%d",RexLang_REG_A,offset);
                            RexLang_Indentation_Appendf(ll,&ll->text,"st %s\t%s",location_s,RexLang_REG_A);

                            CStr_Free(&location_a);
                            CStr_Free(&location_s);
                        }
                        return Token_Move(TOKEN_STRING,stack_name);
                    }else{
                        Environment_ErrorHandler(&ll->ev,"Arw: Member %s isn't pub or non self %s tries to access!",b->str,a->str);
                        return Token_Null();
                    }
                }else{
                    Environment_ErrorHandler(&ll->ev,"Arw: Member %s not found!",b->str);
                    return Token_Null();
                }
            }else{
                Environment_ErrorHandler(&ll->ev,"Arw: Type %s not found!",v->typename);
                return Token_Null();
            }
        }else{
            Environment_ErrorHandler(&ll->ev,"Arw: Variable %s not found!",a->str);
            return Token_Null();
        }
    }else{
        Environment_ErrorHandler(&ll->ev,"Arw: Error -> %s has no dref op!",a->str);
        return Token_Null();
    }
}

Token Pointer_I64_Handler_Subs(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    //printf("[Pointer]: SUBS: %s[%s]\n",a->str,b->str);
    
    Token add = Token_By(TOKEN_REXLANG_ADD,"+");
    Token drf = Token_By(TOKEN_REXLANG_DRF,"*");

    Token out = Pointer_Pointer_Handler_Add(ll,&add,args);
    TokenMap newargs = TokenMap_Make((Token[]){ out,Token_Null() });
    Token ret = Pointer_Handler_Drf(ll,&drf,&newargs);
    TokenMap_Free(&newargs);

    Token_Free(&drf);
    Token_Free(&add);
    return ret;
}

Token Pointer_Null_Handler_Cast(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("[Pointer]: CAST: %s\n",a->str);

    String ret = String_New();

    if(a->tt==TOKEN_NUMBER){
        String_AppendNumber(&ret,Number_Parse(a->str));
    }else{
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            RexLangVariable* sv = (RexLangVariable*)Variable_Data(v);
            String_Appendf(&ret,"T: %s,S:%d [&:%d,%d]",v->typename,sv->stack,sv->destroy,sv->sizeonstack);
        }else{
            printf("[I64]: Cast -> Error!\n");
            String_Append(&ret,"ERROR");
        }
    }

    CStr out = String_CStr(&ret);
    String_Free(&ret);
    return Token_Move(TOKEN_CONSTSTRING_DOUBLE,out);
}

Token Pointer_Handler_Cast(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    if(op->str==NULL) return Pointer_Null_Handler_Cast(ll,op,args);
    if(CStr_Cmp(op->str,I8_TYPE) || CStr_Cmp(op->str,I16_TYPE) ||
       CStr_Cmp(op->str,U8_TYPE) || CStr_Cmp(op->str,U16_TYPE) ||
       RexLang_PointerType(ll,op->str))
        return Int_Int_Handler_Cast(ll,op,args,op->str);
    return Token_Null();
}
Token Pointer_Handler_Size(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    //printf("[Pointer]: SIZE: %s\n",a->str);
    return Token_Move(TOKEN_NUMBER,Number_Get(POINTER_SIZE));
}

void Ex_Packer(ExternFunctionMap* Extern_Functions,Vector* funcs,Scope* s){//Vector<CStr>
    TypeMap_PushContained(&s->types,funcs,
        Type_Make(POINTER_TYPE,sizeof(RexLangVariable),OperatorInterationMap_Make((OperatorInterater[]){
            OperatorInterater_Make((CStr[]){ NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ADR,(Token(*)(void*,Token*,Vector*))Pointer_Handler_Adr),
                OperatorDefiner_New(TOKEN_REXLANG_DRF,(Token(*)(void*,Token*,Vector*))Pointer_Handler_Drf),
                OperatorDefiner_New(TOKEN_CAST,(Token(*)(void*,Token*,Vector*))Pointer_Handler_Cast),
                OperatorDefiner_New(TOKEN_INIT,(Token(*)(void*,Token*,Vector*))RexLang_Init),
                OperatorDefiner_New(TOKEN_REXLANG_SIZE,(Token(*)(void*,Token*,Vector*))Pointer_Handler_Size),
                //OperatorDefiner_New(TOKEN_DESTROY,NULL),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ POINTER_TYPE,NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ASS,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Ass),
                OperatorDefiner_New(TOKEN_REXLANG_ADD,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Add),
                OperatorDefiner_New(TOKEN_REXLANG_SUB,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Sub),
                OperatorDefiner_New(TOKEN_REXLANG_EQU,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Equ),
                OperatorDefiner_New(TOKEN_REXLANG_NEQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Neq),
                OperatorDefiner_New(TOKEN_REXLANG_LES,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Les),
                OperatorDefiner_New(TOKEN_REXLANG_GRT,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Grt),
                OperatorDefiner_New(TOKEN_REXLANG_LEQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Leq),
                OperatorDefiner_New(TOKEN_REXLANG_GRQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Grq),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ I8_TYPE,NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ASS,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Ass),
                OperatorDefiner_New(TOKEN_REXLANG_ADD,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Add),
                OperatorDefiner_New(TOKEN_REXLANG_SUB,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Sub),
                OperatorDefiner_New(TOKEN_REXLANG_EQU,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Equ),
                OperatorDefiner_New(TOKEN_REXLANG_NEQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Neq),
                OperatorDefiner_New(TOKEN_REXLANG_LES,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Les),
                OperatorDefiner_New(TOKEN_REXLANG_GRT,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Grt),
                OperatorDefiner_New(TOKEN_REXLANG_LEQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Leq),
                OperatorDefiner_New(TOKEN_REXLANG_GRQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Grq),
                OperatorDefiner_New(TOKEN_REXLANG_SUBS,(Token(*)(void*,Token*,Vector*))Pointer_I64_Handler_Subs),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ I16_TYPE,NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ASS,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Ass),
                OperatorDefiner_New(TOKEN_REXLANG_ADD,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Add),
                OperatorDefiner_New(TOKEN_REXLANG_SUB,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Sub),
                OperatorDefiner_New(TOKEN_REXLANG_EQU,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Equ),
                OperatorDefiner_New(TOKEN_REXLANG_NEQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Neq),
                OperatorDefiner_New(TOKEN_REXLANG_LES,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Les),
                OperatorDefiner_New(TOKEN_REXLANG_GRT,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Grt),
                OperatorDefiner_New(TOKEN_REXLANG_LEQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Leq),
                OperatorDefiner_New(TOKEN_REXLANG_GRQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Grq),
                OperatorDefiner_New(TOKEN_REXLANG_SUBS,(Token(*)(void*,Token*,Vector*))Pointer_I64_Handler_Subs),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ U8_TYPE,NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ASS,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Ass),
                OperatorDefiner_New(TOKEN_REXLANG_ADD,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Add),
                OperatorDefiner_New(TOKEN_REXLANG_SUB,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Sub),
                OperatorDefiner_New(TOKEN_REXLANG_EQU,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Equ),
                OperatorDefiner_New(TOKEN_REXLANG_NEQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Neq),
                OperatorDefiner_New(TOKEN_REXLANG_LES,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Les),
                OperatorDefiner_New(TOKEN_REXLANG_GRT,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Grt),
                OperatorDefiner_New(TOKEN_REXLANG_LEQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Leq),
                OperatorDefiner_New(TOKEN_REXLANG_GRQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Grq),
                OperatorDefiner_New(TOKEN_REXLANG_SUBS,(Token(*)(void*,Token*,Vector*))Pointer_I64_Handler_Subs),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ U16_TYPE,NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ASS,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Ass),
                OperatorDefiner_New(TOKEN_REXLANG_ADD,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Add),
                OperatorDefiner_New(TOKEN_REXLANG_SUB,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Sub),
                OperatorDefiner_New(TOKEN_REXLANG_EQU,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Equ),
                OperatorDefiner_New(TOKEN_REXLANG_NEQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Neq),
                OperatorDefiner_New(TOKEN_REXLANG_LES,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Les),
                OperatorDefiner_New(TOKEN_REXLANG_GRT,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Grt),
                OperatorDefiner_New(TOKEN_REXLANG_LEQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Leq),
                OperatorDefiner_New(TOKEN_REXLANG_GRQ,(Token(*)(void*,Token*,Vector*))Pointer_Pointer_Handler_Grq),
                OperatorDefiner_New(TOKEN_REXLANG_SUBS,(Token(*)(void*,Token*,Vector*))Pointer_I64_Handler_Subs),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ OPERATORINTERATER_DONTCARE,NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_REXLANG_ARW,(Token(*)(void*,Token*,Vector*))Pointer_Handler_Arw),
                OPERATORDEFINER_END
            })),
            OPERATORINTERATER_END
        }),RexLang_Destroyer,RexLang_Cpyer)
    );
}
