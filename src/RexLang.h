#ifndef RexLang_H
#define RexLang_H

#include "/home/codeleaded/System/Static/Container/CVector.h"

#include "/home/codeleaded/System/Static/Library/AlxScope.h"
#include "/home/codeleaded/System/Static/Library/AlxShutingYard.h"
#include "/home/codeleaded/System/Static/Library/AlxCompiler.h"
#include "/home/codeleaded/System/Static/Library/ConstParser.h"
#include "/home/codeleaded/System/Static/Library/Files.h"
#include "/home/codeleaded/System/Static/Library/String.h"

#include "RexLangDefines.h"
#include "RexLangASM.h"


Boolean RexLang_Compress(RexLang* ll,TokenMap* tm);
Boolean RexLang_Compress_pointer(RexLang* ll,TokenMap* tm);

Boolean RexLang_Compress_defines(RexLang* ll,TokenMap* tm){
    for(int i = 0;i<tm->size;i++){
        Token* t = (Token*)Vector_Get(tm,i);
        if(t->tt==TOKEN_STRING){
            for(int j = 0;j<ll->defines.size;j++){
                Define* d = (Define*)CVector_Get(&ll->defines,j);
                if(CStr_Cmp(d->name,t->str)){
                    Token_Free(t);
                    Vector_Remove(tm,i);

                    TokenMap tokm = TokenMap_Cpy(&d->content);
                    Vector_AddCount(tm,tokm.Memory,tokm.size,i);
                }
            }
        }
    }
    return False;
}
Boolean RexLang_Compress_pointer(RexLang* ll,TokenMap* tm){
    for(int i = 0;i<tm->size-1;i++){
        Token* type = (Token*)Vector_Get(tm,i);

        if(type->tt==TOKEN_TYPE){
            Token* next = (Token*)Vector_Get(tm,i+1);
            
            if(next->tt==TOKEN_ASTERISK || next->tt==TOKEN_REXLANG_MUL || next->tt==TOKEN_REXLANG_DRF){
                CStr newtype = CStr_Concat(type->str,"*");
                CStr_Set((char**)&type->str,newtype);
                CStr_Free(&newtype);

                Token_Free(next);
                Vector_Remove(tm,i+1);
                i--;
            }else if(next->tt==TOKEN_AMPERSAND || next->tt==TOKEN_REXLANG_AND || next->tt==TOKEN_REXLANG_ADR){
                CStr newtype = CStr_Concat(type->str,"&");
                CStr_Set((char**)&type->str,newtype);
                CStr_Free(&newtype);
                
                Token_Free(next);
                Vector_Remove(tm,i+1);
                i--;
            }else if(next->tt==TOKEN_REXLANG_SUBS){
                if(!RexLang_CompressStackType(ll,tm,i)) continue;
                type = (Token*)Vector_Get(tm,i);

                RexLang_Compress(ll,(TokenMap*)Vector_Get(type->args,0));

                CStr newtype = CStr_Concat(type->str,"*");
                CStr_Set((char**)&type->str,newtype);
                CStr_Free(&newtype);
                
                Token_Free(next);
                Vector_Remove(tm,i+1);
                i--;
            }
        }
    }
    return False;
}
Boolean RexLang_Compress_functionpointer(RexLang* ll,TokenMap* tm){
    for(int i = 0;i<tm->size-1;i++){
        Token* type = (Token*)Vector_Get(tm,i);

        if(type->tt==TOKEN_TYPE){
            Token* next = (Token*)Vector_Get(tm,i+1);
            if(next->tt==TOKEN_PARENTHESES_L){
                int pretc = 1;
                int pretr = 2;
                for(;pretc>0;pretr++){
                    next = (Token*)Vector_Get(tm,i + pretr);
                    
                    if(next->tt==TOKEN_PARENTHESES_L) pretc++;
                    if(next->tt==TOKEN_PARENTHESES_R) pretc--;
                    if(next->tt!=TOKEN_TYPE && next->tt!=TOKEN_COMMA && next->tt!=TOKEN_PARENTHESES_L && next->tt!=TOKEN_PARENTHESES_R){
                        continue;
                    }
                    if(pretr + i==tm->size-1){
                        Compiler_ErrorHandler(&ll->ev,"Compress Functionpointer -> needs a ) to end function pointer type!");
                        return 0;
                    }
                }

                String builder = String_New();
                for(int j = 0;j<pretr;j++){
                    next = (Token*)Vector_Get(tm,i + j);
                    String_Append(&builder,next->str);
                }
                CStr newtype = String_CStr(&builder);
                CStr_Set((char**)&type->str,newtype);
                CStr_Free(&newtype);
                String_Free(&builder);

                TokenMap_Remove(tm,i+1,i+pretr);
                RexLang_Compress_pointer(ll,tm);
                i = -1;
            }
        }
    }
    return False;
}
Boolean RexLang_Compress_cast(RexLang* ll,TokenMap* tm){
    for(int i = 0;i<tm->size;i++){
        Token* pretl = (Token*)Vector_Get(tm,i);
        Token* type = (Token*)Vector_Get(tm,i+1);
        Token* pretr = (Token*)Vector_Get(tm,i+2);

        if(pretl->tt==TOKEN_PARENTHESES_L && type->tt==TOKEN_TYPE && pretr->tt==TOKEN_PARENTHESES_R){
            pretl->tt = TOKEN_CAST;
            CStr_Set((char**)&pretl->str,type->str);
            TokenMap_Remove(tm,i+1,i+3);
        }
    }
    return False;
}
Boolean RexLang_Compress_subscript(RexLang* ll,TokenMap* tm){
    for(int i = 0;i<tm->size;i++){
        Token* subs = (Token*)Vector_Get(tm,i);

        if(subs->tt==TOKEN_REXLANG_SUBS){
            Vector_Add(tm,(Token[]){ Token_By(TOKEN_PARENTHESES_L,"(") },i+1);
        }else if(subs->tt==TOKEN_REXLANG_SUBSR){
            subs->tt = TOKEN_PARENTHESES_R;
            CStr_Set((CStr*)&subs->str,")");
        }
    }
    return False;
}
Boolean RexLang_Compress_staticmethods(RexLang* ll,TokenMap* tm){
    for(int i = 0;i<tm->size;i++){
        Token* class = (Token*)Vector_Get(tm,i);
        
        if(i<tm->size-2){
            Token* dddop = (Token*)Vector_Get(tm,i+1);
            Token* func = (Token*)Vector_Get(tm,i+2);

            if((class->tt==TOKEN_STRING || class->tt==TOKEN_TYPE) && dddop->tt==TOKEN_REXLANG_DDDOT && (func->tt==TOKEN_STRING || func->tt==TOKEN_TYPE)){
                String builder = String_Make(class->str);
                String_Append(&builder,dddop->str);
                String_Append(&builder,func->str);

                CStr space = String_CStr(&builder);
                String_Free(&builder);

                CStr_Set((CStr*)&func->str,space);
                CStr_Free(&space);

                TokenMap_Remove(tm,i,i+2);
                i--;
                continue;
            }
        }
        if(class->tt==TOKEN_STRING){
            Type* t = TypeMap_Find(&ll->ev.sc.types,class->str);
            if(t) class->tt = TOKEN_TYPE;
            else{
                CStr spacename = RexLang_SpaceName(ll,class->str);
                Type* t = TypeMap_Find(&ll->ev.sc.types,spacename);
                if(t){
                    class->tt = TOKEN_TYPE;
                    CStr_Set(&class->str,spacename);
                }
                CStr_Free(&spacename);
            }
        }
    }
    return False;
}
Boolean RexLang_Compress_functioncalls(RexLang* ll,TokenMap* tm){
    for(int i = 0;i<tm->size-1;i++){
        Token* func = (Token*)Vector_Get(tm,i);
        Token* prentl = (Token*)Vector_Get(tm,i+1);

        if(func->tt==TOKEN_STRING && prentl->tt==TOKEN_PARENTHESES_L){
            func->tt = TOKEN_FUNCTION;
            func->args = malloc(sizeof(Vector));
            *func->args = Vector_New(sizeof(TokenMap));
            
            int Parentheses = 0;
            int Last = i+2;
            Token* prentr = (Token*)Vector_Get(tm,i+2);
            if(prentr->tt!=TOKEN_PARENTHESES_R){
                for(int j = Last;j<tm->size;j++){
                    Token* t = (Token*)Vector_Get(tm,j);
            
                    if(t->tt==TOKEN_PARENTHESES_L) Parentheses++;
                    if(t->tt==TOKEN_PARENTHESES_R) Parentheses--;
                    if(t->tt==TOKEN_COMMA || Parentheses<0){
                        if(Parentheses<=0){
                            TokenMap newtm = TokenMap_Sub(tm,Last,j);
                            RexLang_Compress(ll,&newtm);
                            Vector_Push(func->args,&newtm);
                            Last = j + 1;
                        }
                    }
                    if(Parentheses<0) break;
                } 
            }else{
                Last++;
            }

            TokenMap_Remove(tm,i+1,Last);
        }
    }
    return False;
}
Boolean RexLang_Compress_functionaspointer(RexLang* ll,TokenMap* tm){
    for(int i = 0;i<tm->size;i++){
        Token* func = (Token*)Vector_Get(tm,i);

        if(func->tt==TOKEN_STRING){
            Function* f = FunctionMap_Get(&ll->ev.fs,func->str);
            if(f){
                func->tt = TOKEN_FUNCTIONPOINTER;
            }
        }
    }
    return False;
}
Boolean RexLang_Compress_function(RexLang* ll,TokenMap* tm){
    for(int i = 0;i<tm->size-2;i++){
        Token* ttype = (Token*)Vector_Get(tm,i);
        Token* tname = (Token*)Vector_Get(tm,i+1);
        Token* tprentl = (Token*)Vector_Get(tm,i+2);
        
        if(ttype->tt==TOKEN_TYPE && tname->tt==TOKEN_STRING && tprentl->tt==TOKEN_PARENTHESES_L){
            Vector params = Vector_New(sizeof(Member));

            TT_Iter pretl = TokenMap_Find(tm,TOKEN_PARENTHESES_L);
            TT_Iter pretr = TokenMap_Find(tm,TOKEN_PARENTHESES_R);
            
            if(pretr - pretl > 1){
                TokenMap rest = TokenMap_SubFromToken(tm,TOKEN_PARENTHESES_L);
                TokenMap between = TokenMap_SubToToken(&rest,TOKEN_PARENTHESES_R);
                TokenMap_Free(&rest);

                for(;between.size>0;){
                    TokenMap first = TokenMap_SubToToken(&between,TOKEN_COMMA);
                    TokenMap after = TokenMap_SubFromToken(&between,TOKEN_COMMA);

                    TT_Iter type = TokenMap_Find(&first,TOKEN_TYPE);
                    TT_Iter name = TokenMap_Find(&first,TOKEN_STRING);
                    TT_Iter constant = TokenMap_Find(&first,TOKEN_REXLANG_CONST);

                    if(type>=0 && name>=0){
                        CStr sname = ((Token*)Vector_Get(&first,name))->str;
                        CStr stype = ((Token*)Vector_Get(&first,type))->str;

                        Boolean bconstant = constant>=0;
                        Vector_Push(&params,(Member[]){ Member_Make(bconstant,0,stype,sname) });
                    }else{
                        Compiler_ErrorHandler(&ll->ev,"Compress Function: no type and no var found");
                    }

                    TokenMap_Free(&first);
                    TokenMap_Free(&between);
                    between = after;
                }
                TokenMap_Free(&between);
            }

            //Boolean bref = False;
            //Boolean bconstant = False;

            TokenMap rettypem = TokenMap_SubToToken(tm,TOKEN_PARENTHESES_L);
            TT_Iter it_pub = TokenMap_Find(&rettypem,TOKEN_REXLANG_PUB);
            Boolean bpub = it_pub>=0 ? RexLang_ACCESS_PUB : RexLang_ACCESS_NONE;
            TokenMap_Free(&rettypem);

            Vector_Push(&params,(Member[]){ MEMBER_END });

            CStr realname = RexLang_FuncSpaceName(ll,tname->str);
            CStr_Set((char**)&tname->str,realname);

            CallPosition cp = CallPosition_New(TOKEN_FUNCTIONDECL,ll->ev.iter);
            Vector_Push(&ll->ev.cs,&cp);

            Function f = Function_MakeX(ll->ev.iter,bpub,realname,ttype->str,(Member*)params.Memory);
            Vector_Push(&ll->ev.fs,&f);
            Vector_Free(&params);
            CStr_Free(&realname);

            CStr cname = CStr_Cpy(tname->str);
            TokenMap_Clear(tm);
            TokenMap_Push(tm,(Token[]){ Token_Move(TOKEN_FUNCTIONDECL,cname) });
            //TokenMap_Push(tm,(Token[]){ Token_By(TOKEN_CURLY_BRACES_L,"{") });
        }
    }

    return False;
}
Boolean RexLang_Compress(RexLang* ll,TokenMap* tm){
    //RexLang_Compress_objects(ev,tm);

    RexLang_Compress_staticmethods(ll,tm);
    RexLang_Compress_defines(ll,tm);
    RexLang_Compress_pointer(ll,tm);
    RexLang_Compress_functionpointer(ll,tm);
    RexLang_Compress_cast(ll,tm);
    RexLang_Compress_subscript(ll,tm);
    RexLang_Compress_function(ll,tm);
    RexLang_Compress_functioncalls(ll,tm);
    RexLang_Compress_functionaspointer(ll,tm);
    return False;
}

