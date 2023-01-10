uint8_t hash(const char* s)
{
   uint8_t h = 3;
   for (; *s; s++) {
       h *= 19;
       h ^= *s;
   }
   return h;
}

uint8_t first_word_hash(char* s, char** start, char** end)
{
    for(; isspace(*s); ++s) { }
    if(!*s) {
        *start = NULL;
        return -1;
    }
    uint8_t h = 3;
    for(; *s && !isspace(*s); ++s) {
       h *= 19;
       h ^= *s;
    }
    *end = s;
    return h;}

return_type map(char* s) {
    switch (hash(s)) {
        case 127:
            if (strcmp("lea", s) == 0) {
                match_lea
            } else { 
                match_error
            }
            break;
        case 95:
            if (strcmp("mov", s) == 0) {
                match_mov 
            } else {
                match_error 
            }
            break;
        case 163:
            if (strcmp("exit", s) == 0) {
                match_exit 
            } else {
                match_error 
            }
            break;
        case 104:
            if (strcmp("ret", s) == 0) {
                match_ret 
            } else {
                match_error 
            }
            break;
        case 212:
            if (strcmp("cmov", s) == 0) {
                match_cmov 
            } else {
                match_error 
            }
            break;
        case 124:
            if (strcmp("jmp", s) == 0) {
                match_jmp 
            } else {
                match_error 
            }
            break;
        case 195:
            if (strcmp("cjmp", s) == 0) {
                match_cjmp 
            } else {
                match_error 
            }
            break;
        case 3:
            if (strcmp("push", s) == 0) {
                match_push 
            } else {
                match_error 
            }
            break;
        case 60:
            if (strcmp("pop", s) == 0) {
                match_pop 
            } else {
                match_error 
            }
            break;
        case 241:
            if (strcmp("stor", s) == 0) {
                match_stor 
            } else {
                match_error 
            }
            break;
        case 119:
            if (strcmp("load", s) == 0) {
                match_load 
            } else {
                match_error 
            }
            break;
        case 224:
            if (strcmp("add", s) == 0) {
                match_add 
            } else {
                match_error 
            }
            break;
        case 179:
            if (strcmp("sub", s) == 0) {
                match_sub 
            } else {
                match_error 
            }
            break;
        case 7:
            if (strcmp("mul", s) == 0) {
                match_mul 
            } else {
                match_error 
            }
            break;
        case 252:
            if (strcmp("div", s) == 0) {
                match_div 
            } else {
                match_error 
            }
            break;
        case 77:
            if (strcmp("mod", s) == 0) {
                match_mod 
            } else {
                match_error 
            }
            break;
        case 118:
            if (strcmp("and", s) == 0) {
                match_and 
            } else {
                match_error 
            }
            break;
        case 16:
            if (strcmp("or", s) == 0) {
                match_or 
            } else {
                match_error 
            }
            break;
        case 134:
            if (strcmp("xor", s) == 0) {
                match_xor 
            } else {
                match_error 
            }
            break;
        case 234:
            if (strcmp("comp", s) == 0) {
                match_comp 
            } else {
                match_error 
            }
            break;
        case 18:
            if (strcmp("sl", s) == 0) {
                match_sl 
            } else {
                match_error 
            }
            break;
        case 12:
            if (strcmp("sr", s) == 0) {
                match_sr 
            } else {
                match_error 
            }
            break;
        case 207:
            if (strcmp("call", s) == 0) {
                match_call 
            } else {
                match_error 
            }
            break;
        case 165:
            if (strcmp("eq", s) == 0) {
                match_eq 
            } else {
                match_error 
            }
            break;
        case 65:
            if (strcmp("neq", s) == 0) {
                match_neq 
            } else {
                match_error 
            }
            break;
        case 142:
            if (strcmp("gt", s) == 0) {
                match_gt 
            } else {
                match_error 
            }
            break;
        case 188:
            if (strcmp("geq", s) == 0) {
                match_geq 
            } else {
                match_error 
            }
            break;
        case 59:
            if (strcmp("lt", s) == 0) {
                match_lt 
            } else {
                match_error 
            }
            break;
        case 111:
            if (strcmp("leq", s) == 0) {
                match_leq 
            } else {
                match_error 
            }
            break;
        case 154:
            if (strcmp("not", s) == 0) {
                match_not 
            } else {
                match_error 
            }
            break;
        case 34:
            if (strcmp("syscall", s) == 0) {
                match_syscall 
            } else {
                match_error 
            }
            break;
        default:
            match_error break;}}
