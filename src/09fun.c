#include "common.h"

struct sFunNode
{
    int id;
    sNode*% obj;
    string name;
    vector<sNode*%>*% params;
    sNodeBlock? block;
};

sFunNode*% sFunNode*::initialize(sFunNode*% self, string name, sNode*% obj, vector<sNode*%>*% params, sNodeBlock? block)
{
    self.id = gNodeID++;
    self.name = name;
    self.obj = obj;
    self.params = params;
    self.block = block;
    
    return self;
}

unsigned int sFunNode*::id(sFunNode* self)
{
    return self.id;
}

bool sFunNode*::compile(sFunNode* self, sInfo* info)
{
    char* name = self.name;
    vector<sNode*%>* params = self.params;
    sNode* obj = self.obj;
    sNodeBlock&? block = self.block;
    
    if(!obj.compile->(info)) {
        return false;
    }
    
    for(int i=0; i<params.length(); i++) {
        sNode* node = params[i];
        
        if(!node.compile->(info)) {
            return false;
        }
    }
    
    buffer*? codes = null;
    if(block) {
        sInfo info2;
        
        info2.p = info.p;
        info2.command = null;
        info2.codes = new buffer();
        info2.nodes = null;
        info2.head = null;
        info2.op = null;
        info2.stack = new vector<ZVALUE*%>();
        
        info2.loop_head = 0;
        info2.breaks = null;
        info2.result_value = null;
        
        info2.stack_num = 0;
        
        if(!compile_block(block!, &info2)) {
            return false;
        }

        codes = nullable borrow clone info2.codes;
        
    }
    
    info.codes.append_int(OP_FUN);
    info.codes.append_int(params.length());
    
    append_str_to_codes(info, wstring(name));
    
    info.codes.append_int(codes ? codes!.length():0);
    
    if(codes) {
        info.codes.append(codes!.buf, codes!.length());
        
        delete codes!;
    }
    
    info.stack_num -= params.length() + 1;
    info.stack_num++;
    
    return true;
}