Boolean RexLang_PP_if(Compiler* ev,TokenMap* tm){
    CallPosition cp =  CallPosition_New(TOKEN_REXLANG_IF,ev->iter);
    Vector_Push(&ev->cs,&cp);
    return False;
}
Boolean RexLang_PP_elif(Compiler* ev,TokenMap* tm){
    CallPosition cp =  CallPosition_New(TOKEN_REXLANG_ELIF,ev->iter);
    Vector_Push(&ev->cs,&cp);
    return False;
}
Boolean RexLang_PP_else(Compiler* ev,TokenMap* tm){
    CallPosition cp =  CallPosition_New(TOKEN_REXLANG_ELSE,ev->iter);
    Vector_Push(&ev->cs,&cp);
    return False;
}
Boolean RexLang_PP_while(Compiler* ev,TokenMap* tm){
    CallPosition cp =  CallPosition_New(TOKEN_REXLANG_WHILE,ev->iter);
    Vector_Push(&ev->cs,&cp);
    return False;
}
Boolean RexLang_PP_for(Compiler* ev,TokenMap* tm){
    CallPosition cp = CallPosition_New(TOKEN_REXLANG_FOR,ev->iter);
    Vector_Push(&ev->cs,&cp);
    return False;
}

Boolean RexLang_PP_Struct(RexLang* ll,TokenMap* tm){
    Token* t_struct = (Token*)Vector_Get(tm,0);
    Token* t_name = (Token*)Vector_Get(tm,1);

    if(t_name->tt==TOKEN_STRING){
        if(tm->size>2){
            Token* subsl = (Token*)Vector_Get(tm,2);
            if(subsl->tt==TOKEN_REXLANG_SUBS){
                MemberMap params = MemberMap_New();

                TokenMap rest = TokenMap_SubFromToken(tm,TOKEN_REXLANG_SUBS);
                TokenMap between = TokenMap_SubToToken(&rest,TOKEN_REXLANG_SUBSR);
                TokenMap_Free(&rest);

                for(;between.size>0;){
                    TokenMap first = TokenMap_SubToToken(&between,TOKEN_COMMA);
                    TokenMap after = TokenMap_SubFromToken(&between,TOKEN_COMMA);

                    TT_Iter type = TokenMap_Find(&first,TOKEN_TYPE);
                    TT_Iter name = TokenMap_Find(&first,TOKEN_STRING);
                    TT_Iter pub = TokenMap_Find(&first,TOKEN_REXLANG_PUB);

                    if(type<0){
                        Compiler_ErrorHandler(&ll->ev,"Struct -> Error: couldn't find Type!");
                        return False;
                    }else if(name<0){
                        Compiler_ErrorHandler(&ll->ev,"Struct -> Error: couldn't find name!");
                        return False;
                    }else{
                        CStr sname = ((Token*)Vector_Get(&first,name))->str;
                        CStr stype = ((Token*)Vector_Get(&first,type))->str;

                        Boolean bpub = pub>=0;
                        Vector_Push(&params,(Member[]){ Member_MakeX(0,0,bpub,stype,sname) });
                    }

                    TokenMap_Free(&first);
                    TokenMap_Free(&between);
                    between = after;
                }
                TokenMap_Free(&between);

                Type* parent = Scope_FindType(&ll->ev.sc,STRUCT_TYPE);
                if(!parent){
                    Compiler_ErrorHandler(&ll->ev,"Struct -> Error: type %s doesn't exist!",STRUCT_TYPE);
                    return False;
                }
                
                Type t = Type_Cpy(parent);
                
                CStr realname = RexLang_SpaceName(ll,t_name->str);
                CStr_Set(&t.name,realname);
                Type_SetAllOperators(&t,STRUCT_TYPE,realname);
                CStr_Free(&realname);

                MemberMap_Set(&t.related,&params);

                TypeMap_Push(&ll->ev.sc.types,&t);
                return False;
            }else{
                Compiler_ErrorHandler(&ll->ev,"Struct: expected [: ");
                return False;
            }
        }else{
            Compiler_ErrorHandler(&ll->ev,"Struct: struct decl needs body [...]!");
            return False;
        }
    }else{
        Compiler_ErrorHandler(&ll->ev,"Struct: name of struct doesn't exist!");
        return False;
    }
}
Boolean RexLang_PP_Impl(RexLang* ll,TokenMap* tm){
    Token* t_impl = (Token*)Vector_Get(tm,0);
    Token* t_name = (Token*)Vector_Get(tm,1);
    
    if(t_name->tt==TOKEN_TYPE){
        CallPosition cp = CallPosition_New_N(TOKEN_REXLANG_IMPL,ll->ev.iter,t_name->str);
        Vector_Push(&ll->ev.cs,&cp);
    }else{
        Compiler_ErrorHandler(&ll->ev,"Impl: name of impl doesn't exist!");
    }
    return False;
}
Boolean RexLang_PP_Namespace(RexLang* ll,TokenMap* tm){
    Token* t_impl = (Token*)Vector_Get(tm,0);
    Token* t_name = (Token*)Vector_Get(tm,1);
    
    if(t_name->tt==TOKEN_STRING){
        CallPosition cp = CallPosition_New_N(TOKEN_REXLANG_NAMESPACE,ll->ev.iter,t_name->str);
        Vector_Push(&ll->ev.cs,&cp);
    }else{
        Compiler_ErrorHandler(&ll->ev,"Namespace: name of namespace doesn't exist!");
    }
    return False;
}
Boolean RexLang_PP_Curly_L(RexLang* ll,TokenMap* tm){
    CallPosition cp = CallPosition_New(TOKEN_NONE,ll->ev.iter);
    Vector_Push(&ll->ev.cs,&cp);
    return False;
}
Boolean RexLang_PP_Curly_R(RexLang* ll,TokenMap* tm){
    TT_Type tt = CallStack_Back(&ll->ev.cs);
    
    switch (tt){
    case TOKEN_FUNCTIONDECL:
        break;
    case TOKEN_REXLANG_IMPL:
        break;
    case TOKEN_REXLANG_NAMESPACE:
        break;
    case TOKEN_REXLANG_IF:
        break;
    case TOKEN_REXLANG_ELIF:
        break;
    case TOKEN_REXLANG_ELSE:
        break;
    case TOKEN_REXLANG_WHILE:
        break;
    case TOKEN_REXLANG_FOR:
        break;
    case TOKEN_NONE:
        break;
    default:
        Compiler_ErrorHandler(&ll->ev,"Call Stack top type is not defined!");
        break;
    }
    CallStack_Pop(&ll->ev.cs);
    return False;
}

Boolean RexLang_Import(RexLang* ll,TokenMap* tm){
    Token* file = (Token*)Vector_Get(tm,1);

    if(file->tt==TOKEN_CONSTSTRING_DOUBLE){
        char* type = Files_Type(file->str);
        if(CStr_Cmp(type,RexLang_TYPE)){
            CStr current = *(CStr*)CVector_Get(&ll->filesstack,ll->filesstack.size-1);
            CStr dir = Files_Path(current);
            CStr realpath = Files_FromPath(dir,file->str);
            CStr_Free(&dir);

            if(!CVector_Contains(&ll->filesinc,(CStr[]){ realpath })){
                CVector_Push(&ll->filesstack,(CStr[]){ CStr_Cpy(realpath) });
                CVector_Push(&ll->filesinc,(CStr[]){ CStr_Cpy(realpath) });
                
                Compiler_AddScript(&ll->ev,ll->ev.iter,realpath);
            
                CVector_PopTop(&ll->filesstack);
                CStr_Free(&realpath);
            }else{
                //Compiler_ErrorHandler(&ll->ev,"module \"%s\" already included!",file->str);
            }
        }else{
            Compiler_ErrorHandler(&ll->ev,"file type of module \"%s\" is wrong: .%s, should be .%s",file->str,type,RexLang_TYPE);
        }
        free(type);
    }else if(file->tt==TOKEN_STRING){
        CStr current = *(CStr*)CVector_Get(&ll->filesstack,ll->filesstack.size-1);
        CStr path = CStr_Concat(file->str,"." RexLang_TYPE);
        CStr realpath = Files_FromPath("./lib/",path);
        CStr_Free(&path);

        if(!CVector_Contains(&ll->filesinc,(CStr[]){ realpath })){
            CVector_Push(&ll->filesstack,(CStr[]){ CStr_Cpy(realpath) });
            CVector_Push(&ll->filesinc,(CStr[]){ CStr_Cpy(realpath) });
            
            Compiler_AddScript(&ll->ev,ll->ev.iter,realpath);
        
            CVector_PopTop(&ll->filesstack);
            CStr_Free(&realpath);
        }else{
            //Compiler_ErrorHandler(&ll->ev,"module \"%s\" already included!",file->str);
        }
    }else{
        Compiler_ErrorHandler(&ll->ev,"module \"%s\" doesn't exist!",file->str);
    }
    return True;
}
Boolean RexLang_Define(RexLang* ll,TokenMap* tm){
    Token* define = (Token*)Vector_Get(tm,0);
    Token* name = (Token*)Vector_Get(tm,1);
    
    if(tm->size>1 && name->tt==TOKEN_STRING){
        Define d = Define_New(name->str,TokenMap_Sub(tm,2,tm->size));
        CVector_Push(&ll->defines,&d);
    }else{
        Compiler_ErrorHandler(&ll->ev,"Define: define needs a name!");
    }
    TokenMap_Clear(tm);
    return False;
}

