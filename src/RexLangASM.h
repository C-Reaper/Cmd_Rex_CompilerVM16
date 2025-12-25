#ifndef RexLangASM
#define RexLangASM

#include "/home/codeleaded/System/Static/Container/CVector.h"
#include "/home/codeleaded/System/Static/Library/AlxScope.h"
#include "/home/codeleaded/System/Static/Library/AlxShutingYard.h"
#include "/home/codeleaded/System/Static/Library/AlxCompiler.h"
#include "/home/codeleaded/System/Static/Library/ConstParser.h"
#include "/home/codeleaded/System/Static/Library/Files.h"
#include "/home/codeleaded/System/Static/Library/String.h"

#include "RexLangDefines.h"


typedef struct LogicBlock {
    int count;
    int extend;
} LogicBlock;

typedef struct Define {
    CStr name;
    TokenMap content;
} Define;

Define Define_New(CStr name,TokenMap tm){
    Define d;
    d.name = CStr_Cpy(name);
    d.content = tm;
    return d;
}
void Define_Free(Define* d){
    CStr_Free(&d->name);
    TokenMap_Free(&d->content);
}
void Define_Print(Define* d){
    printf("%s: ",d->name);
    TokenMap_Print_S(&d->content);
}


typedef struct RexLang {
    Compiler ev; // like inheritance
    CVector filesstack; // call stack of files -> path
    CVector filesinc;
    String bss;
    String data;
    String text;
    CVector externs;
    CVector globals;
    CVector constsstr;
    CVector defines; // Vector<Define>
    Vector logicpath; // Vector<LogicBlock>
    char* dllpath;
    char* src;
    char* output;
    char bits;
    char PADD1;
    TT_Type logic;
    unsigned int stack;
    unsigned int indent;
    unsigned int lcmps;
} RexLang;

int RexLang_Bytes(RexLang* ll){
    return ll->bits / 8;
}
int RexLang_Indentation(RexLang* ll){
    return I32_Min(1,ll->indent);
}
void RexLang_Indentation_To(RexLang* ll,int indent){
    ll->indent = indent;
}

CStr RexLang_Indentation_CStr(RexLang* ll){
    String builder = String_New();

    int Indentation = RexLang_Indentation(ll);
    for(int i = 0;i<Indentation;i++) String_Append(&builder,RexLang_INDENTATION);

    CStr cstr = String_CStr(&builder);
    String_Free(&builder);
    return cstr;
}
void RexLang_Indentation_Do(RexLang* ll,String* str){
    if(RexLang_Indentation(ll)>0){
        CStr indent = RexLang_Indentation_CStr(ll);
        String_Append(&ll->text,indent);
        CStr_Free(&indent);
    }
}
void RexLang_Indentation_Append(RexLang* ll,String* str,char* cstr){
    RexLang_Indentation_Do(ll,str);
    String_Append(str,cstr);
}
void RexLang_Indentation_Appendf(RexLang* ll,String* str,char* FormatCStr,...){
    RexLang_Indentation_Do(ll,str);
    
    va_list args;
    va_start(args,FormatCStr);
    String app = String_FormatA(FormatCStr,args);
    String_AppendString(str,&app);
    String_AppendChar(str,'\n');
    String_Free(&app);
    va_end(args);
}

Boolean RexLang_PointerType(RexLang* ll,CStr name){
    int size = CStr_Size(name);
    for(int i = size-1;i>=0;i--){
        if(name[i]=='*') return 1;
        if(name[i]=='&') continue;
        return 0;
    }
    return 0;
}
Boolean RexLang_DrefType(RexLang* ll,CStr name){
    if(!name) return 0;
    
    int size = CStr_Size(name);
    if(name[size-1]=='&') return 1;
    return 0;
}
int RexLang_DrefTypeCount(RexLang* ll,CStr name){
    int size = CStr_Size(name);
    for(int i = size - 1;i>=0;i--)
        if(name[i]!='&') return (size - 1) - i;
    return size;
}
CStr RexLang_TypeOfDref(RexLang* ll,CStr name){
    int size = CStr_Size(name);
    for(int i = size-1;i>=0;i--){
        if(name[i]=='&') continue;
        return CStr_Cpy_From_To(name,0,i+1);
    }
    return NULL;
}
CStr RexLang_TypeOfPointer(RexLang* ll,CStr name){
    int size = CStr_Size(name);
    for(int i = size-1;i>=0;i--){
        if(name[i]=='&' || name[i]=='*') continue;
        return CStr_Cpy_From_To(name,0,i+1);
    }
    return NULL;
}
int RexLang_Size(RexLang* ll,CStr name){
    if(RexLang_DrefType(ll,name)){
        return RexLang_Bytes(ll);
    }else{
        Type* t = Scope_FindType(&ll->ev.sc,name);
        if(t){
            Token op = Token_By(TOKEN_REXLANG_SIZE,"sizeof");
            
            Vector othertypes = Vector_New(sizeof(CStr));
            Token (*Handler)(void*,Token*,Vector*) = Scope_HandlerOf(&ll->ev.sc,&op,name,&othertypes);
            //for(int i = 0;i<types.size;i++) CStr_Free((CStr*)Vector_Get(&types,i));
            Vector_Free(&othertypes);
        
            if(Handler){
                TokenMap args = TokenMap_Make((Token[]){ Token_By(TOKEN_STRING,t->name),Token_Null() });
                Token Item = Handler(ll,&op,&args);
                TokenMap_Free(&args);
                Token_Free(&op);
            
                Number size = Number_Parse(Item.str);
                Token_Free(&Item);
                return size;
            }
            Token_Free(&op);
        }
    }
    return 0;
}

char RexLang_CompressStackType(RexLang* ll,TokenMap* tm,int start){
    Token* type = (Token*)Vector_Get(tm,start);
    
    int pret = 0;

    for(int i = start + 1;i<tm->size;i++){
        Token* tok = (Token*)Vector_Get(tm,i);
        
        if(tok->tt == TOKEN_REXLANG_SUBS) pret++;
        if(tok->tt == TOKEN_REXLANG_SUBSR){
            pret--;

            if(pret == 0){
                type->args = (Vector*)malloc(sizeof(Vector));
                *type->args = Vector_New(sizeof(TokenMap));
                Vector_Push(type->args,(TokenMap[]){ TokenMap_Sub(tm,start + 2,i) });
                TokenMap_Remove(tm,start + 2,i + 1);
                return 1;
            }else if(pret < 0){
                return 0;
            }
        }
    }
    return 0;
}