bool vm(sInfo* info) version 8
{
    inherit(info);
    
    switch(*info->op) {
        case OP_FUN: {
            info->op++;
            
            int len = *info->op;
            info->op++;
            
            string fun_name = get_str_from_codes(info).to_string();
            
            int codes_len = *info->op;
            info->op++;
            
            buffer*% codes = new buffer();
            if(codes_len > 0) {
                char*% mem = new char[codes_len+1];
                memcpy(mem, (char*)info->op, sizeof(char)*codes_len);
                codes.append(mem, codes_len);
                
                info->op += codes_len / sizeof(int);
            }
            
            ZVALUE*% obj = clone info.stack[-len-1];
            
            list<ZVALUE*%>*% params = new list<ZVALUE*%>();
            
            for(int i=0; i<len; i++) {
                ZVALUE*% value = clone info.stack[-len+i];
                
                params.push_back(value);
            }
            
            for(int i=0; i<len+1; i++) {
                info.stack.delete_back();
            }
            
            ZVALUE*%? result = null;
            
            switch(obj.kind) {
                case kStrValue: {
                    if(fun_name === "chomp") {
                        if(params.length() == 0) {
                            string str = obj.strValue.to_string();
                            if(str[str.length()-1] == '\n') {
                                string str2 = str.substring(0, -2);
                                
                                result = nullable new ZVALUE(kind:kStrValue, str_value:str2.to_wstring());
                            }
                            else {
                                result = nullable new ZVALUE(kind:kStrValue, str_value:str.to_wstring());
                            }
                        }
                    }
                    else if(fun_name === "lower_case") {
                        if(params.length() == 0) {
                            string str = obj.strValue.to_string();
                            string str2 = str.lower_case();
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:str2.to_wstring());
                        }
                    }
                    else if(fun_name === "reverse") {
                        if(params.length() == 0) {
                            string str = obj.strValue.to_string();
                            string str2 = str.reverse();
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:str2.to_wstring());
                        }
                    }
                    else if(fun_name === "upper_case") {
                        if(params.length() == 0) {
                            string str = obj.strValue.to_string();
                            string str2 = str.upper_case();
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:str2.to_wstring());
                        }
                    }
                    else if(fun_name === "length") {
                        if(params.length() == 0) {
                            string str = obj.strValue.to_string();
                            int len = str.length();
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:len);
                        }
                    }
                    else if(fun_name === "printable") {
                        if(params.length() == 0) {
                            string str = obj.strValue.to_string().printable();
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:str.to_wstring());
                        }
                    }
                    else if(fun_name === "scan") {
                        if(params.length() == 1 && params[0].kind == kRegexValue) {
                            regex_struct* param0 = params[0].regexValue;
                            
                            list<string>*% list1 = obj.strValue.to_string().scan(param0);
                            
                            list<ZVALUE*%>*% list2 = new list<ZVALUE*%>();
                            
                            foreach(it, list1) {
                                list2.push_back(new ZVALUE(kind:kStrValue, str_value:it.to_wstring()));
                            }
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:clone list2);
                        }
                    }
                    else if(fun_name === "compare") {
                        if(params.length() == 1 && params[0].kind == kStrValue) {
                            string param0 = params[0].strValue.to_string();
                            
                            int result2 = obj.strValue.to_string().compare(param0);
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:result2);
                        }
                    }
                    else if(fun_name === "sub") {
                        if(params.length() == 2 && params[0].kind == kRegexValue && params[1].kind == kStrValue) {
                            regex_struct* param0 = params[0].regexValue;
                            char*% param1 = params[1].strValue.to_string();
                            
                            string str = obj.strValue.to_string().sub(param0, param1);
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:str.to_wstring());
                        }
                    }
                    else if(fun_name === "delete") {
                        if(params.length() == 2 && params[0].kind == kIntValue && params[1].kind == kIntValue) {
                            int param0 = params[0].intValue;
                            int param1 = params[1].intValue;
                            
                            char* str = obj.strValue.to_string().delete(param0, param1);
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:str.to_wstring());
                        }
                    }
                    else if(fun_name === "index_count") {
                        if(params.length() == 3 && params[0].kind == kStrValue && params[1].kind == kIntValue && params[2].kind == kIntValue) {
                            string param0 = params[0].strValue.to_string();
                            int param1 = params[1].intValue;
                            int param2 = params[2].intValue;
                            
                            int n = obj.strValue.to_string().index_count(param0, param1, param2);
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:n);
                        }
                    }
                    else if(fun_name === "index_regex_count") {
                        if(params.length() == 3 && params[0].kind == kRegexValue && params[1].kind == kIntValue && params[2].kind == kIntValue) {
                            regex_struct* param0 = params[0].regexValue;
                            int param1 = params[1].intValue;
                            int param2 = params[2].intValue;
                            
                            int n = obj.strValue.to_string().index_regex_count(param0, param1, param2);
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:n);
                        }
                    }
                    else if(fun_name === "rindex_regex_count") {
                        if(params.length() == 3 && params[0].kind == kRegexValue && params[1].kind == kIntValue && params[2].kind == kIntValue) {
                            regex_struct* param0 = params[0].regexValue;
                            int param1 = params[1].intValue;
                            int param2 = params[2].intValue;
                            
                            int n = obj.strValue.to_string().rindex_regex_count(param0, param1, param2);
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:n);
                        }
                    }
                    else if(fun_name === "substring") {
                        if(params.length() == 2 && params[0].kind == kIntValue && params[1].kind == kIntValue) {
                            int param0 = params[0].intValue;
                            int param1 = params[1].intValue;
                            
                            string str = obj.strValue.to_string().substring(param0, param1);
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:str.to_wstring());
                        }
                    }
                    else if(fun_name === "split_maxsplit") {
                        if(params.length() == 2 && params[0].kind == kRegexValue && params[1].kind == kIntValue) {
                            regex_struct* param0 = params[0].regexValue;
                            int param1 = params[1].intValue;
                            
                            list<string>*% list1 = obj.strValue.to_string().split_maxsplit(param0, param1);
                            
                            list<ZVALUE*%>*% list2 = new list<ZVALUE*%>();
                            
                            foreach(it, list1) {
                                list2.push_back(new ZVALUE(kind:kStrValue, str_value:it.to_wstring()));
                            }
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:clone list2);
                        }
                    }
                    else if(fun_name === "index") {
                        if(params.length() == 2 && params[0].kind == kStrValue && params[1].kind == kIntValue) {
                            char*% param0 = params[0].strValue.to_string();
                            int param1 = params[1].intValue;
                            
                            int result2 = obj.strValue.to_string().index(param0, param1);
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:result2);
                        }
                    }
                    else if(fun_name === "index_regex") {
                        if(params.length() == 2 && params[0].kind == kRegexValue && params[1].kind == kIntValue) {
                            regex_struct* param0 = params[0].regexValue;
                            int param1 = params[1].intValue;
                            
                            int result2 = obj.strValue.to_string().index_regex(param0, param1);
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:result2);
                        }
                    }
                    else if(fun_name === "replace") {
                        if(params.length() == 2 && params[0].kind == kIntValue && params[1].kind == kStrValue) {
                            int param0 = params[0].intValue;
                            string param1 = params[1].strValue.to_string();
                            
                            char* result2 = obj.strValue.to_string().replace(param0, param1[0]);
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:result2.to_wstring());
                        }
                    }
                    else if(fun_name === "rindex") {
                        if(params.length() == 2 && params[0].kind == kStrValue && params[1].kind == kIntValue) {
                            char*% param0 = params[0].strValue.to_string();
                            int param1 = params[1].intValue;
                            
                            int result2 = obj.strValue.to_string().rindex(param0, param1);
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:result2);
                        }
                    }
                    else if(fun_name === "rindex_count") {
                        if(params.length() == 3 && params[0].kind == kStrValue && params[1].kind == kIntValue && params[2].kind == kIntValue) {
                            string param0 = params[0].strValue.to_string();
                            int param1 = params[1].intValue;
                            int param2 = params[2].intValue;
                            
                            int result2 = obj.strValue.to_string().rindex_count(param0, param1, param2);
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:result2);
                        }
                    }
                    else if(fun_name === "index_regex") {
                        if(params.length() == 2 && params[0].kind == kRegexValue && params[1].kind == kIntValue) {
                            regex_struct* param0 = params[0].regexValue;
                            int param1 = params[1].intValue;
                            
                            int result2 = obj.strValue.to_string().index_regex(param0, param1);
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:result2);
                        }
                    }
                    else if(fun_name === "rindex_regex") {
                        if(params.length() == 2 && params[0].kind == kRegexValue && params[1].kind == kIntValue) {
                            regex_struct* param0 = params[0].regexValue;
                            int param1 = params[1].intValue;
                            
                            int result2 = obj.strValue.to_string().rindex_regex(param0, param1);
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:result2);
                        }
                    }
                    else if(fun_name === "multiply") {
                        if(params.length() == 1 && params[0].kind == kIntValue) {
                            int param0 = params[0].intValue;
                            
                            string result2 = obj.strValue.to_string().multiply(param0);
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:result2.to_wstring());
                        }
                    }
                    else if(fun_name === "match") {
                        if(params.length() == 1 && params[0].kind == kRegexValue) {
                            regex_struct* param0 = params[0].regexValue;
                            
                            bool result2 = obj.strValue.to_string().match(param0);
                            
                            result = nullable new ZVALUE(kind:kBoolValue, bool_value:result2);
                        }
                    }
                    else if(fun_name === "match_count") {
                        if(params.length() == 2 && params[0].kind == kRegexValue && params[1].kind == kIntValue) {
                            regex_struct* param0 = params[0].regexValue;
                            int param1 = params[1].intValue;
                            
                            int n = obj.strValue.to_string().match_count(param0, param1);
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:n);
                        }
                    }
                    else if(fun_name === "split") {
                        if(params.length() == 1 && params[0].kind == kRegexValue) {
                            regex_struct* param0 = params[0].regexValue;
                            
                            list<string>*% list1 = obj.strValue.to_string().split(param0);
                            
                            list<ZVALUE*%>*% list2 = new list<ZVALUE*%>();
                            
                            foreach(it, list1) {
                                list2.push_back(new ZVALUE(kind:kStrValue, str_value:it.to_wstring()));
                            }
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:clone list2);
                        }
                    }
                    else if(fun_name === "strip") {
                        if(params.length() == 0) {
                            string str = obj.strValue.to_string().strip();
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:str.to_wstring());
                        }
                    }
                    else if(fun_name === "split_char") {
                        if(params.length() == 1 && params[0].kind == kStrValue) {
                            string param0 = params[0].strValue.to_string();
                            
                            list<string>*% list1 = obj.strValue.to_string().split_char(param0[0]);
                            
                            list<ZVALUE*%>*% list2 = new list<ZVALUE*%>();
                            
                            foreach(it, list1) {
                                list2.push_back(new ZVALUE(kind:kStrValue, str_value:it.to_wstring()));
                            }
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:clone list2);
                        }
                    }
                    else if(fun_name === "sub_count") {
                        if(params.length() == 3 && params[0].kind == kRegexValue && params[1].kind == kStrValue && params[2].kind == kIntValue) {
                            regex_struct* param0 = params[0].regexValue;
                            char*% param1 = params[1].strValue.to_string();
                            int param2 = params[2].intValue;
                            
                            string result2 = obj.strValue.to_string().sub_count(param0, param1, param2);
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:result2.to_wstring());
                        }
                    }
                    else if(fun_name === "sub_block") {
                        if(params.length() == 1 && params[0].kind == kRegexValue) {
                            regex_struct* param0 = params[0].regexValue;
                            
                            string str = obj.strValue.to_string().sub_block(param0) {
                                ZVALUE*% value = new ZVALUE(kind:kStrValue, str_value:it.to_wstring());
                                append_var(name:"it".to_wstring(), value: value);
                                
                                list<string>* group_strings = it2;
                                
                                list<ZVALUE*%>*% list2 = new list<ZVALUE*%>();
                                
                                foreach(it, group_strings) {
                                    list2.push_back(new ZVALUE(kind:kStrValue, str_value:it.to_wstring()));
                                }
                                
                                ZVALUE*% value2 = new ZVALUE(kind:kListValue, list_value:clone list2);
                                append_var(name:"it2".to_wstring(), value: value2);
                                
                                sInfo info2;
                                
                                info2.p = null;
                                info2.command = null;
                                info2.codes = clone *(parent.codes);
                                info2.nodes = null;
                                info2.head = (int*)info2.codes.buf;
                                info2.op = (int*)info2.codes.buf;
                                info2.stack = new vector<ZVALUE*%>();
                                info2.loop_head = 0;
                                info2.breaks = null;
                                info2.result_value = null;
                                info2.stack_num = 0;
                                
                                while(*info2.op) {
                                    if(!vm(&info2)) {
                                        exit(1);
                                    }
                                }
                                
                                if(info2.result_value == null || info2.result_value.kind != kStrValue) {
                                    fprintf(stderr, "invalid block result value\n");
                                    exit(1);
                                }
                                
                                return info2.result_value.strValue.to_string();
                            }
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:str.to_wstring());
                        }
                    }
                    else if(fun_name === "sub_block_count") {
                        if(params.length() == 2 && params[0].kind == kRegexValue && params[1].kind == kIntValue) {
                            regex_struct* param0 = params[0].regexValue;
                            int param1 = params[1].intValue;
                            
                            string str = obj.strValue.to_string().sub_block_count(param0, param1) {
                                ZVALUE*% value = new ZVALUE(kind:kStrValue, str_value:it.to_wstring());
                                append_var(name:"it".to_wstring(), value: value);
                                
                                list<string>* group_strings = it2;
                                
                                list<ZVALUE*%>*% list2 = new list<ZVALUE*%>();
                                
                                foreach(it, group_strings) {
                                    list2.push_back(new ZVALUE(kind:kStrValue, str_value:it.to_wstring()));
                                }
                                
                                ZVALUE*% value2 = new ZVALUE(kind:kListValue, list_value:clone list2);
                                append_var(name:"it2".to_wstring(), value: value2);
                                
                                sInfo info2;
                                
                                info2.p = null;
                                info2.command = null;
                                info2.codes = clone *(parent.codes);
                                info2.nodes = null;
                                info2.head = (int*)info2.codes.buf;
                                info2.op = (int*)info2.codes.buf;
                                info2.stack = new vector<ZVALUE*%>();
                                info2.loop_head = 0;
                                info2.breaks = null;
                                info2.result_value = null;
                                info2.stack_num = 0;
                                
                                while(*info2.op) {
                                    if(!vm(&info2)) {
                                        exit(1);
                                    }
                                }
                                
                                if(info2.result_value == null || info2.result_value.kind != kStrValue) {
                                    fprintf(stderr, "invalid block result value\n");
                                    exit(1);
                                }
                                
                                return info2.result_value.strValue.to_string();
                            }
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:str.to_wstring());
                        }
                    }
                    else if(fun_name === "scan_block") {
                        if(params.length() == 1 && params[0].kind == kRegexValue) {
                            regex_struct* param0 = params[0].regexValue;
                            
                            list<string>*% list1 = obj.strValue.to_string().scan_block(param0) {
                                ZVALUE*% value = new ZVALUE(kind:kStrValue, str_value:it.to_wstring());
                                append_var(name:"it".to_wstring(), value: value);
                                
                                list<ZVALUE*%>*% list2 = new list<ZVALUE*%>();
                                
                                list<string>* group_strings = it2;
                                
                                foreach(it, group_strings) {
                                    list2.push_back(new ZVALUE(kind:kStrValue, str_value:it.to_wstring()));
                                }
                                
                                ZVALUE*% value2 = new ZVALUE(kind:kListValue, list_value:clone list2);
                                append_var(name:"it2".to_wstring(), value: value2);
                                
                                sInfo info2;
                                
                                info2.p = null;
                                info2.command = null;
                                info2.codes = clone *(parent.codes);
                                info2.nodes = null;
                                info2.head = (int*)info2.codes.buf;
                                info2.op = (int*)info2.codes.buf;
                                info2.stack = new vector<ZVALUE*%>();
                                info2.loop_head = 0;
                                info2.breaks = null;
                                info2.result_value = null;
                                info2.stack_num = 0;
                                
                                while(*info2.op) {
                                    if(!vm(&info2)) {
                                        exit(1);
                                    }
                                }
                                
                                if(info2.result_value == null || info2.result_value.kind != kStrValue) {
                                    fprintf(stderr, "invalid block result value\n");
                                    exit(1);
                                }
                                
                                return info2.result_value.strValue.to_string();
                            }
                            
                            list<ZVALUE*%>*% list2 = new list<ZVALUE*%>();
                            
                            foreach(it, list1) {
                                list2.push_back(new ZVALUE(kind:kStrValue, str_value:it.to_wstring()));
                            }
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:clone list2);
                        }
                    }
                    else if(fun_name === "scan_block_count") {
                        if(params.length() == 2 && params[0].kind == kRegexValue && params[1].kind == kIntValue) {
                            regex_struct* param0 = params[0].regexValue;
                            int param1 = params[1].intValue;
                            
                            list<string>*% list1 = obj.strValue.to_string().scan_block_count(param0, param1) {
                                ZVALUE*% value = new ZVALUE(kind:kStrValue, str_value:it.to_wstring());
                                append_var(name:"it".to_wstring(), value: value);
                                
                                list<ZVALUE*%>*% list2 = new list<ZVALUE*%>();
                                
                                list<string>* group_strings = it2;
                                
                                foreach(it, group_strings) {
                                    list2.push_back(new ZVALUE(kind:kStrValue, str_value:it.to_wstring()));
                                }
                                
                                ZVALUE*% value2 = new ZVALUE(kind:kListValue, list_value:clone list2);
                                append_var(name:"it2".to_wstring(), value: value2);
                                
                                sInfo info2;
                                
                                info2.p = null;
                                info2.command = null;
                                info2.codes = clone *(parent.codes);
                                info2.nodes = null;
                                info2.head = (int*)info2.codes.buf;
                                info2.op = (int*)info2.codes.buf;
                                info2.stack = new vector<ZVALUE*%>();
                                info2.loop_head = 0;
                                info2.breaks = null;
                                info2.result_value = null;
                                info2.stack_num = 0;
                                
                                while(*info2.op) {
                                    if(!vm(&info2)) {
                                        exit(1);
                                    }
                                }
                                
                                if(info2.result_value == null || info2.result_value.kind != kStrValue) {
                                    fprintf(stderr, "invalid block result value\n");
                                    exit(1);
                                }
                                
                                return info2.result_value.strValue.to_string();
                            }
                            
                            list<ZVALUE*%>*% list2 = new list<ZVALUE*%>();
                            
                            foreach(it, list1) {
                                list2.push_back(new ZVALUE(kind:kStrValue, str_value:it.to_wstring()));
                            }
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:clone list2);
                        }
                    }
                    }
                    break;
                    
                case kFileValue: {
                    if(fun_name === "read") {
                        if(params.length() == 0) {
                            string str = obj.fileValue.read();
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:str.to_wstring());
                        }
                    }
                    else if(fun_name === "sub_block") {
                        if(params.length() == 1 && params[0].kind == kRegexValue) {
                            regex_struct* param0 = params[0].regexValue;
                            
                            string str = obj.strValue.to_string().sub_block(param0) {
                                ZVALUE*% value = new ZVALUE(kind:kStrValue, str_value:it.to_wstring());
                                append_var(name:"it".to_wstring(), value: value);
                                
                                list<string>* group_strings = it2;
                                
                                list<ZVALUE*%>*% list2 = new list<ZVALUE*%>();
                                
                                foreach(it, group_strings) {
                                    list2.push_back(new ZVALUE(kind:kStrValue, str_value:it.to_wstring()));
                                }
                                
                                ZVALUE*% value2 = new ZVALUE(kind:kListValue, list_value:clone list2);
                                append_var(name:"it2".to_wstring(), value: value2);
                                
                                sInfo info2;
                                
                                info2.p = null;
                                info2.command = null;
                                info2.codes = clone *(parent.codes);
                                info2.nodes = null;
                                info2.head = (int*)info2.codes.buf;
                                info2.op = (int*)info2.codes.buf;
                                info2.stack = new vector<ZVALUE*%>();
                                info2.loop_head = 0;
                                info2.breaks = null;
                                info2.result_value = null;
                                info2.stack_num = 0;
                                
                                while(*info2.op) {
                                    if(!vm(&info2)) {
                                        exit(1);
                                    }
                                }
                                
                                if(info2.result_value == null || info2.result_value.kind != kStrValue) {
                                    fprintf(stderr, "invalid block result value\n");
                                    exit(1);
                                }
                                
                                return info2.result_value.strValue.to_string();
                            }
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:str.to_wstring());
                        }
                    }
                    }
                    break;
                    
                case kListValue: {
                    if(fun_name === "join") {
                        if(params.length() == 1 && params[0].kind == kStrValue) {
                            string param0 = params[0].strValue.to_string();
                            
                            list<ZVALUE*%>* list1 = obj.listValue;
                            
                            list<string>*% list2 = new list<string>();
                            
                            foreach(it, list1) {
                                list2.push_back(it.strValue.to_string());
                            }
                            
                            string str = list2.join(param0);
                            
                            result = nullable new ZVALUE(kind:kStrValue, str_value:str.to_wstring());
                        }
                    }
                    else if(fun_name === "filter") {
                        if(params.length() == 0) {
                            list<ZVALUE*%>*% list1 = obj.listValue.filter {
                                ZVALUE*% value = clone it;
                                append_var(name:"it".to_wstring(), value: value);
                                
                                sInfo info2;
                                
                                info2.p = null;
                                info2.command = null;
                                info2.codes = clone *(parent.codes);
                                info2.nodes = null;
                                info2.head = (int*)info2.codes.buf;
                                info2.op = (int*)info2.codes.buf;
                                info2.stack = new vector<ZVALUE*%>();
                                info2.loop_head = 0;
                                info2.breaks = null;
                                info2.result_value = null;
                                info2.stack_num = 0;
                                
                                while(*info2.op) {
                                    if(!vm(&info2)) {
                                        exit(1);
                                    }
                                }
                                
                                if(info2.result_value == null || info2.result_value.kind != kBoolValue) {
                                    fprintf(stderr, "invalid block result value\n");
                                    exit(1);
                                }
                                
                                return info2.result_value.boolValue;
                            }
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:list1);
                        }
                    }
                    else if(fun_name === "map") {
                        if(params.length() == 0) {
                            list<ZVALUE*%>*% list1 = obj.listValue.map {
                                ZVALUE*% value = clone it;
                                append_var(name:"it".to_wstring(), value: value);
                                
                                sInfo info2;
                                
                                info2.p = null;
                                info2.command = null;
                                info2.codes = clone *(parent.codes);
                                info2.nodes = null;
                                info2.head = (int*)info2.codes.buf;
                                info2.op = (int*)info2.codes.buf;
                                info2.stack = new vector<ZVALUE*%>();
                                info2.loop_head = 0;
                                info2.breaks = null;
                                info2.result_value = null;
                                info2.stack_num = 0;
                                
                                while(*info2.op) {
                                    if(!vm(&info2)) {
                                        exit(1);
                                    }
                                }
                                
                                return info2.result_value;
                            }
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:list1);
                        }
                    }
                    else if(fun_name === "each") {
                        if(params.length() == 0) {
                            list<ZVALUE*%>* list1 = obj.listValue.each {
                                ZVALUE*% value = clone it;
                                append_var(name:"it".to_wstring(), value: value);
                                
                                sInfo info2;
                                
                                info2.p = null;
                                info2.command = null;
                                info2.codes = clone *(parent.codes);
                                info2.nodes = null;
                                info2.head = (int*)info2.codes.buf;
                                info2.op = (int*)info2.codes.buf;
                                info2.stack = new vector<ZVALUE*%>();
                                info2.loop_head = 0;
                                info2.breaks = null;
                                info2.result_value = null;
                                info2.stack_num = 0;
                                
                                while(*info2.op) {
                                    if(!vm(&info2)) {
                                        exit(1);
                                    }
                                }
                            }
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:list1);
                        }
                    }
                    else if(fun_name === "add") {
                        if(params.length() == 1) {
                            ZVALUE* param0 = params[0];
                            
                            list<ZVALUE*%>* list1 = obj.listValue;
                            
                            list1.push_back(clone param0);
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:list1);
                        }
                    }
                    else if(fun_name === "insert") {
                        if(params.length() == 2 && params[0].kind == kIntValue) {
                            int param0 = params[0].intValue;
                            ZVALUE* param1 = params[1];
                            
                            list<ZVALUE*%>* list1 = obj.listValue;
                            
                            list1.insert(param0, clone param1);
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:list1);
                        }
                    }
                    else if(fun_name === "delete") {
                        if(params.length() == 2 && params[0].kind == kIntValue && params[1].kind == kIntValue) {
                            int param0 = params[0].intValue;
                            int param1 = params[1].intValue;
                            
                            list<ZVALUE*%>* list1 = obj.listValue;
                            
                            list1.delete(param0, param1);
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:list1);
                        }
                    }
                    else if(fun_name === "replace") {
                        if(params.length() == 2 && params[0].kind == kIntValue) {
                            int param0 = params[0].intValue;
                            ZVALUE* param1 = params[1];
                            
                            list<ZVALUE*%>* list1 = obj.listValue;
                            
                            list1.replace(param0, clone param1);
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:list1);
                        }
                    }
                    else if(fun_name === "sublist") {
                        if(params.length() == 2 && params[0].kind == kIntValue && params[1].kind == kIntValue) {
                            int param0 = params[0].intValue;
                            int param1 = params[1].intValue;
                            
                            list<ZVALUE*%>* list1 = obj.listValue;
                            
                            list<ZVALUE*%>*% list2 = list1.sublist(param0, param1);
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:list2);
                        }
                    }
                    else if(fun_name === "uniq") {
                        if(params.length() == 0) {
                            list<ZVALUE*%>* list1 = obj.listValue;
                            
                            list<ZVALUE*%>*% list2 = list1.uniq();
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:list2);
                        }
                    }
                    else if(fun_name === "reverse") {
                        if(params.length() == 0) {
                            list<ZVALUE*%>* list1 = obj.listValue;
                            
                            list<ZVALUE*%>*% list2 = list1.reverse();
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:list2);
                        }
                    }
                    else if(fun_name === "sort") {
                        if(params.length() == 0) {
                            list<ZVALUE*%>* list1 = obj.listValue;
                            
                            list<ZVALUE*%>*% list2 = list1.sort_default();
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:list2);
                        }
                    }
                    else if(fun_name === "find") {
                        if(params.length() == 1) {
                            ZVALUE* param0 = params[0];
                            
                            list<ZVALUE*%>* list1 = obj.listValue;
                            
                            int n = list1.find(param0, -1);
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:n);
                        }
                    }
                    else if(fun_name === "length") {
                        if(params.length() == 0) {
                            list<ZVALUE*%>* list1 = obj.listValue;
                            
                            int n = list1.length();
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:n);
                        }
                    }
                    }
                    break;
                    
                case kMapValue: {
                    if(fun_name === "length") {
                        if(params.length() == 0) {
                            map<ZVALUE*%, ZVALUE*%>* map1 = obj.mapValue;
                            
                            int n = map1.length();
                            
                            result = nullable new ZVALUE(kind:kIntValue, int_value:n);
                        }
                    }
                    else if(fun_name === "find") {
                        if(params.length() == 1) {
                            ZVALUE* param0 = params[0];
                            
                            map<ZVALUE*%, ZVALUE*%>* map1 = obj.mapValue;
                            
                            bool n = map1.find(param0);
                            
                            result = nullable new ZVALUE(kind:kBoolValue, bool_value:n);
                        }
                    }
                    else if(fun_name === "keys") {
                        if(params.length() == 0) {
                            map<ZVALUE*%, ZVALUE*%>* map1 = obj.mapValue;
                            
                            list<ZVALUE*%>*% l = map1.keys();
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:l);
                        }
                    }
                    else if(fun_name === "values") {
                        if(params.length() == 0) {
                            map<ZVALUE*%, ZVALUE*%>* map1 = obj.mapValue;
                            
                            list<ZVALUE*%>*% l = map1.values();
                            
                            result = nullable new ZVALUE(kind:kListValue, list_value:l);
                        }
                    }
                    }
                    break;
            }
            
            if(result == null) {
                fprintf(stderr, "method not found(%s)\n", fun_name);
                exit(1);
            }
            
            info.stack.push_back(result!);
            }
            break;
    }
    
    return true;
}