Boolean RexLang_If(RexLang* ll,TokenMap* tm){
    Token* if_tok = (Token*)Vector_Get(tm,0);
    Token* cbl_tok = (Token*)Vector_Get(tm,tm->size-1);
    
    if(tm->size>2){
        if(cbl_tok->tt==TOKEN_CURLY_BRACES_L){
            if(ll->ev.sc.range>0){
                Token* t = (Token*)Vector_Get(tm,0);
                Token_Free(t);
                Vector_Remove(tm,0);

                t = (Token*)Vector_Get(tm,tm->size-1);
                Token_Free(t);
                Vector_Remove(tm,tm->size-1);

                Token btok = Token_By(TOKEN_STRING,RexLang_BOOL);
                RexLang_Variable_Build_Decl(ll,btok.str,BOOL_TYPE);

                Vector_Add(tm,(Token[]){ Token_Cpy(&btok) },0);
                Vector_Add(tm,(Token[]){ Compiler_SetterToken(&ll->ev) },1);

                Boolean ret = Compiler_ShutingYard(&ll->ev,tm);
                RexLang_IntoReg(ll,&btok,RexLang_REG_A);
                Scope_DestroyVariable(&ll->ev.sc,btok.str);
                Token_Free(&btok);

                RexLang_LogicAddPath(ll);

                CStr nextlabel = RexLang_Logic(ll,0,RexLang_IF,ll->ev.sc.range);
                CStr otherlabel = RexLang_Logic(ll,1,RexLang_IF,ll->ev.sc.range);
                RexLang_Indentation_Appendf(ll,&ll->text,"sub\t\t%s\t1",RexLang_REG_A);
                RexLang_Indentation_Appendf(ll,&ll->text,"jnz\t\t%s",otherlabel);
                RexLang_Indentation_Appendf(ll,&ll->text,"%s:",nextlabel);
                CStr_Free(&otherlabel);
                CStr_Free(&nextlabel);

                ll->ev.sc.range++;
                CallPosition cp = CallPosition_New(TOKEN_REXLANG_IF,ll->ev.iter);
                CallStack_Push(&ll->ev.cs,&cp);
                return ret;
            }else{
                Compiler_ErrorHandler(&ll->ev,"if: global if isn't allowed!");
                return False;
            }
        }else{
            Compiler_ErrorHandler(&ll->ev,"if: should end with a { like: if ... { ");
            return False;
        }
    }else{
        Compiler_ErrorHandler(&ll->ev,"if: should look like: if ... { ");
        return False;
    }
}
Boolean RexLang_Elif(RexLang* ll,TokenMap* tm){
    Token* elif_tok = (Token*)Vector_Get(tm,0);
    Token* cbl_tok = (Token*)Vector_Get(tm,tm->size-1);
    
    if(tm->size>2){
        if(cbl_tok->tt==TOKEN_CURLY_BRACES_L){
            if(ll->ev.sc.range>0){
                Token* t = (Token*)Vector_Get(tm,0);
                Token_Free(t);
                Vector_Remove(tm,0);

                t = (Token*)Vector_Get(tm,tm->size-1);
                Token_Free(t);
                Vector_Remove(tm,tm->size-1);

                Token btok = Token_By(TOKEN_STRING,RexLang_BOOL);
                RexLang_Variable_Build_Decl(ll,btok.str,BOOL_TYPE);

                Vector_Add(tm,(Token[]){ Token_Cpy(&btok) },0);
                Vector_Add(tm,(Token[]){ Compiler_SetterToken(&ll->ev) },1);

                Boolean ret = Compiler_ShutingYard(&ll->ev,tm);
                RexLang_IntoReg(ll,&btok,RexLang_REG_A);
                Scope_DestroyVariable(&ll->ev.sc,btok.str);
                Token_Free(&btok);

                RexLang_LogicAddExtend(ll);

                CStr nextlabel = RexLang_Logic(ll,0,RexLang_ELIF,ll->ev.sc.range);
                CStr otherlabel = RexLang_Logic(ll,1,RexLang_ELIF,ll->ev.sc.range);
                RexLang_Indentation_Appendf(ll,&ll->text,"sub\t\t%s\t1",RexLang_REG_A);
                RexLang_Indentation_Appendf(ll,&ll->text,"jnz\t\t%s",otherlabel);
                RexLang_Indentation_Appendf(ll,&ll->text,"%s:",nextlabel);
                CStr_Free(&otherlabel);
                CStr_Free(&nextlabel);

                ll->ev.sc.range++;
                CallPosition cp = CallPosition_New(TOKEN_REXLANG_ELIF,ll->ev.iter);
                CallStack_Push(&ll->ev.cs,&cp);
                return ret;
            }else{
                Compiler_ErrorHandler(&ll->ev,"elif: global elif isn't allowed!");
                return False;
            }
        }else{
            Compiler_ErrorHandler(&ll->ev,"elif: should end with a { like: elif ... { ");
            return False;
        }
    }else{
        Compiler_ErrorHandler(&ll->ev,"elif: should look like: elif ... { ");
        return False;
    }
}
Boolean RexLang_Else(RexLang* ll,TokenMap* tm){
    Token* if_tok = (Token*)Vector_Get(tm,0);
    Token* cbl_tok = (Token*)Vector_Get(tm,tm->size-1);
    
    if(tm->size==2){
        if(cbl_tok->tt==TOKEN_CURLY_BRACES_L){
            if(ll->ev.sc.range>0){
                Token* t = (Token*)Vector_Get(tm,0);
                Token_Free(t);
                Vector_Remove(tm,0);

                t = (Token*)Vector_Get(tm,tm->size-1);
                Token_Free(t);
                Vector_Remove(tm,tm->size-1);

                CStr nextlabel = RexLang_Logic(ll,0,RexLang_ELSE,ll->ev.sc.range);
                RexLang_Indentation_Appendf(ll,&ll->text,"%s:",nextlabel);
                CStr_Free(&nextlabel);

                ll->ev.sc.range++;
                CallPosition cp = CallPosition_New(TOKEN_REXLANG_ELSE,ll->ev.iter);
                CallStack_Push(&ll->ev.cs,&cp);
                return False;
            }else{
                Compiler_ErrorHandler(&ll->ev,"else: global else isn't allowed!");
                return False;
            }
        }else{
            Compiler_ErrorHandler(&ll->ev,"else: should end with a { like: else { ");
            return False;
        }
    }else{
        Compiler_ErrorHandler(&ll->ev,"else: should look like: else { ");
        return False;
    }
}
Boolean RexLang_While(RexLang* ll,TokenMap* tm){
    Token* while_tok = (Token*)Vector_Get(tm,0);
    Token* cbl_tok = (Token*)Vector_Get(tm,tm->size-1);
    
    if(tm->size>2){
        if(cbl_tok->tt==TOKEN_CURLY_BRACES_L){
            if(ll->ev.sc.range>0){
                Token* t = (Token*)Vector_Get(tm,0);
                Token_Free(t);
                Vector_Remove(tm,0);

                t = (Token*)Vector_Get(tm,tm->size-1);
                Token_Free(t);
                Vector_Remove(tm,tm->size-1);

                RexLang_LogicAddPath(ll);
                CStr nextlabel = RexLang_Logic(ll,0,RexLang_WHILE,ll->ev.sc.range);
                CStr otherlabel = RexLang_Logic(ll,1,RexLang_WHILE,ll->ev.sc.range);
                RexLang_Indentation_Appendf(ll,&ll->text,"%s:",nextlabel);

                Token btok = Token_By(TOKEN_STRING,RexLang_BOOL);
                RexLang_Variable_Build_Decl(ll,btok.str,BOOL_TYPE);

                Vector_Add(tm,(Token[]){ Token_Cpy(&btok) },0);
                Vector_Add(tm,(Token[]){ Compiler_SetterToken(&ll->ev) },1);

                Boolean ret = Compiler_ShutingYard(&ll->ev,tm);
                RexLang_IntoReg(ll,&btok,RexLang_REG_A);
                Scope_DestroyVariable(&ll->ev.sc,btok.str);
                Token_Free(&btok);

                RexLang_Indentation_Appendf(ll,&ll->text,"sub\t\t%s\t1",RexLang_REG_A);
                RexLang_Indentation_Appendf(ll,&ll->text,"jnz\t\t%s",otherlabel);
                CStr_Free(&otherlabel);
                CStr_Free(&nextlabel);

                ll->ev.sc.range++;
                CallPosition cp = CallPosition_New(TOKEN_REXLANG_WHILE,ll->ev.iter);
                CallStack_Push(&ll->ev.cs,&cp);
                return ret;
            }else{
                Compiler_ErrorHandler(&ll->ev,"while: global while isn't allowed!");
                return False;
            }
        }else{
            Compiler_ErrorHandler(&ll->ev,"while: should end with a { like: while ... { ");
            return False;
        }
    }else{
        Compiler_ErrorHandler(&ll->ev,"while: should look like: while ... { ");
        return False;
    }
}
Boolean RexLang_For(RexLang* ll,TokenMap* tm){
    Token* for_tok = (Token*)Vector_Get(tm,0);
    Token* cbl_tok = (Token*)Vector_Get(tm,tm->size-1);
    
    if(tm->size>2){
        if(cbl_tok->tt==TOKEN_CURLY_BRACES_L){
            if(ll->ev.sc.range>0){
                Token* t = (Token*)Vector_Get(tm,0);
                Token_Free(t);
                Vector_Remove(tm,0);

                t = (Token*)Vector_Get(tm,tm->size-1);
                Token_Free(t);
                Vector_Remove(tm,tm->size-1);

                TokenMap statement_0 = TokenMap_SubToToken(tm,TOKEN_COMMA);
                TokenMap statement_up = TokenMap_SubFromToken(tm,TOKEN_COMMA);
                TokenMap statement_1 = TokenMap_SubToToken(&statement_up,TOKEN_COMMA);
                TokenMap statement_2 = TokenMap_SubFromToken(&statement_up,TOKEN_COMMA);
                TokenMap_Free(&statement_up);

                ll->ev.sc.range++;
                Compiler_Do(&ll->ev,&statement_0);
                TokenMap_Free(&statement_0);

                RexLang_LogicAddPath(ll);
                CStr nextlabel = RexLang_Logic(ll,0,RexLang_FOR,ll->ev.sc.range);
                CStr otherlabel = RexLang_Logic(ll,1,RexLang_FOR,ll->ev.sc.range);
                RexLang_Indentation_Appendf(ll,&ll->text,"%s:",nextlabel);

                Boolean ret = False;
                if(statement_1.size>0){
                   Token btok = Token_By(TOKEN_STRING,RexLang_BOOL);
                    RexLang_Variable_Build_Decl(ll,btok.str,BOOL_TYPE);

                    Vector_Add(&statement_1,(Token[]){ Token_Cpy(&btok) },0);
                    Vector_Add(&statement_1,(Token[]){ Compiler_SetterToken(&ll->ev) },1);

                    ret = Compiler_ShutingYard(&ll->ev,&statement_1);
                    RexLang_IntoReg(ll,&btok,RexLang_REG_A);
                    Scope_DestroyVariable(&ll->ev.sc,btok.str);
                    Token_Free(&btok); 

                    RexLang_Indentation_Appendf(ll,&ll->text,"sub\t\t%s\t1",RexLang_REG_A);
                    RexLang_Indentation_Appendf(ll,&ll->text,"jnz\t\t%s",otherlabel);
                }
                TokenMap_Free(&statement_1);
                
                CStr_Free(&otherlabel);
                CStr_Free(&nextlabel);

                ll->ev.sc.range++;
                CallPosition cp = CallPosition_Make(statement_2,TOKEN_REXLANG_FOR,ll->ev.iter,NULL,ll->ev.sc.range);
                CallStack_Push(&ll->ev.cs,&cp);
                return ret;
            }else{
                Compiler_ErrorHandler(&ll->ev,"for: global for isn't allowed!");
                return False;
            }
        }else{
            //TokenMap_Print(tm);
            Compiler_ErrorHandler(&ll->ev,"for: should end with a { like: for ..., ..., ... { ");
            return False;
        }
    }else{
        Compiler_ErrorHandler(&ll->ev,"for: should look like: for ..., ..., ... { ");
        return False;
    }
}