CStr RexLang_TypeSelector(RexLang* ll,int size){
    if(size==1) return CStr_Cpy(RexLang_DREF_8);
    if(size==2) return CStr_Cpy(RexLang_DREF_16);
    if(size==4) return CStr_Cpy(RexLang_DREF_32);
    if(size==8) return CStr_Cpy(RexLang_DREF_64);
    return NULL;
}
CStr RexLang_TypeSelector_T(RexLang* ll,CStr name){
    if(RexLang_DrefType(ll,name))  return RexLang_TypeSelector(ll,RexLang_Bytes(ll)); 
    else                            return RexLang_TypeSelector(ll,RexLang_Size(ll,name));
}
CStr RexLang_StackDir(RexLang* ll,int size,int stack){
    CStr selector = RexLang_TypeSelector(ll,size);
    CStr cstr = CStr_Format("%s[rsp + %d]",selector,stack);
    CStr_Free(&selector);
    return cstr;
}
CStr RexLang_StackAtS(RexLang* ll,int size,int stack){
    CStr selector = RexLang_TypeSelector(ll,size);
    CStr cstr = CStr_Format("%s[rsp + %d]",selector,ll->stack - stack);
    CStr_Free(&selector);
    return cstr;
}
CStr RexLang_StackAt(RexLang* ll,char* typename,int stack){
    CStr selector = RexLang_TypeSelector_T(ll,typename);
    CStr cstr = CStr_Format("%s[rsp + %d]",selector,ll->stack - stack);
    CStr_Free(&selector);
    return cstr;
}
CStr RexLang_StackAtNT(RexLang* ll,int stack){
    CStr cstr = CStr_Format("[rsp + %d]",ll->stack - stack);
    return cstr;
}
CStr RexLang_GetGlobal(RexLang* ll,CStr name){
    CStr realname = CStr_Format("g.%s",name);
    return realname;
}
CStr RexLang_GlobalAt(RexLang* ll,CStr name,char* typename){
    CStr selector = RexLang_TypeSelector_T(ll,typename);
    CStr realname = RexLang_GetGlobal(ll,name);
    CStr ret = CStr_Format("%s[%s]",selector,realname);
    CStr_Free(&realname);
    CStr_Free(&selector);
    return ret;
}
CStr RexLang_Location(RexLang* ll,char* name){
    Variable* v = Scope_FindVariable(&ll->ev.sc,name);
    if(v){
        RexLangVariable* sv = (RexLangVariable*)Variable_Data(v);
        if(v->range>0){
            return RexLang_StackAt(ll,v->typename,sv->stack);
        }else if(sv->global){
            return RexLang_GlobalAt(ll,v->name,v->typename);
        }
    }
    return NULL;
}

CStr RexLang_Variablename_Next(RexLang* ll,CStr name,int offset){
    CStr build = CStr_Concat(name,"*");

    Number stack_name_count = Scope_FindVariablenameLastLike(&ll->ev.sc,build,'*',offset);
    if(stack_name_count==NUMBER_PARSE_ERROR)    stack_name_count = 0;
    else                                        stack_name_count++;


    CStr_Free(&build);
    CStr stack_name = CStr_Format("%s%d",name,stack_name_count);
    return stack_name;
}
CStr RexLang_Conststr_Next(RexLang* ll,CStr name,int offset){
    CStr build = CStr_Concat(name,"*");

    Number stack_name_count = ll->constsstr.size;

    CStr_Free(&build);
    CStr stack_name = CStr_Format("%s%d",name,stack_name_count);
    return stack_name;
}
CStr RexLang_ConstStr(RexLang* ll,CStr name){
    int size = CStr_Size(name);
    String value = String_New();
    String buffer = String_New();
    for(int i = 0;i<size;i++){
        if(name[i]>=32 && name[i]<127){
            String_AppendChar(&buffer,name[i]);
        }else{
            if(buffer.size>0){
                String_AppendChar(&value,'\"');
                String_AppendString(&value,&buffer);
                String_AppendChar(&value,'\"');
                String_AppendChar(&value,',');
                String_Clear(&buffer);
            }
            String_AppendNumber(&value,name[i]);
            String_AppendChar(&value,',');
        }

        if(i == size - 1){
            if(buffer.size>0){
                String_AppendChar(&value,'\"');
                String_AppendString(&value,&buffer);
                String_AppendChar(&value,'\"');
                String_AppendChar(&value,',');
            }
        }
    }
    String_AppendNumber(&value,0);
    
    CStr cstr_value = String_CStr(&value);
    String_Free(&value);
    String_Free(&buffer);
    return cstr_value;
}
CStr RexLang_BuildConstStr(RexLang* ll,CStr cstr){
    CStr name = RexLang_Conststr_Next(ll,"GLOBAL_STR",10);
    CVector_Push(&ll->constsstr,(CStr[]){ CStr_Cpy(name) });
    
    CStr cstr_value = RexLang_ConstStr(ll,cstr);
    String_Appendf(&ll->data,"%s: db %s\n",name,cstr_value);
    CStr_Free(&cstr_value);
    return name;
}
CStr RexLang_BuildGlobal(RexLang* ll,CStr name,int size){
    CStr g_name = RexLang_GetGlobal(ll,name);
    String_Appendf(&ll->bss,"%s: resb %d\n",g_name,size);
    return g_name;
}

void RexLang_Variable_BuildXX(RexLang* ll,CStr name,CStr type,int sizeonstack,int stack,char destroy){
    Scope_BuildInitVariable(&ll->ev.sc,name,type,(RexLangVariable[]){ RexLangVariable_New(stack,sizeonstack,destroy,ll) });
}
void RexLang_Variable_BuildX(RexLang* ll,CStr name,CStr type,int sizeonstack,char destroy){
    if(ll->ev.sc.range>0){
        ll->stack += RexLang_Size(ll,type);
    }
    Scope_BuildInitVariable(&ll->ev.sc,name,type,(RexLangVariable[]){ RexLangVariable_New(ll->stack,sizeonstack,destroy,ll) });
    if(ll->ev.sc.range>0){
        ll->stack += sizeonstack;
    }
}
void RexLang_Variable_Build(RexLang* ll,CStr name,CStr type){
    RexLang_Variable_BuildX(ll,name,type,0,1);
}
void RexLang_Variable_BuildRange(RexLang* ll,CStr name,CStr type,Range r,int sizeonstack,char destroy){
    if(ll->ev.sc.range>0){
        ll->stack += RexLang_Size(ll,type);
    }
    Scope_BuildInitVariableRange(&ll->ev.sc,name,type,r,(RexLangVariable[]){ RexLangVariable_New(ll->stack,sizeonstack,destroy,ll) });
}

void RexLang_Variable_Build_Decl(RexLang* ll,CStr name,CStr type){
    RexLang_Variable_Build(ll,name,type);
    
    if(ll->ev.sc.range==0){
        CStr g_value = RexLang_BuildGlobal(ll,name,RexLang_Size(ll,type));
        Variable* v = Scope_FindVariable(&ll->ev.sc,name);
        RexLangVariable* sv = (RexLangVariable*)Variable_Data(v);
        sv->global = g_value;
    }else{
        RexLang_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",RexLang_Size(ll,type));
    }
}
void RexLang_Variable_Build_DeclStack(RexLang* ll,CStr name,CStr type,int stacksize){
    RexLang_Variable_BuildX(ll,name,type,stacksize,1);
    
    if(ll->ev.sc.range==0){
        CStr g_value = RexLang_BuildGlobal(ll,name,RexLang_Size(ll,type));
        Variable* v = Scope_FindVariable(&ll->ev.sc,name);
        RexLangVariable* sv = (RexLangVariable*)Variable_Data(v);
        sv->global = g_value;
    }else{
        RexLang_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",RexLang_Size(ll,type));
        RexLang_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",stacksize);
        RexLang_Indentation_Appendf(ll,&ll->text,"mov QWORD[rsp + %d],rsp",stacksize);
    }
}
void RexLang_Variable_Build_Ref(RexLang* ll,CStr name,CStr type){
    RexLang_Variable_BuildX(ll,name,type,0,0);
}
void RexLang_Variable_BuildRange_Decl(RexLang* ll,CStr name,CStr type,Range r){
    RexLang_Variable_BuildRange(ll,name,type,r,0,1);
    
    if(ll->ev.sc.range==0){
        CStr g_value = RexLang_BuildGlobal(ll,name,RexLang_Size(ll,type));
        Variable* v = Scope_FindVariable(&ll->ev.sc,name);
        RexLangVariable* sv = (RexLangVariable*)Variable_Data(v);
        sv->global = g_value;
    }else{
        RexLang_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",RexLang_Size(ll,type));
    }
}
void RexLang_Variable_Build_Ref_Decl(RexLang* ll,CStr name,CStr type){
    RexLang_Variable_Build_Ref(ll,name,type);

    if(ll->ev.sc.range==0){
        CStr g_value = RexLang_BuildGlobal(ll,name,RexLang_Size(ll,type));
        Variable* v = Scope_FindVariable(&ll->ev.sc,name);
        RexLangVariable* sv = (RexLangVariable*)Variable_Data(v);
        sv->global = g_value;
    }else{
        RexLang_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",RexLang_Size(ll,type));
    }
}
void RexLang_Variable_Build_Use(RexLang* ll,CStr name,CStr type,int stack){
    RexLang_Variable_BuildXX(ll,name,type,0,stack,2);
}