sNode*? exp_node(sInfo* info) version 9
{
    sNode*? result = null;
    if(*info->p == '.') {
        result = inherit(info);
        
        string fun_name = string("read");
        vector<sNode*%>*% params = new vector<sNode*%>();
        sNodeBlock? block = null
        result = borrow new sNode(new sFunNode(fun_name, clone result!, params, block));
    }
    else {
        result = inherit(info);
    }
    
    while(*info->p == '.') {
        info->p++;
        skip_spaces(info);
        
        if(result == null) {
            fprintf(stderr, "invalid method call. require object\n");
            exit(2);
        }
        
        buffer*% buf = new buffer();
        
        while(xisalnum(*(info->p)) || *info->p == '_') {
            buf.append_char(*info->p);
            info->p++;
        }
        skip_spaces(info);
        
        string fun_name = buf.to_string();
        
        vector<sNode*%>*% params = new vector<sNode*%>();
        
        if(*info->p != '{' && *info->p == '(') {
            if(*info->p == '(') {
                info->p++;
                skip_spaces(info);
            }
            
            while(true) {
                if(*info->p == ')') {
                    info->p++;
                    skip_spaces(info);
                    break;
                }
                
                sNode*? node = expression(info);
                
                if(node == null) {
                    fprintf(stderr, "require parametor expression\n");
                    exit(1);
                }
                
                params.push_back(clone node!);
                
                delete node!;
                
                if(*info->p == ',') {
                    info->p++;
                    skip_spaces(info);
                }
            }
        }
        
        sNodeBlock? block = null
        if(*info->p == '{') {
            block = parse_block(info);
        }
        
        sNode*? result2 = borrow new sNode(new sFunNode(fun_name, clone result!, params, block));
        
        delete result!;
        
        result = result2;
    }
    
    return result;
}