Boolean RexLang_Curly_L(RexLang* ll,TokenMap* tm){
    RexLang_LogicAddPath(ll);
    ll->ev.sc.range++;
    CallPosition cp = CallPosition_New(TOKEN_NONE,ll->ev.iter);
    CallStack_Push(&ll->ev.cs,&cp);
    return False;
}
Boolean RexLang_Curly_R(RexLang* ll,TokenMap* tm){
    TT_Type tt = CallStack_Back(&ll->ev.cs);

    switch (tt){
    case TOKEN_FUNCTIONDECL:
        Function* f = RexLang_FunctionIn(ll);
        
        Scope_To(&ll->ev.sc,1);
        Scope_Pop(&ll->ev.sc);

        ll->stack += RexLang_Bytes(ll);//Return Address
        RexLang_Indentation_Append(ll,&ll->text,"ret\n");

        RexLang_Indentation_To(ll,RexLang_INDENTATION_NONE);
        String_Append(&ll->text,"\n");
        break;
    case TOKEN_REXLANG_IMPL:
        break;
    case TOKEN_REXLANG_NAMESPACE:
        break;
    case TOKEN_REXLANG_IF:
        Scope_Pop(&ll->ev.sc);
        ll->logic = TOKEN_REXLANG_IF;
        break;
    case TOKEN_REXLANG_ELIF:
        Scope_Pop(&ll->ev.sc);
        ll->logic = TOKEN_REXLANG_ELIF;
        break;
    case TOKEN_REXLANG_ELSE:
        Scope_Pop(&ll->ev.sc);
        ll->logic = TOKEN_REXLANG_ELSE;
        break;
    case TOKEN_REXLANG_WHILE:
        Scope_Pop(&ll->ev.sc);
        ll->logic = TOKEN_REXLANG_WHILE;
        break;
    case TOKEN_REXLANG_FOR:
        CallPosition* cp = (CallPosition*)Vector_Get(&ll->ev.cs,ll->ev.cs.size-1);
        Compiler_Do(&ll->ev,&cp->tm);

        Scope_Pop(&ll->ev.sc);
        ll->logic = TOKEN_REXLANG_FOR;
        break;
    case TOKEN_NONE:
        Scope_Pop(&ll->ev.sc);
        break;
    default:
        Compiler_ErrorHandler(&ll->ev,"Call Stack top type is not defined!");
        break;
    }
    CallStack_Pop(&ll->ev.cs);
    return False;
}
Boolean RexLang_Struct(RexLang* ll,TokenMap* tm){
    return False;
}
Boolean RexLang_Impl(RexLang* ll,TokenMap* tm){
    Token* t_impl = (Token*)Vector_Get(tm,0);
    Token* t_name = (Token*)Vector_Get(tm,1);
    
    if(t_name->tt==TOKEN_TYPE){
        CallPosition cp =  CallPosition_New_N(TOKEN_REXLANG_IMPL,ll->ev.iter,t_name->str);
        CallStack_Push(&ll->ev.cs,&cp);
    }else{
        Compiler_ErrorHandler(&ll->ev,"Impl: name of impl doesn't exist!");
    }
    return False;
}
Boolean RexLang_Namespace(RexLang* ll,TokenMap* tm){
    Token* t_impl = (Token*)Vector_Get(tm,0);
    Token* t_name = (Token*)Vector_Get(tm,1);
    
    if(t_name->tt==TOKEN_STRING){
        CallPosition cp =  CallPosition_New_N(TOKEN_REXLANG_NAMESPACE,ll->ev.iter,t_name->str);
        CallStack_Push(&ll->ev.cs,&cp);
    }else{
        Compiler_ErrorHandler(&ll->ev,"Impl: name of impl doesn't exist!");
    }
    return False;
}
Boolean RexLang_Function(RexLang* ll,TokenMap* tm){
    Token* tfunc = (Token*)Vector_Get(tm,0);
    
    TT_Iter it_func = FunctionMap_Find(&ll->ev.fs,tfunc->str);
    if(it_func>=0){
        Function* f = (Function*)Vector_Get(&ll->ev.fs,it_func);

        ll->ev.sc.range++;

        Type* t = Scope_FindType(&ll->ev.sc,f->rettype);
        if(t){
            RexLang_Variable_Build_Ref(ll,COMPILER_RETURN"0",f->rettype);
            
            for(int i = 0;i<f->params.size;i++){
                Member* m = (Member*)Vector_Get(&f->params,i);
                RexLang_Variable_Build_Ref(ll,m->name,m->type);
            }
            ll->stack += RexLang_Bytes(ll);//Return Address
            ll->ev.sc.range++;
        }

        RexLang_Indentation_To(ll,RexLang_INDENTATION_FUNCTION);
        
        CStr fname = RexLang_FunctionName(ll,tfunc->str);
        if(f->access & RexLang_ACCESS_GLOBAL)   String_Appendf(&ll->text,"global %s:\n",fname);
        else                                    String_Appendf(&ll->text,"%s:\n",fname);
        CStr_Free(&fname);

        CallPosition cp = CallPosition_New_N(TOKEN_FUNCTIONDECL,ll->ev.iter,tfunc->str);
        CallStack_Push(&ll->ev.cs,&cp);
    }else{
        Compiler_ErrorHandler(&ll->ev,"Function -> %s not defined!",tfunc->str);
    }

    return False;
}
Boolean RexLang_Return(RexLang* ll,TokenMap* tm){
    if(tm->size!=1){
        TokenMap cpy = TokenMap_Cpy(tm);
        Variable* v = Scope_FindVariable(&ll->ev.sc,COMPILER_RETURN"0");
        
        if(v){
            Vector_Remove(&cpy,0);
            Vector_Add(&cpy,(Token[]){ Token_By(TOKEN_STRING,v->name) },0);
            Vector_Add(&cpy,(Token[]){ Token_By(TOKEN_REXLANG_ASS,"=") },1);
        }else{
            Compiler_ErrorHandler(&ll->ev,"return: return value doesn't exist!");
            TokenMap_Free(&cpy);
            return False;
        }

        Compiler_ShutingYard(&ll->ev,&cpy);
        TokenMap_Free(&cpy);
    }

    RexLang_FromTo_DestroyOnly(ll,1,ll->ev.sc.range);
    
    /*Function* f = Compiler_FunctionIn(&ll->ev);
    if(f && f->params.size>0){
        CStr retaddress = RexLang_StackDir(ll,8,0);
        RexLang_Indentation_Appendf(ll,&ll->text,"mov\t\trax\t%s",retaddress);
        CStr_Free(&retaddress);
        RexLang_Indentation_Appendf(ll,&ll->text,"add\t\tsp\t%d",RexLang_Bytes(ll));
        Scope_Range_DestroyOnly(&ll->ev.sc,1);
        RexLang_Indentation_Appendf(ll,&ll->text,"push\trax");
    }*/
    
    //ll->stack += RexLang_Bytes(ll);//Return Address
    RexLang_Indentation_Append(ll,&ll->text,"ret\n");

    return False;
}
Boolean RexLang_Decl(RexLang* ll,TokenMap* tm){
    if(tm->size >= 2){
        Token* pottype = (Token*)Vector_Get(tm,0);
        Token* name = (Token*)Vector_Get(tm,1);
        
        if(name->tt==TOKEN_STRING){
            if(!CStr_Cmp(name->str,RexLang_SELF)){
                if(pottype->args && pottype->args->size > 0){
                    TokenMap* stm = (TokenMap*)Vector_Get(pottype->args,0);
                    Token* stok = (Token*)Vector_Get(stm,0);
                    
                    Number size = 0;
                    if(stok->tt == TOKEN_NUMBER){
                        size = Number_Parse(stok->str);

                        if(size == NUMBER_PARSE_ERROR){
                            Compiler_ErrorHandler(&ll->ev,"decl: number: \'%s\' is not valid!",stok->str);
                            return False;
                        }
                    }else{
                        Compiler_ErrorHandler(&ll->ev,"decl: size of stack var is not a number: \'%s\'!",stok->str);
                        return False;
                    }

                    RexLang_Variable_Build_DeclStack(ll,name->str,pottype->str,size * RexLang_TypeDrefPtrSizeT(ll,pottype->str));
                }else{
                    RexLang_Variable_Build_Decl(ll,name->str,pottype->str);
                }
                
                if(tm->size>2){
                    if(ll->ev.sc.range>0){
                        Token* t = (Token*)Vector_Get(tm,0);
                        Token_Free(t);
                        Vector_Remove(tm,0);
                        Boolean ret = Compiler_ShutingYard(&ll->ev,tm);
                        return ret;
                    }else{
                        Compiler_ErrorHandler(&ll->ev,"decl: global variable can't be set like this!");
                        return False;
                    }
                }
            }else
                Compiler_ErrorHandler(&ll->ev,"decl: name self is not allowed for decl!");
        }else{
            Compiler_ErrorHandler(&ll->ev,"decl: name of decl variable doesn't exist!");
            return False;
        }
    }
    return False;
}
Boolean RexLang_Continue(RexLang* ll,TokenMap* tm){
    if(tm->size==1 || tm->size==2){
        for(int i = ll->ev.cs.size - 1;i>=0;i--){
            CallPosition* cp = (CallPosition*)Vector_Get(&ll->ev.cs,i);
            if(cp->type == TOKEN_REXLANG_WHILE || cp->type == TOKEN_REXLANG_FOR){
                Compiler_Do(&ll->ev,&cp->tm);
                
                RexLang_FromTo_DestroyOnly(ll,cp->range,ll->ev.sc.range);

                CStr typecstr = cp->type == TOKEN_REXLANG_WHILE ? RexLang_WHILE : RexLang_FOR;
                CStr end_label = RexLang_Logic(ll,0,typecstr,cp->range - 1);
                RexLang_Indentation_Appendf(ll,&ll->text,"jmp\t\t%s",end_label);
                CStr_Free(&end_label);
                return False;
            }
        }
        Compiler_ErrorHandler(&ll->ev,"Break: break keyword should be used in while or for blocks!");
    }else{
        Compiler_ErrorHandler(&ll->ev,"Break: break keyword should be used correctly!");
    }
    return False;
}
Boolean RexLang_Break(RexLang* ll,TokenMap* tm){
    if(tm->size==1 || tm->size==2){
        for(int i = ll->ev.cs.size - 1;i>=0;i--){
            CallPosition* cp = (CallPosition*)Vector_Get(&ll->ev.cs,i);
            if(cp->type == TOKEN_REXLANG_WHILE || cp->type == TOKEN_REXLANG_FOR){
                RexLang_FromTo_DestroyOnly(ll,cp->range,ll->ev.sc.range);

                CStr typecstr = cp->type == TOKEN_REXLANG_WHILE ? RexLang_WHILE : RexLang_FOR;
                CStr end_label = RexLang_Logic(ll,1,typecstr,cp->range - 1);
                RexLang_Indentation_Appendf(ll,&ll->text,"jmp\t\t%s",end_label);
                CStr_Free(&end_label);
                return False;
            }
        }
        Compiler_ErrorHandler(&ll->ev,"Break: break keyword should be used in while or for blocks!");
    }else{
        Compiler_ErrorHandler(&ll->ev,"Break: break keyword should be used correctly!");
    }
    return False;
}