void RexLang_Variable_Destroy_Decl(RexLang* ll,CStr name){
    Variable* v = Scope_FindVariable(&ll->ev.sc,name);
    RexLangVariable* sv = (RexLangVariable*)Variable_Data(v);
    
    if(v->range>0){
        int size = RexLang_Size(ll,v->typename);
        if(size>0){
            RexLang_Indentation_Appendf(ll,&ll->text,"add rsp,%d",size);
            ll->stack -= size;
        }
        if(sv->sizeonstack > 0){
            RexLang_Indentation_Appendf(ll,&ll->text,"add rsp,%d",sv->sizeonstack);
            ll->stack -= sv->sizeonstack;
        }
    }
}
void RexLang_Variable_Destroy_Only(RexLang* ll,CStr name){
    Variable* v = Scope_FindVariable(&ll->ev.sc,name);
    RexLangVariable* sv = (RexLangVariable*)Variable_Data(v);
    
    if(v->range>0){
        int size = RexLang_Size(ll,v->typename);
        if(size>0){
            RexLang_Indentation_Appendf(ll,&ll->text,"add rsp,%d",size);
        }
        if(sv->sizeonstack > 0){
            RexLang_Indentation_Appendf(ll,&ll->text,"add rsp,%d",sv->sizeonstack);
        }
    }
}
void RexLang_Variable_Destroy_Ref_Decl(RexLang* ll,CStr name){
    Variable* v = Scope_FindVariable(&ll->ev.sc,name);
    RexLangVariable* sv = (RexLangVariable*)Variable_Data(v);
    
    if(v->range>0){
        int size = RexLang_Size(ll,v->typename);
        if(size>0){
            ll->stack -= size;
        }
        if(sv->sizeonstack > 0){
            ll->stack -= sv->sizeonstack;
        }
    }
}
void RexLang_Variable_Destroy_Use(RexLang* ll,CStr name){
    //Variable* v = Scope_FindVariable(&ll->ev.sc,name);
}

CStr RexLang_VariableType(RexLang* ll,Token* a){
    if(a->tt==TOKEN_STRING){
        Variable* v_a = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v_a) return RexLang_TypeOfDref(ll,v_a->typename);
    }
    if(a->tt==TOKEN_REXLANG_BOOLEAN)    return CStr_Cpy(BOOL_TYPE);
    if(a->tt==TOKEN_NUMBER)              return CStr_Cpy(I64_TYPE);
    if(a->tt==TOKEN_CONSTSTRING_DOUBLE)  return CStr_Cpy(STR_TYPE);
    if(a->tt==TOKEN_CONSTSTRING_SINGLE)  return CStr_Cpy(CHAR_TYPE);
    if(a->tt==TOKEN_FLOAT)               return CStr_Cpy(F64_TYPE);
    if(a->tt==TOKEN_FUNCTIONPOINTER)     return CStr_Cpy(NULL_TYPE);
    if(a->tt==TOKEN_REXLANG_NULL)       return CStr_Cpy(NULL_TYPE);
    return NULL;
}
CStr RexLang_VariablesType(RexLang* ll,Token* a,Token* b){
    CStr ret = RexLang_VariableType(ll,a);
    if(ret) return ret;
    return RexLang_VariableType(ll,b);
}

int RexLang_TypeRealSize(RexLang* ll,Token* a){
    CStr type = RexLang_VariableType(ll,a);
    if(!type) return 0;

    if(RexLang_DrefType(ll,type)){
        CStr ntype = RexLang_TypeOfDref(ll,type);
        CStr_Set(&type,ntype);
        CStr_Free(&ntype);
    }

    int size = RexLang_Size(ll,type);
    CStr_Free(&type);
    return size;
}
char** RexLang_SelectRT(RexLang* ll,int size){
    if(size==1) return RexLang_RT_8;
    if(size==2) return RexLang_RT_16;
    if(size==4) return RexLang_RT_32;
    if(size==8) return RexLang_RT_64;
    return NULL;
}
CStr RexLang_FunctionName(RexLang* ll,CStr name){
    String builder = String_Format("f.%s",name);
    String_Replace(&builder,"::",".");
    
    CStr functionname = String_CStr(&builder);
    String_Free(&builder);
    return functionname;
}
CStr RexLang_SpaceName(RexLang* ll,CStr name){
    String builder = String_New();

    for(int i = 0;i<ll->ev.cs.size;i++){
        CallPosition* cp = (CallPosition*)Vector_Get(&ll->ev.cs,i);
        if(cp->type==TOKEN_REXLANG_NAMESPACE){
            String_Append(&builder,cp->fname);
            String_Append(&builder,"::");
        }
    }
    String_Append(&builder,name);

    CStr cstr = String_CStr(&builder);
    String_Free(&builder);
    return cstr;
}
CStr RexLang_FuncSpaceName(RexLang* ll,CStr name){
    String builder = String_New();

    for(int i = 0;i<ll->ev.cs.size;i++){
        CallPosition* cp = (CallPosition*)Vector_Get(&ll->ev.cs,i);
        if(cp->type==TOKEN_REXLANG_IMPL){
            String_Append(&builder,cp->fname);
            String_Append(&builder,"::");
            break;
        }
    }

    if(builder.size == 0){
        for(int i = 0;i<ll->ev.cs.size;i++){
            CallPosition* cp = (CallPosition*)Vector_Get(&ll->ev.cs,i);
            if(cp->type==TOKEN_REXLANG_NAMESPACE){
                String_Append(&builder,cp->fname);
                String_Append(&builder,"::");
            }
        }
    }

    String_Append(&builder,name);

    CStr cstr = String_CStr(&builder);
    String_Free(&builder);
    return cstr;
}
Function* RexLang_FunctionIn(RexLang* ll) {
    for(int i = ll->ev.cs.size - 1;i>=0;i--){
        CallPosition* cp = Vector_Get(&ll->ev.cs,i);
        
        if(cp && cp->type==TOKEN_FUNCTIONDECL){
            TT_Iter it = FunctionMap_Find(&ll->ev.fs,cp->fname);
            Function* f = (Function*)Vector_Get(&ll->ev.fs,it);
            return f;
        }
    }
    return NULL;
}

void RexLang_LogicAddPath(RexLang* ll){
    if(ll->ev.sc.range>=ll->logicpath.size){
        while(ll->ev.sc.range>=ll->logicpath.size) Vector_Push(&ll->logicpath,(LogicBlock[]){ 0,0 });
    }else{
        LogicBlock* r = (LogicBlock*)Vector_Get(&ll->logicpath,ll->ev.sc.range);
        r->count++;
    }
}
void RexLang_LogicAddExtend(RexLang* ll){
    if(ll->ev.sc.range>=ll->logicpath.size){
        while(ll->ev.sc.range>=ll->logicpath.size) Vector_Push(&ll->logicpath,(LogicBlock[]){ 0,0 });
    }else{
        LogicBlock* r = (LogicBlock*)Vector_Get(&ll->logicpath,ll->ev.sc.range);
        r->extend++;
    }
}
int RexLang_GetLogicPath(RexLang* ll,Range r){
    if(r<0){
        return -1;
    }
    if(r>=ll->logicpath.size){
        while(ll->ev.sc.range>=ll->logicpath.size) Vector_Push(&ll->logicpath,(LogicBlock[]){ 0,0 });
        return 0;
    }
    return ((LogicBlock*)Vector_Get(&ll->logicpath,r))->count;
}
int RexLang_GetLogicExtend(RexLang* ll,Range r){
    if(r<0){
        return -1;
    }
    if(r>=ll->logicpath.size){
        while(ll->ev.sc.range>=ll->logicpath.size) Vector_Push(&ll->logicpath,(LogicBlock[]){ 0,0 });
        return 0;
    }
    return ((LogicBlock*)Vector_Get(&ll->logicpath,r))->extend;
}
CStr RexLang_Logic(RexLang* ll,char se,CStr type,Range r){
    int lp = RexLang_GetLogicPath(ll,ll->ev.sc.range);
    if(CStr_Cmp(type,"ELIF")){
        int ex = RexLang_GetLogicExtend(ll,ll->ev.sc.range);
        return CStr_Format("l.%d_%s%d_%d_%d",se,type,r,lp,ex);
    }
    return CStr_Format("l.%d_%s%d_%d",se,type,r,lp);
}
void RexLang_LogicCorrection(RexLang* ll,TokenMap* tm){
    Token* t = (Token*)Vector_Get(tm,0);
    
    if(ll->logic==TOKEN_REXLANG_IF){
        int lp = RexLang_GetLogicPath(ll,ll->ev.sc.range);
        if(t->tt==TOKEN_REXLANG_ELIF || t->tt==TOKEN_REXLANG_ELSE){
            CStr log_label = RexLang_Logic(ll,1,RexLang_LOG,ll->ev.sc.range);
            RexLang_Indentation_Appendf(ll,&ll->text,"jmp %s",log_label);
            CStr_Free(&log_label);
        }else{
            CStr log_label = RexLang_Logic(ll,1,RexLang_LOG,ll->ev.sc.range);
            RexLang_Indentation_Appendf(ll,&ll->text,"%s:",log_label);
            CStr_Free(&log_label);
        }

        CStr if_label = RexLang_Logic(ll,1,RexLang_IF,ll->ev.sc.range);
        RexLang_Indentation_Appendf(ll,&ll->text,"%s:",if_label);
        CStr_Free(&if_label);
    }else if(ll->logic==TOKEN_REXLANG_ELIF){
        int lp = RexLang_GetLogicPath(ll,ll->ev.sc.range);
        if(t->tt==TOKEN_REXLANG_ELIF || t->tt==TOKEN_REXLANG_ELSE){
            CStr log_label = RexLang_Logic(ll,1,RexLang_LOG,ll->ev.sc.range);
            RexLang_Indentation_Appendf(ll,&ll->text,"jmp %s",log_label);
            CStr_Free(&log_label);
        }else{
            CStr log_label = RexLang_Logic(ll,1,RexLang_LOG,ll->ev.sc.range);
            RexLang_Indentation_Appendf(ll,&ll->text,"%s:",log_label);
            CStr_Free(&log_label);
        }

        CStr if_label = RexLang_Logic(ll,1,RexLang_ELIF,ll->ev.sc.range);
        RexLang_Indentation_Appendf(ll,&ll->text,"%s:",if_label);
        CStr_Free(&if_label);
    }else if(ll->logic==TOKEN_REXLANG_ELSE){
        int lp = RexLang_GetLogicPath(ll,ll->ev.sc.range);
        CStr log_label = RexLang_Logic(ll,1,RexLang_LOG,ll->ev.sc.range);
        RexLang_Indentation_Appendf(ll,&ll->text,"%s:",log_label);
        CStr_Free(&log_label);
    }else if(ll->logic==TOKEN_REXLANG_WHILE){
        int lp = RexLang_GetLogicPath(ll,ll->ev.sc.range);
        CStr start_label = RexLang_Logic(ll,0,RexLang_WHILE,ll->ev.sc.range);
        CStr end_label = RexLang_Logic(ll,1,RexLang_WHILE,ll->ev.sc.range);
        RexLang_Indentation_Appendf(ll,&ll->text,"jmp %s",start_label);
        RexLang_Indentation_Appendf(ll,&ll->text,"%s:",end_label);
        CStr_Free(&end_label);
        CStr_Free(&start_label);
    }else if(ll->logic==TOKEN_REXLANG_FOR){
        int lp = RexLang_GetLogicPath(ll,ll->ev.sc.range);
        CStr start_label = RexLang_Logic(ll,0,RexLang_FOR,ll->ev.sc.range);
        CStr end_label = RexLang_Logic(ll,1,RexLang_FOR,ll->ev.sc.range);
        RexLang_Indentation_Appendf(ll,&ll->text,"jmp %s",start_label);
        RexLang_Indentation_Appendf(ll,&ll->text,"%s:",end_label);
        CStr_Free(&end_label);
        CStr_Free(&start_label);
        Scope_Pop(&ll->ev.sc);// first possible decl in for ... , 
    }
    ll->logic = TOKEN_NONE;
}
CStr RexLang_LogicCmp(RexLang* ll,CStr type){
    return CStr_Format("lcmp.%d_%s",ll->lcmps++,type);
}

Boolean RexLang_TypeInt(CStr typename){
    return
    CStr_Cmp(typename,I8_TYPE) || CStr_Cmp(typename,I16_TYPE) ||
    CStr_Cmp(typename,I32_TYPE) || CStr_Cmp(typename,I64_TYPE) ||
    CStr_Cmp(typename,U8_TYPE) || CStr_Cmp(typename,U16_TYPE) ||
    CStr_Cmp(typename,U32_TYPE) || CStr_Cmp(typename,U64_TYPE);
}
Boolean RexLang_TypeFloat(CStr typename){
    return CStr_Cmp(typename,F32_TYPE) || CStr_Cmp(typename,F64_TYPE);
}

Number RexLang_Function_Add(Number a,Number b){
    return a + b;
}
Number RexLang_Function_Sub(Number a,Number b){
    return a - b;
}
Number RexLang_Function_Mul(Number a,Number b){
    return a * b;
}
Number RexLang_Function_Div(Number a,Number b){
    return a / b;
}
Number RexLang_Function_Mod(Number a,Number b){
    return a % b;
}
Number RexLang_Function_And(Number a,Number b){
    return a & b;
}
Number RexLang_Function_Or(Number a,Number b){
    return a | b;
}
Number RexLang_Function_Xor(Number a,Number b){
    return a ^ b;
}
Number RexLang_Function_Not(Number a){
    return ~a;
}
Number RexLang_Function_Neg(Number a){
    return -a;
}
Number RexLang_Function_Shl(Number a,Number b){
    return a << b;
}
Number RexLang_Function_Shr(Number a,Number b){
    return a >> b;
}
Boolean RexLang_Function_Equ(Number a,Number b){
    return a == b;
}
Boolean RexLang_Function_Neq(Number a,Number b){
    return a != b;
}
Boolean RexLang_Function_Les(Number a,Number b){
    return a < b;
}
Boolean RexLang_Function_Grt(Number a,Number b){
    return a > b;
}
Boolean RexLang_Function_Leq(Number a,Number b){
    return a <= b;
}
Boolean RexLang_Function_Grq(Number a,Number b){
    return a >= b;
}
Boolean RexLang_Function_Lnd(Boolean a,Boolean b){
    return a && b;
}
Boolean RexLang_Function_Lor(Boolean a,Boolean b){
    return a || b;
}
Boolean RexLang_Function_Lot(Boolean a){
    return !a;
}