Boolean RexLang_Assembly(RexLang* ll,TokenMap* tm){
    int pulled = 3;
    
    for(int i = 1;i<tm->size;i++){
        Token* t = (Token*)Vector_Get(tm,i);

        if(t->tt==TOKEN_STRING){
            if(pulled == 0){
                Compiler_ErrorHandler(&ll->ev,"asm: assembly keyword can't handle more then 4 args: %s!",t->str);
                return False;
            }
            RexLang_AddressReg(ll,t,RexLang_RT[pulled]);
            pulled--;
        }
    }    

    pulled = 3;

    CStr indent = RexLang_Indentation_CStr(ll);
    String_Append(&ll->text,indent);
    CStr_Free(&indent);

    for(int i = 1;i<tm->size;i++){
        Token* t = (Token*)Vector_Get(tm,i);

        if(t->tt==TOKEN_CONSTSTRING_DOUBLE) String_Append(&ll->text,t->str);
        else if(t->tt==TOKEN_NUMBER)        String_Append(&ll->text,t->str);
        else if(t->tt==TOKEN_FLOAT)         String_Append(&ll->text,t->str);
        else if(t->tt==TOKEN_STRING){
            String_Append(&ll->text,RexLang_RT[pulled]);
            pulled--;
        }else{
            Compiler_ErrorHandler(&ll->ev,"asm: assembly keyword can't handle: %s!",t->str);
        }
    }
    String_AppendChar(&ll->text,'\n');
    return False;
}

Boolean RexLang_FunctionCall_Acs(RexLang* ll,TokenMap* tm,int i,int args,Token* tok){
    if(tm->size > 1){
        Token* accssed = (Token*)Vector_Get(tm,0);
        Token* func = (Token*)Vector_Get(tm,1);

        if(func->tt==TOKEN_FUNCTION){
            Variable* v = Scope_FindVariable(&ll->ev.sc,accssed->str);

            if(v){
                CStr type = RexLang_TypeOfDref(ll,v->typename);
                CStr oldname = CStr_Cpy(func->str);
                CStr newname = CStr_Format("%s::%s",type,func->str);
                CStr_Set((char**)&func->str,newname);

                TT_Iter it_f = FunctionMap_Find(&ll->ev.fs,func->str);
                if(it_f>=0){
                    TokenMap acs = TokenMap_Make((Token[]){
                        Token_By(TOKEN_REXLANG_ADR,"&"),
                        Token_Cpy(accssed),
                        Token_Null()
                    });

                    Vector_Add(func->args,&acs,0);
                }else{
                    CStr_Set((char**)&func->str,oldname);
                }

                CStr_Free(&newname);
                CStr_Free(&oldname);
                CStr_Free(&type);

                it_f = FunctionMap_Find(&ll->ev.fs,func->str);
                if(it_f>=0){
                    Function* f = (Function*)Vector_Get(&ll->ev.fs,it_f);

                    if(f->access || CStr_Cmp(accssed->str,RexLang_SELF)){
                        Boolean ret = Compiler_FunctionCall(&ll->ev,func);
                        if(!ret){
                            CStr retstr = Compiler_Variablename_This(&ll->ev,COMPILER_RETURN,7);
                            *tok = Token_Move(TOKEN_STRING,retstr);
                        }
                        return ret;
                    }else{
                        Compiler_ErrorHandler(&ll->ev,"Function: %s isn't pub or non self %s tries to access!",func->str,accssed->str);
                        return FUNCTIONRT_NONE;
                    }
                }
            }
        } 
    }
    return FUNCTIONRT_NONE;
}
Boolean RexLang_FunctionCall_Arw(RexLang* ll,TokenMap* tm,int i,int args,Token* tok){
    if(tm->size > 1){
        Token* accssed = (Token*)Vector_Get(tm,0);
        Token* func = (Token*)Vector_Get(tm,1);

        if(func->tt==TOKEN_FUNCTION){
            Variable* v = Scope_FindVariable(&ll->ev.sc,accssed->str);

            if(v){
                CStr type = RexLang_TypeOfPointer(ll,v->typename);
                CStr newname = CStr_Format("%s::%s",type,func->str);

                TT_Iter it_f = FunctionMap_Find(&ll->ev.fs,newname);
                if(it_f>=0){
                    CStr_Set((char**)&func->str,newname);

                    TokenMap acs = TokenMap_Make((Token[]){
                        Token_Cpy(accssed),
                        Token_Null()
                    });

                    Vector_Add(func->args,&acs,0);
                }

                CStr_Free(&newname);
                CStr_Free(&type);

                it_f = FunctionMap_Find(&ll->ev.fs,func->str);
                if(it_f>=0){
                    Function* f = (Function*)Vector_Get(&ll->ev.fs,it_f);
                    if(f->access || CStr_Cmp(accssed->str,RexLang_SELF)){
                        Boolean ret = Compiler_FunctionCall(&ll->ev,func);
                        if(!ret){
                            CStr retstr = Compiler_Variablename_This(&ll->ev,COMPILER_RETURN,7);
                            *tok = Token_Move(TOKEN_STRING,retstr);
                        }
                        return ret;
                    }else{
                        Compiler_ErrorHandler(&ll->ev,"Function: %s isn't pub or non self %s tries to access!",func->str,accssed->str);
                        return FUNCTIONRT_NONE;
                    }
                }
            }
        }
    }
    return FUNCTIONRT_NONE;
}

void RexLang_Function_Handler(RexLang* ll,Token* t,Function* f){
    if(t->tt==TOKEN_FUNCTION){
        if(f->pos>=0){
            CStr functionname = RexLang_FunctionName(ll,f->name);
            RexLang_Indentation_Appendf(ll,&ll->text,"call\t%s",functionname);
            CStr_Free(&functionname);
        }else{
            RexLang_IntoReg(ll,(Token[]){ Token_Move(TOKEN_STRING,f->name) },RexLang_REG_D);
            RexLang_Indentation_Appendf(ll,&ll->text,"call\t%s",RexLang_REG_D);
        }
    }else{
        Compiler_ErrorHandler(&ll->ev,"Function: %s doesn't exist!",t->str);
    }
}