void RexLang_Destroyer(Variable* v){
    //printf("[RexLang]: Destroyer: %s -> ",v->typename);
    RexLangVariable* v_sv = (RexLangVariable*)Variable_Data(v);
    
    if(v_sv->destroy==0){
        RexLang_Variable_Destroy_Ref_Decl(v_sv->parent,v->name);
        //printf("Ref -> %s -> %d\n",v->name,((RexLang*)v_sv->parent)->stack);
    }else if(v_sv->destroy==1){
        RexLang_Variable_Destroy_Decl(v_sv->parent,v->name);
        //printf("Real -> %s -> %d\n",v->name,((RexLang*)v_sv->parent)->stack);
    }else if(v_sv->destroy==2){
        RexLang_Variable_Destroy_Use(v_sv->parent,v->name);
        //printf("Use -> %s -> %d -> %d\n",v->name,v_sv->stack,((RexLang*)v_sv->parent)->stack);
    }else if(v_sv->destroy==3){
        RexLang_Variable_Destroy_Only(v_sv->parent,v->name);
        //printf("Only -> %s -> %d -> %d\n",v->name,v_sv->stack,((RexLang*)v_sv->parent)->stack);
    }

    if(v_sv->destroy != 3)
        RexLangVariable_Free(v_sv);
}
void RexLang_Cpyer(Variable* src,Variable* dst){
    //printf("[RexLang]: Cpyer!\n");
    //RexLangVariable* src_str = (RexLangVariable*)Variable_Data(src);
    //RexLangVariable* dst_str = (RexLangVariable*)Variable_Data(dst);
    //*dst_str = VariableMap_Cpy(src_str);
}
Token RexLang_Init(RexLang* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("[RexLang]: INIT: %s\n",a->str);
    
    Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
    if(v){
        RexLangVariable* sv = (RexLangVariable*)Variable_Data(v);
        int size = RexLang_Size(ll,v->typename);
        
        if(v->range>0){
            if(size>0){
                ll->stack += size;
                RexLang_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",size);
            }
            *sv = RexLangVariable_New(ll->stack,0,1,ll);
        }else{
            *sv = RexLangVariable_New(ll->stack,0,1,ll);
            CStr value = RexLang_BuildGlobal(ll,v->name,size);
            sv->global = value;
        }
    }
    return Token_Cpy(a);
}

void RexLang_FromTo_DestroyOnly(RexLang* ll,Range start,Range end) {
    for(int i = ll->ev.sc.vars.size-1;i>=0;i--){
        Variable* v = (Variable*)PVector_Get(&ll->ev.sc.vars,i);
        
        if(v->range > start && v->range<=end){
            RexLangVariable* v_sv = (RexLangVariable*)Variable_Data(v);
            char destroy = v_sv->destroy;
            v_sv->destroy = 3;
            Variable_Destroyer(v);
            v_sv->destroy = destroy;
        }
    }
}

Double RexLang_GetFloat(RexLang* ll,CStr fstr){
    return Double_Parse(fstr,0);
}
CStr RexLang_GetFloatStr(RexLang* ll,CStr fstr){
    Double d = RexLang_GetFloat(ll,fstr);
    CStr cstr = Number_Get(*((Number*)&d));
    return cstr;
}
Token RexLang_GetFloatToken(RexLang* ll,CStr fstr){
    return Token_Move(TOKEN_FLOAT,RexLang_GetFloatStr(ll,fstr));
}

CStr RexLang_FInstOf(RexLang* ll,Token* a,CStr inst){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            if(RexLang_TypeFloat(v->typename)){
                return CStr_Format("f%s",inst);
            }else if(RexLang_TypeInt(v->typename)){
                return CStr_Format("fi%s",inst);
            }
        }
    }else if(a->tt==TOKEN_FLOAT){
        return CStr_Format("f%s",inst);
    }else if(a->tt==TOKEN_NUMBER){
        return CStr_Format("fi%s",inst);
    }
    return NULL;
}

Boolean RexLang_Extract(RexLang* ll,Token* a,Number* n){
    if(a->tt==TOKEN_STRING){
        return False;
    }else if(a->tt==TOKEN_NUMBER){
        *n = Number_Parse(a->str);
        return True;
    }else{
        Compiler_ErrorHandler(&ll->ev,"Number -> 1. Arg: %s is not a int type!",a->str);
        return False;
    }
}
Boolean RexLang_ExtractBool(RexLang* ll,Token* a,Boolean* b){
    if(a->tt==TOKEN_STRING){
        return False;
    }else if(a->tt==TOKEN_REXLANG_BOOLEAN){
        *b = Boolean_Parse(a->str);
        return True;
    }else{
        Compiler_ErrorHandler(&ll->ev,"1. Arg: %s is not a bool type!",a->str);
        return False;
    }
}

Boolean RexLang_ErrorsInArg(RexLang* ll,Token* a){
    if(a->tt==TOKEN_NONE){
        Compiler_ErrorHandler(&ll->ev,"Errors -> Tokentype of %s is NONE!",a->str);
        return 1;
    }
    if(a->tt==TOKEN_CONSTSTRING_DOUBLE){
        if(a->str == NULL){
            Compiler_ErrorHandler(&ll->ev,"Errors -> const str %s is null!",a->str);
            return 1; 
        }
    }
    if(a->tt==TOKEN_CONSTSTRING_SINGLE){
        if(a->str == NULL){
            Compiler_ErrorHandler(&ll->ev,"Errors -> const char %s is null!",a->str);
            return 1;
        }
    }
    if(a->tt==TOKEN_NUMBER){
        if(Number_Parse(a->str) == NUMBER_PARSE_ERROR){
            Compiler_ErrorHandler(&ll->ev,"Errors -> number %s is invalid!",a->str);
            return 1;
        }
    }
    if(a->tt==TOKEN_FLOAT){
        if(Double_Parse(a->str,1) == DOUBLE_PARSE_ERROR){
            Compiler_ErrorHandler(&ll->ev,"Errors -> float %s is invalid!",a->str);
            return 1;
        }
    }
    if(a->tt==TOKEN_REXLANG_BOOLEAN){
        if(Boolean_Parse(a->str) == BOOL_PARSE_ERROR){
            Compiler_ErrorHandler(&ll->ev,"Errors -> bool %s is invalid!",a->str);
            return 1;
        }
    }
    if(a->tt==TOKEN_STRING){
        if(Scope_FindVariable(&ll->ev.sc,a->str) == NULL){
            Compiler_ErrorHandler(&ll->ev,"Errors -> variable %s doesn't exist!",a->str);
            return 1;
        }
    }
    if(a->tt==TOKEN_FUNCTIONPOINTER){
        if(Scope_FindVariable(&ll->ev.sc,a->str) == NULL){
            Compiler_ErrorHandler(&ll->ev,"Errors -> function %s is null!",a->str);
            return 1;
        }
    }

    const int size = RexLang_TypeRealSize(ll,a);
    if(size <= 0){
        Compiler_ErrorHandler(&ll->ev,"Errors -> size of %s is %d!",a->str,size);
        return 1;
    }

    return 0;
}