RexLang RexLang_New(char* dllpath,char* src,char* output,char bits) {
    RexLang ll;
    ll.ev = Compiler_New(
        KeywordMap_Make((KeywordRP[]){
            KeywordRP_New("return",TOKEN_REXLANG_RETURN),
            KeywordRP_New("continue",TOKEN_REXLANG_CONTINUE),
            KeywordRP_New("break",TOKEN_REXLANG_BREAK),
            KeywordRP_New("if",TOKEN_REXLANG_IF),
            KeywordRP_New("elif",TOKEN_REXLANG_ELIF),
            KeywordRP_New("else",TOKEN_REXLANG_ELSE),
            KeywordRP_New("while",TOKEN_REXLANG_WHILE),
            KeywordRP_New("for",TOKEN_REXLANG_FOR),
            KeywordRP_New("import",TOKEN_REXLANG_IMPORT),
            KeywordRP_New("const",TOKEN_REXLANG_CONST),
            KeywordRP_New("struct",TOKEN_REXLANG_STRUCT),
            KeywordRP_New("impl",TOKEN_REXLANG_IMPL),
            KeywordRP_New("namespace",TOKEN_REXLANG_NAMESPACE),
            KeywordRP_New("define",TOKEN_REXLANG_DEFINE),
            KeywordRP_New("asm",TOKEN_REXLANG_ASMBY),
            KeywordRP_New("sizeof",TOKEN_REXLANG_SIZE),
            KeywordRP_New("pub",TOKEN_REXLANG_PUB),
            KeywordRP_New("false",TOKEN_REXLANG_BOOLEAN),
            KeywordRP_New("true",TOKEN_REXLANG_BOOLEAN),
            KeywordRP_New("null",TOKEN_REXLANG_NULL),
            KEYWORD_END
        }),
        OperatorMap_Make((OperatorRP[]){
            OperatorRP_Make((TT_Type[]){ TOKEN_EQUAL_SIGN,TOKEN_END },                                      TOKEN_REXLANG_ASS,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_PLUS_SIGN,TOKEN_END },                                       TOKEN_REXLANG_ADD,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_MINUS_SIGN,TOKEN_END },                                      TOKEN_REXLANG_SUB,1),
            OperatorRP_Make((TT_Type[]){ TOKEN_ASTERISK,TOKEN_END },                                        TOKEN_REXLANG_MUL,1),
            OperatorRP_Make((TT_Type[]){ TOKEN_SLASH,TOKEN_END },                                           TOKEN_REXLANG_DIV,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_MINUS_SIGN,TOKEN_END },                                      TOKEN_REXLANG_NEG,0),

            OperatorRP_Make((TT_Type[]){ TOKEN_AMPERSAND,TOKEN_END },                                       TOKEN_REXLANG_AND,1),
            OperatorRP_Make((TT_Type[]){ TOKEN_PIPE,TOKEN_END },                                            TOKEN_REXLANG_OR,1),
            OperatorRP_Make((TT_Type[]){ TOKEN_CARET,TOKEN_END },                                           TOKEN_REXLANG_XOR,1),
            OperatorRP_Make((TT_Type[]){ TOKEN_TILDE,TOKEN_END },                                           TOKEN_REXLANG_NOT,0),

            OperatorRP_Make((TT_Type[]){ TOKEN_AMPERSAND,TOKEN_AMPERSAND,TOKEN_END },                       TOKEN_REXLANG_LND,1),
            OperatorRP_Make((TT_Type[]){ TOKEN_PIPE,TOKEN_PIPE,TOKEN_END },                                 TOKEN_REXLANG_LOR,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_EXCLAMATION_MARK,TOKEN_END },                                TOKEN_REXLANG_LOT,ARGS_IGNORE),

            OperatorRP_Make((TT_Type[]){ TOKEN_AMPERSAND,TOKEN_END },                                       TOKEN_REXLANG_ADR,0),
            OperatorRP_Make((TT_Type[]){ TOKEN_ASTERISK,TOKEN_END },                                        TOKEN_REXLANG_DRF,0),

            OperatorRP_Make((TT_Type[]){ TOKEN_PERCENT,TOKEN_END },                                         TOKEN_REXLANG_MOD,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_PLUS_SIGN,TOKEN_PLUS_SIGN,TOKEN_END },                       TOKEN_REXLANG_INC,0),
            OperatorRP_Make((TT_Type[]){ TOKEN_MINUS_SIGN,TOKEN_MINUS_SIGN,TOKEN_END },                     TOKEN_REXLANG_DEC,0),

            OperatorRP_Make((TT_Type[]){ TOKEN_ANGLE_BRACKETS_L,TOKEN_ANGLE_BRACKETS_L,TOKEN_END },         TOKEN_REXLANG_SHL,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_ANGLE_BRACKETS_R,TOKEN_ANGLE_BRACKETS_R,TOKEN_END },         TOKEN_REXLANG_SHR,ARGS_IGNORE),

            OperatorRP_Make((TT_Type[]){ TOKEN_EQUAL_SIGN,TOKEN_EQUAL_SIGN,TOKEN_END },                     TOKEN_REXLANG_EQU,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_EXCLAMATION_MARK,TOKEN_EQUAL_SIGN,TOKEN_END },               TOKEN_REXLANG_NEQ,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_ANGLE_BRACKETS_L,TOKEN_END },                                TOKEN_REXLANG_LES,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_ANGLE_BRACKETS_R,TOKEN_END },                                TOKEN_REXLANG_GRT,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_ANGLE_BRACKETS_L,TOKEN_EQUAL_SIGN,TOKEN_END },               TOKEN_REXLANG_LEQ,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_ANGLE_BRACKETS_R,TOKEN_EQUAL_SIGN,TOKEN_END },               TOKEN_REXLANG_GRQ,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_PERIOD,TOKEN_END },                                          TOKEN_REXLANG_ACS,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_MINUS_SIGN,TOKEN_ANGLE_BRACKETS_R,TOKEN_END },               TOKEN_REXLANG_ARW,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_COLON,TOKEN_END },                                           TOKEN_REXLANG_DDOT,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_COLON,TOKEN_COLON,TOKEN_END },                               TOKEN_REXLANG_DDDOT,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_SQUARE_BRACKETS_L,TOKEN_END },                               TOKEN_REXLANG_SUBS,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_SQUARE_BRACKETS_R,TOKEN_END },                               TOKEN_REXLANG_SUBSR,ARGS_IGNORE),

            OperatorRP_Make((TT_Type[]){ TOKEN_PLUS_SIGN,TOKEN_EQUAL_SIGN,TOKEN_END },                      TOKEN_REXLANG_ASD,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_MINUS_SIGN,TOKEN_EQUAL_SIGN,TOKEN_END },                     TOKEN_REXLANG_ASU,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_ASTERISK,TOKEN_EQUAL_SIGN,TOKEN_END },                       TOKEN_REXLANG_ASM,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_SLASH,TOKEN_EQUAL_SIGN,TOKEN_END },                          TOKEN_REXLANG_ASV,ARGS_IGNORE),
    
            OperatorRP_Make((TT_Type[]){ TOKEN_SLASH,TOKEN_SLASH,TOKEN_END },                               TOKEN_REXLANG_LINECOMMENT,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_SLASH,TOKEN_ASTERISK,TOKEN_END },                            TOKEN_REXLANG_BLOCKCOMMENTSTART,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_ASTERISK,TOKEN_SLASH,TOKEN_END },                            TOKEN_REXLANG_BLOCKCOMMENTEND,ARGS_IGNORE),
            OperatorRP_End()
        }),
        DirectiveMap_Make((Directive[]){
            Directive_End()
        }),
        DirectiveMap_Make((Directive[]){
            Directive_Comment_Line(TOKEN_REXLANG_LINECOMMENT),
            Directive_Comment_Block(TOKEN_REXLANG_BLOCKCOMMENTSTART,TOKEN_REXLANG_BLOCKCOMMENTEND,True),
            Directive_Remove(TOKEN_NEWLINE),
            Directive_Remove(TOKEN_CARTURN),
            Directive_Remove(TOKEN_HTAB),
            Directive_Remove(TOKEN_VTAB),
            Directive_End()
        }),
        ReseterMap_Make((Reseter[]){ 
            // operators who might have two versions: 1 arg vs. 2 args are not listed: -
            { TOKEN_NONE,RESETER_NONE },
            { TOKEN_NUMBER,RESETER_ADD1 },
            { TOKEN_FLOAT,RESETER_ADD1 },
            { TOKEN_STRING,RESETER_ADD1 },
            { TOKEN_TYPE,RESETER_RST },
            
            { TOKEN_SPACE,RESETER_NONE },
            { TOKEN_NEWLINE,RESETER_RST },// without semicolon like python
            { TOKEN_CARTURN,RESETER_NONE },
            { TOKEN_HTAB,RESETER_NONE },
            { TOKEN_VTAB,RESETER_NONE },
    
            { TOKEN_COMMA,RESETER_RST },

            //{ TOKEN_PARENTHESES_R,RESETER_TO1 },
            { TOKEN_PARENTHESES_R,RESETER_NONE },
            { TOKEN_REXLANG_SUBSR,RESETER_NONE },
            { TOKEN_CURLY_BRACES_R,RESETER_NONE },
            //{ TOKEN_ANGLE_BRACKETS_R,RESETER_NONE }, //also < and > operator
            { TOKEN_DOUBLE_ANGLE_BRACKETS_R,RESETER_NONE },
            { TOKEN_WHITE_SQUARE_BRACKETS_R,RESETER_NONE },
            
            { TOKEN_PARENTHESES_L,RESETER_RST },
            { TOKEN_SQUARE_BRACKETS_L,RESETER_RST },
            { TOKEN_CURLY_BRACES_L,RESETER_RST },
            //{ TOKEN_ANGLE_BRACKETS_L,RESETER_RST }, //also < and > operator
            { TOKEN_DOUBLE_ANGLE_BRACKETS_L,RESETER_RST },
            { TOKEN_WHITE_SQUARE_BRACKETS_L,RESETER_RST },
            { TOKEN_SEMICOLON,RESETER_RST },
            { TOKEN_COLON,RESETER_RST },
            { TOKEN_EXCLAMATION_MARK,RESETER_RST },
            { TOKEN_QUESTION_MARK,RESETER_RST },
            { TOKEN_ELLIPSIS,RESETER_RST },
            { TOKEN_PLUS_SIGN,RESETER_RST },
            { TOKEN_MULTIPLICATION_SIGN,RESETER_RST },
            { TOKEN_DIVISION_SIGN,RESETER_RST },
            { TOKEN_EQUAL_SIGN,RESETER_RST },
            { TOKEN_NOT_EQUAL_SIGN,RESETER_RST },
            { TOKEN_LESS_THAN,RESETER_RST },
            { TOKEN_GREATER_THAN,RESETER_RST },
            { TOKEN_LESS_THAN_OR_EQUAL_TO,RESETER_RST },
            { TOKEN_GREATER_THAN_OR_EQUAL_TO,RESETER_RST },
            { TOKEN_APPROXIMATION_SIGN,RESETER_RST },
            { TOKEN_PLUS_MINUS_SIGN,RESETER_RST },
            { TOKEN_AMPERSAND,RESETER_RST },
            { TOKEN_AT_SIGN,RESETER_RST },
            { TOKEN_ASTERISK,RESETER_RST },
            { TOKEN_CARET,RESETER_RST },
            { TOKEN_TILDE,RESETER_RST },
            { TOKEN_SLASH,RESETER_RST },
            { TOKEN_BACKSLASH,RESETER_RST },
            { TOKEN_UNDERSCORE,RESETER_RST },
            { TOKEN_PIPE,RESETER_RST },
            { TOKEN_PERCENT,RESETER_RST },
            { TOKEN_HASH_POUND_SIGN,RESETER_RST },
            { TOKEN_SINGLE_QUOTE,RESETER_RST },
            { TOKEN_DOUBLE_QUOTE,RESETER_RST },
            { TOKEN_BACKTICK,RESETER_RST },
            { TOKEN_PRIME,RESETER_RST },
            { TOKEN_DOUBLE_PRIME,RESETER_RST },
            { TOKEN_SECTION_SYMBOL,RESETER_RST },
            { TOKEN_DEGREE_SYMBOL,RESETER_RST },
            { TOKEN_FRACTION_SLASH,RESETER_RST },
            { TOKEN_INTERROBANG,RESETER_RST },
            { TOKEN_BULLET_POINT,RESETER_RST },
    
            { TOKEN_CAST,RESETER_NONE },
            { TOKEN_FUNCTION,RESETER_ADD1 },
            { TOKEN_CONSTSTRING_SINGLE,RESETER_ADD1 },
            { TOKEN_CONSTSTRING_DOUBLE,RESETER_ADD1 },

            { TOKEN_REXLANG_NULL,RESETER_ADD1 },
            { TOKEN_REXLANG_BOOLEAN,RESETER_ADD1 },

            { TOKEN_REXLANG_RETURN,RESETER_RST },
            { TOKEN_END,RESETER_RST }
        }),
        ShutingYard_New(
            PrecedenceMap_Make((Precedencer[]){
                Precedencer_New(TOKEN_REXLANG_ASS,1),
                Precedencer_New(TOKEN_REXLANG_ADD,6),
                Precedencer_New(TOKEN_REXLANG_SUB,5),
                Precedencer_New(TOKEN_REXLANG_MUL,7),
                Precedencer_New(TOKEN_REXLANG_DIV,8),
                Precedencer_New(TOKEN_REXLANG_MOD,9),
                Precedencer_New(TOKEN_REXLANG_NEG,12),
                Precedencer_New(TOKEN_REXLANG_INC,12),
                Precedencer_New(TOKEN_REXLANG_DEC,12),

                Precedencer_New(TOKEN_REXLANG_AND,7),
                Precedencer_New(TOKEN_REXLANG_OR,7),
                Precedencer_New(TOKEN_REXLANG_XOR,7),
                Precedencer_New(TOKEN_REXLANG_NOT,8),

                Precedencer_New(TOKEN_REXLANG_LND,2),
                Precedencer_New(TOKEN_REXLANG_LOR,2),
                Precedencer_New(TOKEN_REXLANG_LOT,4),

                Precedencer_New(TOKEN_REXLANG_SHL,7),
                Precedencer_New(TOKEN_REXLANG_SHR,7),

                Precedencer_New(TOKEN_REXLANG_EQU,4),
                Precedencer_New(TOKEN_REXLANG_NEQ,4),
                Precedencer_New(TOKEN_REXLANG_LES,4),
                Precedencer_New(TOKEN_REXLANG_GRT,4),
                Precedencer_New(TOKEN_REXLANG_LEQ,4),
                Precedencer_New(TOKEN_REXLANG_GRQ,4),

                Precedencer_New(TOKEN_REXLANG_ACS,13), 
                Precedencer_New(TOKEN_REXLANG_ARW,13),
                Precedencer_New(TOKEN_REXLANG_ADR,10),
                Precedencer_New(TOKEN_REXLANG_DRF,10),
                Precedencer_New(TOKEN_REXLANG_DDOT,PRECEDENCE_DONTCARE),
                Precedencer_New(TOKEN_REXLANG_DDDOT,PRECEDENCE_DONTCARE),
                Precedencer_New(TOKEN_REXLANG_SUBS,13),

                Precedencer_New(TOKEN_REXLANG_ASD,1),
                Precedencer_New(TOKEN_REXLANG_ASU,1),
                Precedencer_New(TOKEN_REXLANG_ASM,1),
                Precedencer_New(TOKEN_REXLANG_ASV,1),

                Precedencer_New(TOKEN_CAST,10),
                Precedencer_New(TOKEN_REXLANG_SIZE,13),
                Precedencer_New(TOKEN_PARENTHESES_L,PRECEDENCE_BRACKL),
                Precedencer_New(TOKEN_PARENTHESES_R,PRECEDENCE_BRACKR),
                Precedencer_New(TOKEN_REXLANG_LINECOMMENT,PRECEDENCE_DONTCARE),
                Precedencer_New(TOKEN_REXLANG_BLOCKCOMMENTSTART,PRECEDENCE_DONTCARE),
                Precedencer_New(TOKEN_REXLANG_BLOCKCOMMENTEND,PRECEDENCE_DONTCARE),
                Precedencer_End
            }),
            ExecuteMap_Make((Executer[]){
                Executer_New(TOKEN_REXLANG_ASS,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_ADD,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_SUB,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_MUL,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_DIV,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_MOD,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_NEG,1,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_INC,1,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_DEC,1,Scope_StdHandler),

                Executer_New(TOKEN_REXLANG_AND,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_OR,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_XOR,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_NOT,1,Scope_StdHandler),

                Executer_New(TOKEN_REXLANG_LND,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_LOR,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_LOT,1,Scope_StdHandler),

                Executer_New(TOKEN_REXLANG_SHL,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_SHR,2,Scope_StdHandler),

                Executer_New(TOKEN_REXLANG_EQU,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_NEQ,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_LES,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_GRT,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_LEQ,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_GRQ,2,Scope_StdHandler),

                Executer_New(TOKEN_REXLANG_ACS,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_ARW,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_ADR,1,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_DRF,1,Scope_StdHandler),
                
                Executer_New(TOKEN_REXLANG_DDOT,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_DDDOT,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_SUBS,2,Scope_StdHandler),

                Executer_New(TOKEN_REXLANG_ASD,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_ASU,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_ASM,2,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_ASV,2,Scope_StdHandler),

                Executer_New(TOKEN_REXLANG_INC,1,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_DEC,1,Scope_StdHandler),

                Executer_New(TOKEN_CAST,1,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_SIZE,1,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_LINECOMMENT,ARGS_IGNORE,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_BLOCKCOMMENTSTART,ARGS_IGNORE,Scope_StdHandler),
                Executer_New(TOKEN_REXLANG_BLOCKCOMMENTEND,ARGS_IGNORE,Scope_StdHandler),
                Executer_End
            }),
            PreexecuteMap_Make((Preexecuter[]){
                Preexecuter_New(TOKEN_REXLANG_ACS,(void*)RexLang_FunctionCall_Acs),
                Preexecuter_New(TOKEN_REXLANG_ARW,(void*)RexLang_FunctionCall_Arw),
                Preexecuter_End
            })
        ),
        Scope_Make(
            (StdConstType[]){
                StdConstType_New(TOKEN_NUMBER,I16_TYPE),
                //StdConstType_New(TOKEN_FLOAT,F64_TYPE),
                StdConstType_New(TOKEN_REXLANG_BOOLEAN,BOOL_TYPE),
                StdConstType_New(TOKEN_CONSTSTRING_DOUBLE,STR_TYPE),
                StdConstType_New(TOKEN_CONSTSTRING_SINGLE,CHAR_TYPE),
                StdConstType_New(TOKEN_FUNCTIONPOINTER,NULL_TYPE),
                StdConstType_New(TOKEN_REXLANG_NULL,NULL_TYPE),
                STDCONSTTYPE_END
            },
            VariableMap_Make((Variable[]){
                //Variable_Make("z","int",(Number[]){ 69 },8,0,NULL,NULL),
                //Variable_Make("b","int",(Number[]){ 31 },8,0),
                VARIABLE_END
            }),
            TypeMap_Make((Type[]){
                TYPE_END
            }),
        (void*)Environment_ErrorHandler),
        RangeChangerMap_Make((RangeChanger[]){
            //RangeChanger_New(TOKEN_REXLANG_IF,1),
            //RangeChanger_New(TOKEN_REXLANG_WHILE,1),
            //RangeChanger_New(TOKEN_REXLANG_FOR,1),
            //RangeChanger_New(TOKEN_CURLY_BRACES_L,-1),
            //RangeChanger_New(TOKEN_CURLY_BRACES_R,-1),
            RANGECHANGER_END
        }),
        ExternPackageMap_Make((ExternPackage[]){
            EXTERNPACKAGE_END
        }),
        KeywordExecuterMap_Make((KeywordExecuter[]){
            KeywordExecuter_New(TOKEN_TYPE,                (void*)RexLang_Compress),
            KeywordExecuter_New(TOKEN_STRING,              (void*)RexLang_Compress),
            KeywordExecuter_New(TOKEN_PARENTHESES_L,       (void*)RexLang_Compress),
            KeywordExecuter_New(TOKEN_REXLANG_INC,         (void*)RexLang_Compress),
            KeywordExecuter_New(TOKEN_REXLANG_DEC,         (void*)RexLang_Compress),
            KeywordExecuter_New(TOKEN_REXLANG_RETURN,      (void*)RexLang_Compress),
            //KeywordExecuter_New(TOKEN_REXLANG_CONTINUE,    (void*)RexLang_Compress),
            //KeywordExecuter_New(TOKEN_REXLANG_BREAK,       (void*)RexLang_Compress),
            KeywordExecuter_New(TOKEN_REXLANG_IF,          (void*)RexLang_Compress),
            KeywordExecuter_New(TOKEN_REXLANG_ELIF,        (void*)RexLang_Compress),
            KeywordExecuter_New(TOKEN_REXLANG_ELSE,        (void*)RexLang_Compress),
            KeywordExecuter_New(TOKEN_REXLANG_WHILE,       (void*)RexLang_Compress),
            KeywordExecuter_New(TOKEN_REXLANG_FOR,         (void*)RexLang_Compress),
            KeywordExecuter_New(TOKEN_PARENTHESES_L,       (void*)RexLang_Compress),
            KeywordExecuter_New(TOKEN_REXLANG_DRF,         (void*)RexLang_Compress),
            KeywordExecuter_New(TOKEN_REXLANG_PUB,         (void*)RexLang_Compress),

            KeywordExecuter_New(TOKEN_REXLANG_STRUCT,      (void*)RexLang_Compress_staticmethods),
            KeywordExecuter_New(TOKEN_REXLANG_STRUCT,      (void*)RexLang_Compress_pointer),
            KeywordExecuter_New(TOKEN_REXLANG_IMPL,        (void*)RexLang_Compress_staticmethods),
            KeywordExecuter_New(TOKEN_REXLANG_IMPL,        (void*)RexLang_Compress_pointer),

            KeywordExecuter_New(TOKEN_REXLANG_STRUCT,      (void*)RexLang_PP_Struct),
            KeywordExecuter_New(TOKEN_REXLANG_IMPL,        (void*)RexLang_PP_Impl),
            KeywordExecuter_New(TOKEN_REXLANG_NAMESPACE,   (void*)RexLang_PP_Namespace),
            KeywordExecuter_New(TOKEN_REXLANG_IF,          (void*)RexLang_PP_if),
            KeywordExecuter_New(TOKEN_REXLANG_ELIF,        (void*)RexLang_PP_elif),
            KeywordExecuter_New(TOKEN_REXLANG_ELSE,        (void*)RexLang_PP_else),
            KeywordExecuter_New(TOKEN_REXLANG_WHILE,       (void*)RexLang_PP_while),
            KeywordExecuter_New(TOKEN_REXLANG_FOR,         (void*)RexLang_PP_for),
            KeywordExecuter_New(TOKEN_CURLY_BRACES_L,      (void*)RexLang_PP_Curly_L),
            KeywordExecuter_New(TOKEN_CURLY_BRACES_R,      (void*)RexLang_PP_Curly_R),
            KeywordExecuter_New(TOKEN_REXLANG_IMPORT,      (void*)RexLang_Import),
            KeywordExecuter_New(TOKEN_REXLANG_DEFINE,      (void*)RexLang_Define),
            KEYWORDEXECUTER_END
        }),
        KeywordExecuterMap_Make((KeywordExecuter[]){
            KeywordExecuter_New(TOKEN_TYPE,                (void*)RexLang_Decl),
            KeywordExecuter_New(TOKEN_PARENTHESES_L,       (void*)Compiler_ShutingYard),
            KeywordExecuter_New(TOKEN_REXLANG_DRF,         (void*)Compiler_ShutingYard),
            KeywordExecuter_New(TOKEN_REXLANG_INC,         (void*)Compiler_ShutingYard),
            KeywordExecuter_New(TOKEN_REXLANG_DEC,         (void*)Compiler_ShutingYard),
            KeywordExecuter_New(TOKEN_CURLY_BRACES_L,      (void*)RexLang_Curly_L),
            KeywordExecuter_New(TOKEN_CURLY_BRACES_R,      (void*)RexLang_Curly_R),
            KeywordExecuter_New(TOKEN_REXLANG_IF,          (void*)RexLang_If),
            KeywordExecuter_New(TOKEN_REXLANG_ELIF,        (void*)RexLang_Elif),
            KeywordExecuter_New(TOKEN_REXLANG_ELSE,        (void*)RexLang_Else),
            KeywordExecuter_New(TOKEN_REXLANG_WHILE,       (void*)RexLang_While),
            KeywordExecuter_New(TOKEN_REXLANG_FOR,         (void*)RexLang_For),
            KeywordExecuter_New(TOKEN_REXLANG_RETURN,      (void*)RexLang_Return),
            KeywordExecuter_New(TOKEN_REXLANG_CONTINUE,    (void*)RexLang_Continue),
            KeywordExecuter_New(TOKEN_REXLANG_BREAK,       (void*)RexLang_Break),
            KeywordExecuter_New(TOKEN_REXLANG_STRUCT,      (void*)RexLang_Struct),
            KeywordExecuter_New(TOKEN_REXLANG_IMPL,        (void*)RexLang_Impl),
            KeywordExecuter_New(TOKEN_REXLANG_NAMESPACE,   (void*)RexLang_Namespace),
            KeywordExecuter_New(TOKEN_FUNCTIONDECL,         (void*)RexLang_Function),
            KeywordExecuter_New(TOKEN_REXLANG_ASMBY,       (void*)RexLang_Assembly),
            KEYWORDEXECUTER_END
        }),
        DTT_TypeMap_Make((DTT_Type[]){
            DTT_Type_New(TOKEN_SEMICOLON,Token_Null()),
            DTT_Type_New(TOKEN_CURLY_BRACES_L,Token_By(TOKEN_CURLY_BRACES_L,"{")),
            DTT_Type_New(TOKEN_CURLY_BRACES_R,Token_By(TOKEN_CURLY_BRACES_R,"}")),
            DTT_TYPE_END
        }),
        Token_By(TOKEN_REXLANG_ASS,"="),
        (void*)RexLang_LogicCorrection,
        (void*)RexLang_Function_Handler
    );
    ll.bits = bits;
    ll.logic = TOKEN_NONE;
    ll.stack = 0U;
    ll.indent = 0U;
    ll.lcmps = 0U;
    ll.src = CStr_Cpy(src);
    ll.output = CStr_Cpy(output);
    ll.dllpath = CStr_Cpy(dllpath);

    ll.bss = String_New();
    ll.data = String_New();
    ll.text = String_New();

    ll.externs = CVector_New(sizeof(CStr),(void*)CStr_Free,NULL,(void*)CStr_PCmp,(void*)CStr_print);
    ll.globals = CVector_New(sizeof(CStr),(void*)CStr_Free,NULL,(void*)CStr_PCmp,(void*)CStr_print);
    ll.filesstack = CVector_New(sizeof(CStr),(void*)CStr_Free,NULL,(void*)CStr_PCmp,(void*)CStr_print);
    ll.filesinc = CVector_New(sizeof(CStr),(void*)CStr_Free,NULL,(void*)CStr_PCmp,(void*)CStr_print);
    ll.constsstr = CVector_New(sizeof(CStr),(void*)CStr_Free,NULL,(void*)CStr_PCmp,(void*)CStr_print);
    ll.defines = CVector_New(sizeof(Define),(void*)Define_Free,NULL,NULL,(void*)Define_Print);

    ll.logicpath = Vector_New(sizeof(LogicBlock));

    Vector_Push(&ll.ev.epm,(ExternPackage[]){ ExternPackage_Make(ll.dllpath,VOID_TYPE,"Ex_Packer",  (CStr[]){ VOID_TYPE,    NULL },&ll.ev.sc) });
    Vector_Push(&ll.ev.epm,(ExternPackage[]){ ExternPackage_Make(ll.dllpath,BOOL_TYPE,"Ex_Packer",  (CStr[]){ BOOL_TYPE,    NULL },&ll.ev.sc) });
    Vector_Push(&ll.ev.epm,(ExternPackage[]){ ExternPackage_Make(ll.dllpath,I8_TYPE,  "Ex_Packer",  (CStr[]){ I8_TYPE,      NULL },&ll.ev.sc) });
    Vector_Push(&ll.ev.epm,(ExternPackage[]){ ExternPackage_Make(ll.dllpath,I16_TYPE, "Ex_Packer",  (CStr[]){ I16_TYPE,     NULL },&ll.ev.sc) });
    Vector_Push(&ll.ev.epm,(ExternPackage[]){ ExternPackage_Make(ll.dllpath,U8_TYPE,  "Ex_Packer",  (CStr[]){ U8_TYPE,      NULL },&ll.ev.sc) });
    Vector_Push(&ll.ev.epm,(ExternPackage[]){ ExternPackage_Make(ll.dllpath,U16_TYPE, "Ex_Packer",  (CStr[]){ U16_TYPE,     NULL },&ll.ev.sc) });
    Vector_Push(&ll.ev.epm,(ExternPackage[]){ ExternPackage_Make(ll.dllpath,"pointer","Ex_Packer",  (CStr[]){ POINTER_TYPE, NULL },&ll.ev.sc) });
    Vector_Push(&ll.ev.epm,(ExternPackage[]){ ExternPackage_Make(ll.dllpath,"dref",   "Ex_Packer",  (CStr[]){ DREF_TYPE,    NULL },&ll.ev.sc) });
    Vector_Push(&ll.ev.epm,(ExternPackage[]){ ExternPackage_Make(ll.dllpath,"str",    "Ex_Packer",  (CStr[]){ STR_TYPE,     NULL },&ll.ev.sc) });
    Vector_Push(&ll.ev.epm,(ExternPackage[]){ ExternPackage_Make(ll.dllpath,"struct", "Ex_Packer",  (CStr[]){ STRUCT_TYPE,  NULL },&ll.ev.sc) });

    CVector_Push(&ll.filesstack,(CStr[]){ CStr_Cpy(src) });
    CVector_Push(&ll.filesinc,(CStr[]){ CStr_Cpy(src) });
    Compiler_Script(&ll.ev,src);
    CVector_PopTop(&ll.filesstack);
    return ll;
}
void RexLang_PrintVariable(RexLang* ll,Variable* v) {
    Scope_PrintVariableDirect(&ll->ev.sc,v);
}
void RexLang_Construct_EntryPoint(RexLang* ll) {
    CVector_Push(&ll->globals,(CStr[]){ CStr_Cpy("_start") });
}
void RexLang_Build_Externs(RexLang* ll,String* str) {
    for(int i = 0;i<ll->externs.size;i++){
        CStr name = *(CStr*)CVector_Get(&ll->externs,i);
        String_Append(str,"extern ");
        String_Append(str,name);
        String_Append(str,"\n");
    }
    if(ll->externs.size>0) String_Append(str,"\n");
}
void RexLang_Build_Globals(RexLang* ll,String* str) {
    //for(int i = 0;i<ll->globals.size;i++){
    //    CStr name = *(CStr*)CVector_Get(&ll->globals,i);
    //    String_Append(str,"");
    //    String_Append(str,name);
    //    String_Append(str,"\n");
    //}
    //if(ll->globals.size>0) String_Append(str,"\n");
}
void RexLang_Build_EntryPoint(RexLang* ll) {
    String_Append(&ll->text,"\nglobal _start:\n");
    String_Appendf(&ll->text,"%ssub\t\tsp\t%d\n",RexLang_INDENTATION,RexLang_Bytes(ll));
    
    CStr fmain = RexLang_FunctionName(ll,"main");
    String_Appendf(&ll->text,"%scall\t%s\n",RexLang_INDENTATION,fmain);
    CStr_Free(&fmain);
    
    String_Appendf(&ll->text,"%sld\t\tr0\tsp\n",RexLang_INDENTATION);
    String_Appendf(&ll->text,"%sadd\t\tsp\t%d\n",RexLang_INDENTATION,RexLang_Bytes(ll));
    String_Appendf(&ll->text,"%smov\t\t$0\tr0\n\n",RexLang_INDENTATION);
}
void RexLang_Build(RexLang* ll) {
    RexLang_Construct_EntryPoint(ll);
    RexLang_Build_EntryPoint(ll);

    Compiler_Begin(&ll->ev);
    
    if(!ll->ev.error){
        String output = String_Format(";|\n;| RexLang by codeleaded\n;| Arch: VM16\n;| bits %d\n;|\n",ll->bits);
        //String_Append(&output,"\nsection .bss\n");
        //String_AppendString(&output,&ll->bss);
        //String_Append(&output,"\nsection .data\n");
        //String_AppendString(&output,&ll->data);
        //String_Append(&output,"\nsection .text\n");
        //RexLang_Build_Externs(ll,&output);
        //RexLang_Build_Globals(ll,&output);
        String_AppendString(&output,&ll->text);
        String_AppendString(&output,&ll->bss);
        String_AppendString(&output,&ll->data);

        Files_WriteT(ll->output,output.Memory,output.size);
        String_Free(&output);
    }else{
        Compiler_FlushLogs(&ll->ev);
        printf("%s-> %sbuild aborted because of Errors!%s\n",ANSI_FG_GRAY,ANSI_FG_RED,ANSI_FG_WHITE);
    }
}
void RexLang_Free(RexLang* ll) {
    String_Free(&ll->bss);
    String_Free(&ll->data);
    String_Free(&ll->text);

    CVector_Free(&ll->externs);
    CVector_Free(&ll->globals);
    
    CStr_Free(&ll->dllpath);
    CStr_Free(&ll->src);
    CStr_Free(&ll->output);

    Compiler_Free(&ll->ev);

    CVector_Free(&ll->filesstack);
    CVector_Free(&ll->filesinc);
    CVector_Free(&ll->constsstr);
    CVector_Free(&ll->defines);

    Vector_Free(&ll->logicpath);
}
void RexLang_Print(RexLang* ll) {
    printf("--- RexLang ---\n");
    printf("Stack: %d [%d]\n",ll->stack,ll->bits);
    printf("dlls: %s, src: %s, output: %s\n",ll->dllpath,ll->src,ll->output);
    CVector_Print(&ll->externs);
    CVector_Print(&ll->globals);
    CVector_Print(&ll->filesinc);
    CVector_Print(&ll->constsstr);
    CVector_Print(&ll->defines);
    Compiler_Print(&ll->ev);
    printf("--------------------------\n");
}

#endif