void RexLang_DrefIntoReg(RexLang* ll,Token* a,CStr reg){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            if(RexLang_DrefType(ll,v->typename)){
                CStr location = RexLang_Location(ll,a->str);
                RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s",reg,location);
                CStr typename = CStr_Cpy(v->typename);
                int drefs = RexLang_DrefTypeCount(ll,typename);
                for(int i = 0;i<drefs-1;i++){
                    CStr typeselector = NULL;
                    if(typename[CStr_Size(typename) - 1]=='&')  typeselector = CStr_Cpy(RexLang_DREF_64);
                    else                                        typeselector = RexLang_TypeSelector_T(ll,typename);

                    RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s[%s]",reg,typeselector,reg);
                    CStr_Free(&typeselector);

                    CStr newtypename = CStr_PopOff(typename);
                    CStr_Set(&typename,newtypename);
                    CStr_Free(&newtypename);
                }
                CStr_Free(&location);
            }else{
                Compiler_ErrorHandler(&ll->ev,"IntoReg -> Error: %s is not a var!",a->str);
            }
        }else{
            Compiler_ErrorHandler(&ll->ev,"DrefIntoReg -> Error: %s is not a dref var!",a->str);
        }
    }
}
void RexLang_IntoReg(RexLang* ll,Token* a,CStr reg){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = RexLang_Location(ll,a->str);
        
            if(RexLang_DrefType(ll,v->typename)){
                RexLang_DrefIntoReg(ll,a,RexLang_REG_10_64);
                
                CStr typename = RexLang_TypeOfDref(ll,v->typename);
                CStr typeselector = RexLang_TypeSelector_T(ll,typename);
                RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s[%s]",reg,typeselector,RexLang_REG_10_64);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s",reg,location);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"IntoReg -> Error: %s is not a var!",a->str);
        }
    }else if(a->tt==TOKEN_FLOAT){
        CStr fstr = RexLang_GetFloatStr(ll,a->str);
        RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s",reg,fstr);
        CStr_Free(&fstr);
    }else if(a->tt==TOKEN_REXLANG_BOOLEAN){
        Boolean b = Boolean_Parse(a->str);
        RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%d",reg,b);
    }else if(a->tt==TOKEN_REXLANG_NULL){
        RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,0",reg);
    }else if(a->tt==TOKEN_CONSTSTRING_SINGLE){
        Number val = 0;
        const int size = CStr_Size(a->str);
        for(int i = 0;i<size;i++){
            val += a->str[i] << (i * 8);
        }
        RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%d",reg,val);
    }else{
        RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s",reg,a->str);
    }
}
void RexLang_IntoSet(RexLang* ll,Token* a,CStr reg){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = RexLang_Location(ll,a->str);
        
            if(RexLang_DrefType(ll,v->typename)){
                RexLang_DrefIntoReg(ll,a,RexLang_REG_10_64);
                
                CStr typename = RexLang_TypeOfDref(ll,v->typename);
                CStr typeselector = RexLang_TypeSelector_T(ll,typename);
                RexLang_Indentation_Appendf(ll,&ll->text,"mov %s[%s],%s",typeselector,RexLang_REG_10_64,reg);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s",location,reg);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"IntoSet -> Error: %s is not a var!",a->str);
        }
    }else{
        Compiler_ErrorHandler(&ll->ev,"IntoSet -> Error: %s is not a var but assigned!",a->str);
    }
}
void RexLang_AtReg(RexLang* ll,Token* a,CStr reg,CStr inst){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = RexLang_Location(ll,a->str);
            
            if(RexLang_DrefType(ll,v->typename)){
                RexLang_DrefIntoReg(ll,a,RexLang_REG_10_64);
                
                CStr typename = RexLang_TypeOfDref(ll,v->typename);
                CStr typeselector = RexLang_TypeSelector_T(ll,typename);
                RexLang_Indentation_Appendf(ll,&ll->text,"%s %s,%s[%s]",inst,reg,typeselector,RexLang_REG_10_64);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                RexLang_Indentation_Appendf(ll,&ll->text,"%s %s,%s",inst,reg,location);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"AtReg -> Error: %s is not a var!",a->str);
        }
    }else if(a->tt==TOKEN_FLOAT){
        CStr fstr = RexLang_GetFloatStr(ll,a->str);
        RexLang_Indentation_Appendf(ll,&ll->text,"%s %s,%s",inst,reg,fstr);
        CStr_Free(&fstr);
    }else if(a->tt==TOKEN_REXLANG_BOOLEAN){
        Boolean b = Boolean_Parse(a->str);
        RexLang_Indentation_Appendf(ll,&ll->text,"%s %s,%d",inst,reg,b);
    }else if(a->tt==TOKEN_REXLANG_NULL){
        RexLang_Indentation_Appendf(ll,&ll->text,"%s %s,0",inst,reg);
    }else if(a->tt==TOKEN_CONSTSTRING_SINGLE){
        Number val = 0;
        const int size = CStr_Size(a->str);
        for(int i = 0;i<size;i++){
            val += a->str[i] << (i * 8);
        }
        RexLang_Indentation_Appendf(ll,&ll->text,"%s %s,%d",inst,reg,val);
    }else{
        RexLang_Indentation_Appendf(ll,&ll->text,"%s %s,%s",inst,reg,a->str);
    }
}
void RexLang_AtRegSingle(RexLang* ll,Token* a,CStr inst){//Always A ex: mul,div etc
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = RexLang_Location(ll,a->str);
            
            if(RexLang_DrefType(ll,v->typename)){
                RexLang_DrefIntoReg(ll,a,RexLang_REG_10_64);
                
                CStr typename = RexLang_TypeOfDref(ll,v->typename);
                CStr typeselector = RexLang_TypeSelector_T(ll,typename);
                RexLang_Indentation_Appendf(ll,&ll->text,"%s %s[%s]",inst,typeselector,RexLang_REG_10_64);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                RexLang_Indentation_Appendf(ll,&ll->text,"%s %s",inst,location);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"AtRegSingle -> Error: %s is not a var!",a->str);
        }
    }else if(a->tt==TOKEN_FLOAT){
        CStr fstr = RexLang_GetFloatStr(ll,a->str);
        RexLang_Indentation_Appendf(ll,&ll->text,"%s %s",inst,fstr);
        CStr_Free(&fstr);
    }else if(a->tt==TOKEN_REXLANG_BOOLEAN){
        Boolean b = Boolean_Parse(a->str);
        RexLang_Indentation_Appendf(ll,&ll->text,"%s %d",inst,b);
    }else if(a->tt==TOKEN_REXLANG_NULL){
        RexLang_Indentation_Appendf(ll,&ll->text,"%s 0",inst);
    }else if(a->tt==TOKEN_CONSTSTRING_SINGLE){
        Number val = 0;
        const int size = CStr_Size(a->str);
        for(int i = 0;i<size;i++){
            val += a->str[i] << (i * 8);
        }
        RexLang_Indentation_Appendf(ll,&ll->text,"mov %d",val);
    }else{
        RexLang_Indentation_Appendf(ll,&ll->text,"%s %s",inst,a->str);
    }
}
void RexLang_AtSingle(RexLang* ll,CStr reg,CStr inst){//Always A ex: mul,div etc
    RexLang_Indentation_Appendf(ll,&ll->text,"%s %s",inst,reg);
}
void RexLang_AtSet(RexLang* ll,Token* a,CStr reg,CStr inst){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = RexLang_Location(ll,a->str);
            
            if(RexLang_DrefType(ll,v->typename)){
                RexLang_DrefIntoReg(ll,a,RexLang_REG_10_64);
                
                CStr typename = RexLang_TypeOfDref(ll,v->typename);
                CStr typeselector = RexLang_TypeSelector_T(ll,typename);
                RexLang_Indentation_Appendf(ll,&ll->text,"%s %s[%s],%s",inst,typeselector,RexLang_REG_10_64,reg);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                RexLang_Indentation_Appendf(ll,&ll->text,"%s %s,%s",inst,location,reg);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"AtSet -> Error: %s is not a var!",a->str);
        }
    }else{
        Compiler_ErrorHandler(&ll->ev,"Set -> Error: %s is not a var but assigned!",a->str);
    }
}
void RexLang_CmpAtReg(RexLang* ll,Token* a,CStr reg){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        CStr location = RexLang_Location(ll,a->str);
        
        if(RexLang_DrefType(ll,v->typename)){
            RexLang_DrefIntoReg(ll,a,RexLang_REG_10_64);
                
            CStr typename = RexLang_TypeOfDref(ll,v->typename);
            CStr typeselector = RexLang_TypeSelector_T(ll,typename);
            RexLang_Indentation_Appendf(ll,&ll->text,"cmp %s,%s[%s]",reg,typeselector,RexLang_REG_10_64);
            CStr_Free(&typename);
            CStr_Free(&typeselector);
        }else{
            RexLang_Indentation_Appendf(ll,&ll->text,"cmp %s,%s",reg,location);
        }
        CStr_Free(&location);
    }else if(a->tt==TOKEN_FLOAT){
        CStr fstr = RexLang_GetFloatStr(ll,a->str);
        RexLang_Indentation_Appendf(ll,&ll->text,"cmp %s,%s",reg,fstr);
        CStr_Free(&fstr);
    }else if(a->tt==TOKEN_REXLANG_BOOLEAN){
        Boolean b = Boolean_Parse(a->str);
        RexLang_Indentation_Appendf(ll,&ll->text,"cmp %s,%d",reg,b);
    }else if(a->tt==TOKEN_REXLANG_NULL){
        RexLang_Indentation_Appendf(ll,&ll->text,"cmp %s,0",reg);
    }else if(a->tt==TOKEN_CONSTSTRING_SINGLE){
        Number val = 0;
        const int size = CStr_Size(a->str);
        for(int i = 0;i<size;i++){
            val += a->str[i] << (i * 8);
        }
        RexLang_Indentation_Appendf(ll,&ll->text,"cmp %s,%d",reg,val);
    }else{
        RexLang_Indentation_Appendf(ll,&ll->text,"cmp %s,%s",reg,a->str);
    }
}
void RexLang_CmpAtSet(RexLang* ll,Token* a,CStr inst){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        CStr location = RexLang_Location(ll,a->str);
        
        if(RexLang_DrefType(ll,v->typename)){
            RexLang_DrefIntoReg(ll,a,RexLang_REG_10_64);
                
            CStr typename = RexLang_TypeOfDref(ll,v->typename);
            CStr typeselector = RexLang_TypeSelector_T(ll,typename);
            RexLang_Indentation_Appendf(ll,&ll->text,"%s %s[%s]",inst,typeselector,RexLang_REG_10_64);
            CStr_Free(&typename);
            CStr_Free(&typeselector);
        }else{
            RexLang_Indentation_Appendf(ll,&ll->text,"%s %s",inst,location);
        }
        CStr_Free(&location);
    }else{
        Compiler_ErrorHandler(&ll->ev,"Set -> Error: %s is not a var but assigned!",a->str);
    }
}

Token RexLang_ExecuteAss(RexLang* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(RexLang_ErrorsInArg(ll,a)) return Token_Null();
    if(RexLang_ErrorsInArg(ll,b)) return Token_Null();

    if(b->tt==TOKEN_NUMBER){
        RexLang_AtSet(ll,a,b->str,instname);
        return Token_Cpy(a);
    }else if(b->tt==TOKEN_FLOAT){
        CStr fstr = RexLang_GetFloatStr(ll,b->str);
        RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,%s",RexLang_REG_A_64,fstr);
        RexLang_AtSet(ll,a,RexLang_REG_A_64,instname);
        CStr_Free(&fstr);
        return Token_Cpy(a);
    }else if(b->tt==TOKEN_REXLANG_NULL){
        RexLang_AtSet(ll,a,"0",instname);
        return Token_Cpy(a);
    }else{
        int realsize_a = RexLang_TypeRealSize(ll,a);
        int realsize_b = RexLang_TypeRealSize(ll,b);
        
        if(realsize_b>realsize_a)
            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,0",RexLang_REG_A_64);

        RexLang_IntoReg(ll,b,RexLang_SelectRT(ll,realsize_b)[RexLang_REG_A]);
        RexLang_AtSet(ll,a,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A],instname);
        return Token_Cpy(a);
    }
}
Token RexLang_ExecuteAssA(RexLang* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(RexLang_ErrorsInArg(ll,a)) return Token_Null();
    if(RexLang_ErrorsInArg(ll,b)) return Token_Null();

    int realsize_a = RexLang_TypeRealSize(ll,a);
    int realsize_b = RexLang_TypeRealSize(ll,b);
        
    if(realsize_b>realsize_a)
        RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,0",RexLang_REG_A_64);

    RexLang_IntoReg(ll,a,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
    RexLang_IntoReg(ll,b,RexLang_SelectRT(ll,realsize_b)[RexLang_REG_B]);
    RexLang_AtSingle(ll,RexLang_SelectRT(ll,realsize_b)[RexLang_REG_B],instname);
    RexLang_IntoSet(ll,a,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
    return Token_Cpy(a);
}
Token RexLang_ExecuteAssSingle(RexLang* ll,Token* a,Token* op,CStr instname,CStr instnameupper){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(RexLang_ErrorsInArg(ll,a)) return Token_Null();

    RexLang_AtRegSingle(ll,a,instname);
    return Token_Cpy(a);
}

Token RexLang_Execute(RexLang* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper,Number (*inst)(Number,Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(RexLang_ErrorsInArg(ll,a)) return Token_Null();
    if(RexLang_ErrorsInArg(ll,b)) return Token_Null();

    if(a->tt==TOKEN_NUMBER && b->tt==TOKEN_NUMBER){
        char* resstr = Number_Get(inst(Number_Parse(a->str),Number_Parse(b->str)));
        return Token_Move(TOKEN_NUMBER,resstr);
    }else{
        CStr typename_a = RexLang_VariableType(ll,a);

        int realsize_a = RexLang_TypeRealSize(ll,a);
        int realsize_b = RexLang_TypeRealSize(ll,b);
        
        CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
        RexLang_Variable_Build_Decl(ll,stack_name,typename_a);
        CStr_Free(&typename_a);
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
        if(realsize_b>realsize_a)
            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,0",RexLang_REG_A_64);

        RexLang_IntoReg(ll,a,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
        RexLang_AtReg(ll,b,RexLang_SelectRT(ll,realsize_b)[RexLang_REG_A],instname);
        RexLang_IntoSet(ll,&stack_t,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
        return stack_t;
    }
}
Token RexLang_ExecuteA(RexLang* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper,Number (*inst)(Number,Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(RexLang_ErrorsInArg(ll,a)) return Token_Null();
    if(RexLang_ErrorsInArg(ll,b)) return Token_Null();

    if(a->tt==TOKEN_NUMBER && b->tt==TOKEN_NUMBER){
        char* resstr = Number_Get(inst(Number_Parse(a->str),Number_Parse(b->str)));
        return Token_Move(TOKEN_NUMBER,resstr);
    }else{
        if(b->tt==TOKEN_NUMBER){
            CStr typename_a = RexLang_VariableType(ll,a);
            int realsize_a = RexLang_TypeRealSize(ll,a);
        
            CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
            RexLang_Variable_Build_Decl(ll,stack_name,typename_a);
            CStr_Free(&typename_a);
            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
            RexLang_IntoReg(ll,a,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
            RexLang_IntoReg(ll,b,RexLang_REG_B_64);
            RexLang_AtSingle(ll,RexLang_REG_B_64,instname);
            RexLang_IntoSet(ll,&stack_t,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
            return stack_t;
        }else{
            CStr typename_a = RexLang_VariableType(ll,a);
            int realsize_a = RexLang_TypeRealSize(ll,a);
        
            CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
            RexLang_Variable_Build_Decl(ll,stack_name,typename_a);
            CStr_Free(&typename_a);
            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
            RexLang_IntoReg(ll,a,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
            RexLang_AtRegSingle(ll,b,instname);
            RexLang_IntoSet(ll,&stack_t,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
            return stack_t;
        }
    }
}
Token RexLang_ExecuteAR(RexLang* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper,Number (*inst)(Number,Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(RexLang_ErrorsInArg(ll,a)) return Token_Null();
    if(RexLang_ErrorsInArg(ll,b)) return Token_Null();

    if(a->tt==TOKEN_NUMBER && b->tt==TOKEN_NUMBER){
        char* resstr = Number_Get(inst(Number_Parse(a->str),Number_Parse(b->str)));
        return Token_Move(TOKEN_NUMBER,resstr);
    }else{
        if(b->tt==TOKEN_NUMBER){
            CStr typename_a = RexLang_VariableType(ll,a);
            int realsize_a = RexLang_TypeRealSize(ll,a);
        
            CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
            RexLang_Variable_Build_Decl(ll,stack_name,typename_a);
            CStr_Free(&typename_a);
            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
            RexLang_IntoReg(ll,a,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
            RexLang_IntoReg(ll,b,RexLang_REG_B_64);
            RexLang_AtSingle(ll,RexLang_REG_B_64,instname);
            RexLang_IntoSet(ll,&stack_t,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_D]);
            return stack_t;
        }else{
            CStr typename_a = RexLang_VariableType(ll,a);
            int realsize_a = RexLang_TypeRealSize(ll,a);
        
            CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
            RexLang_Variable_Build_Decl(ll,stack_name,typename_a);
            CStr_Free(&typename_a);
            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
            RexLang_IntoReg(ll,a,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
            RexLang_AtRegSingle(ll,b,instname);
            RexLang_IntoSet(ll,&stack_t,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_D]);
            return stack_t;
        }
    }
}
Token RexLang_ExecuteAR2(RexLang* ll,Token* a,Token* b,int reg2,int reg2_size,Token* op,CStr instname,CStr instnameupper,Number (*inst)(Number,Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(RexLang_ErrorsInArg(ll,a)) return Token_Null();
    if(RexLang_ErrorsInArg(ll,b)) return Token_Null();

    if(reg2 < 0 || reg2_size < 0) return Token_Null();

    if(a->tt==TOKEN_NUMBER && b->tt==TOKEN_NUMBER){
        char* resstr = Number_Get(inst(Number_Parse(a->str),Number_Parse(b->str)));
        return Token_Move(TOKEN_NUMBER,resstr);
    }else{
        CStr typename_a = RexLang_VariableType(ll,a);
        int realsize_a = RexLang_TypeRealSize(ll,a);
        int realsize_b = RexLang_TypeRealSize(ll,b);
        
        CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
        RexLang_Variable_Build_Decl(ll,stack_name,typename_a);
        CStr_Free(&typename_a);
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
        if(realsize_b>realsize_a)
            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,0",RexLang_REG_A_64);

        RexLang_IntoReg(ll,a,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
        RexLang_IntoReg(ll,b,RexLang_SelectRT(ll,realsize_b)[RexLang_REG_C]);
        RexLang_Indentation_Appendf(ll,&ll->text,"%s %s,%s",instname,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A],RexLang_SelectRT(ll,reg2_size)[RexLang_REG_C]);
        RexLang_IntoSet(ll,&stack_t,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
        return stack_t;
    }
}
Token RexLang_ExecuteSingle(RexLang* ll,Token* a,Token* op,CStr instname,CStr instnameupper,Number (*inst)(Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s%s",instnameupper,op->str,a->str);
    
    if(RexLang_ErrorsInArg(ll,a)) return Token_Null();

    if(a->tt==TOKEN_NUMBER){
        char* resstr = Number_Get(inst(Number_Parse(a->str)));
        return Token_Move(TOKEN_NUMBER,resstr);
    }else{
        CStr typename_a = RexLang_VariableType(ll,a);
        int realsize_a = RexLang_TypeRealSize(ll,a);
        
        CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
        RexLang_Variable_Build_Decl(ll,stack_name,typename_a);
        CStr_Free(&typename_a);
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);

        RexLang_IntoReg(ll,a,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
        RexLang_AtSingle(ll,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A],instname);
        RexLang_IntoSet(ll,&stack_t,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
        
        return stack_t;
    }
}
Token RexLang_ExecuteCmp(RexLang* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper,Boolean (*inst)(Number,Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(RexLang_ErrorsInArg(ll,a)) return Token_Null();
    if(RexLang_ErrorsInArg(ll,b)) return Token_Null();

    if(a->tt==TOKEN_NUMBER && b->tt==TOKEN_NUMBER){
        char* resstr = Boolean_Get(inst(Number_Parse(a->str),Number_Parse(b->str)));
        return Token_Move(TOKEN_REXLANG_BOOLEAN,resstr);
    }else{
        int realsize_a = RexLang_TypeRealSize(ll,a);
        int realsize_b = RexLang_TypeRealSize(ll,b);
        
        CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
        RexLang_Variable_Build_Decl(ll,stack_name,BOOL_TYPE);
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
        if(realsize_b>realsize_a)
            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,0",RexLang_REG_A_64);

        RexLang_IntoReg(ll,a,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
        RexLang_CmpAtReg(ll,b,RexLang_SelectRT(ll,realsize_b)[RexLang_REG_A]);
        RexLang_CmpAtSet(ll,&stack_t,instname);
        return stack_t;
    }
}
Token RexLang_ExecuteJmp(RexLang* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper,Boolean (*inst)(Number,Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(RexLang_ErrorsInArg(ll,a)) return Token_Null();
    if(RexLang_ErrorsInArg(ll,b)) return Token_Null();

    if(a->tt==TOKEN_NUMBER && b->tt==TOKEN_NUMBER){
        char* resstr = Boolean_Get(inst(Number_Parse(a->str),Number_Parse(b->str)));
        return Token_Move(TOKEN_REXLANG_BOOLEAN,resstr);
    }else{
        int realsize_a = RexLang_TypeRealSize(ll,a);
        int realsize_b = RexLang_TypeRealSize(ll,b);
        
        CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
        RexLang_Variable_Build_Decl(ll,stack_name,BOOL_TYPE);
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
        if(realsize_b>realsize_a)
            RexLang_Indentation_Appendf(ll,&ll->text,"mov %s,0",RexLang_REG_A_64);

        RexLang_IntoReg(ll,a,RexLang_SelectRT(ll,realsize_a)[RexLang_REG_A]);
        RexLang_CmpAtReg(ll,b,RexLang_SelectRT(ll,realsize_b)[RexLang_REG_A]);
        
        CStr label_true = RexLang_LogicCmp(ll,RexLang_LOG_TRUE);
        CStr label_end = RexLang_LogicCmp(ll,RexLang_LOG_END);

        RexLang_Indentation_Appendf(ll,&ll->text,"%s %s",instname,label_true);
        RexLang_IntoSet(ll,&stack_t,"0");
        RexLang_Indentation_Appendf(ll,&ll->text,"jmp %s",label_end);
        RexLang_Indentation_Appendf(ll,&ll->text,"%s:",label_true);
        RexLang_IntoSet(ll,&stack_t,"1");
        RexLang_Indentation_Appendf(ll,&ll->text,"%s:",label_end);

        CStr_Free(&label_true);
        CStr_Free(&label_end);
        return stack_t;
    }
}



Token Int_Int_Handler_Cast(RexLang* ll,Token* op,Vector* args,CStr type){
    Token* a = (Token*)Vector_Get(args,0);

    if(a->tt==TOKEN_NUMBER){
        return Token_Cpy(a);
    }else if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        CStr stack_name = RexLang_Variablename_Next(ll,".STACK",6);
        
        RexLang_Variable_Build_Decl(ll,stack_name,type);
        
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        RexLang_ExecuteAss(ll,&stack_t,a,(Token[]){ Token_Move(TOKEN_REXLANG_ASS,NULL) },"mov","ASS");
        return stack_t;
    }else{
        Environment_ErrorHandler(&ll->ev,"Cast(f64 -> i64): Error -> %s is from no possible type!",a->str);
        return Token_Null();
    }
}

#endif //!RexLangASM