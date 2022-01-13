#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include "proj1.h"

typedef enum myenum {
    state_plaintext,
    state_escape,
    state_macro,
    state_comment,
    state_after_comment,
    state_argument1_begin,
    state_argument1,
    state_argument1_escape,
    state_argument1_end,
    state_argument2_begin,
    state_argument2,
    state_argument2_escape,
    state_argument2_end,
    state_argument3_begin,
    state_argument3,
    state_argument3_escape,
    state_macro_end,
    state_not_alpha_or_escape,
} state_t;

/* Checks whether or not character is defined as escapable */
bool isescapechar(char c) {
    return (c == '\\' || c == '#' || c == '%' || c == '{' || c == '}');
}

/* Balanced parenthases algorithm */
bool balanced_paren(bool x) {
    static size_t n = 0;
    if(x) {
        n++;
    } else {
        n--;
    }
    if (n < 0) {
        DIE("%s", "Invalid parentheses.");
    } else if (n == 0) {
        return true;
    } else {
        return false;
    }
}

/* State machine ticker */
state_t tick(char c, int arg_max) {
    static state_t state = state_plaintext;
    static state_t prev_state = state_plaintext;

    if(c == '%' && state != state_escape) {
        prev_state = state;
        state = state_comment;
    }

    switch(state){
        case state_plaintext:
        case state_not_alpha_or_escape:
        case state_macro_end:
            if(c == '\\') {
                state = state_escape;
            } else {
                state = state_plaintext;
            }
            break;

        case state_escape:
            if(isalnum(c)) {
                state = state_macro;
            } else if (isescapechar(c)) {
                state = state_plaintext;
            } else {
                state = state_not_alpha_or_escape;
            }
            break;
        
        case state_comment:
            if(c == '\n') {
                state = state_after_comment;
            } else {
                state = state_comment;
            }
            
            break;
        case state_after_comment:
            if(c == '\t' || c == ' ') {
                break;
            }
            state = prev_state;
            tick(c, arg_max);
            break;

        case state_macro:
            if(c == '{') {
                assert(!balanced_paren(true));
                state = state_argument1_begin;
            } else {
                if(!isalnum(c)) {
                    DIE("%s", "Macro not alphanumeric.");
                }
                state = state_macro;
            }
            break;
        
        case state_argument1:
        case state_argument2:
        case state_argument3:
            if (c == '\\') {
                if (state == state_argument1) {
                    state = state_argument1_escape;
                    break;
                } else if (state == state_argument2) {
                    state = state_argument2_escape;
                    break;
                } else if (state == state_argument3) {
                    state = state_argument3_escape;
                    break;
                }
            } else if(c == '{') {
                assert(!balanced_paren(true));
            } else if (c == '}') {
                if(balanced_paren(false)) {
                    switch(arg_max) {
                        case 1:
                            state = state_macro_end;
                            break;
                        case 2:
                            if(state == state_argument2) {
                                state = state_macro_end;
                            } else {
                                state = state_argument1_end;
                            }
                            break;
                        case 3:
                            if(state == state_argument3) {
                                state = state_macro_end;
                            } else if (state == state_argument1) {
                                state = state_argument1_end;
                            } else {
                                state = state_argument2_end;
                            }
                            break;
                    }
                }
            }
            break;

        case state_argument1_escape:
            state = state_argument1;
            break;
        case state_argument2_escape:
            state = state_argument2;
            break;
        case state_argument3_escape:
            state = state_argument3;
            break;
            

        case state_argument1_begin:
            if (c == '\\') {
                state = state_argument1_escape;
                break;
            }
            if(c == '{') {
                assert(!balanced_paren(true));
            }
            if(c == '}') {
                assert(balanced_paren(false));
                if(arg_max == 1) {
                    state = state_macro_end;
                } else {
                    state = state_argument1_end;
                }
            } else {
                state = state_argument1;
            }
            break;
        case state_argument2_begin:
            if (c == '\\') {
                state = state_argument2_escape;
                break;
            }
            if(c == '{') {
                assert(!balanced_paren(true));
            }
            if(c == '}') {
                assert(balanced_paren(false));
                if(arg_max == 2) {
                    state = state_macro_end;
                } else {
                    state = state_argument2_end;
                }
            } else {
                state = state_argument2;
            }
            break;
        case state_argument3_begin:
            if (c == '\\') {
                state = state_argument3_escape;
                break;
            }
            if(c == '{') {
                assert(!balanced_paren(true));
            }
            if(c == '}') {
                assert(balanced_paren(false));
                state = state_macro_end;
            } else {
                state = state_argument3;
            }
            break;

        case state_argument1_end:
            if(arg_max == 1) {
                state = state_macro_end;
            } else {
                if(c != '{') {
                    DIE("%s", "Expected {.");
                }
                assert(!balanced_paren(true));
                state = state_argument2_begin;
            }
            break;
        case state_argument2_end:
            if(arg_max == 2) {
                state = state_macro_end;
            } else {
                if(c != '{') {
                    DIE("%s", "Expected {.");
                }
                assert(!balanced_paren(true));
                state = state_argument3_begin;
            }
            break;
    }

    return state;
}

/* Print current state */
void print_state(state_t state, char c) {
    putchar(c);
    putchar(' ');
    switch(state){
        case state_plaintext:
            printf("state_plaintext\n");
            break;
        case state_escape:
            printf("state_escape\n");
            break;
        case state_macro:
            printf("state_macro\n");
            break;
        case state_comment:
            printf("state_comment\n");
            break;
        case state_after_comment:
            printf("state_after_comment\n");
            break;
        case state_argument1:
            printf("state_argument1\n");
            break;
        case state_argument2:
            printf("state_argument2\n");
            break;
        case state_argument3:
            printf("state_argument3\n");
            break;
        case state_argument1_begin:
            printf("state_argument1_begin\n");
            break;
        case state_argument2_begin:
            printf("state_argument2_begin\n");
            break;
        case state_argument1_end:
            printf("state_argument1_end\n");
            break;
        case state_argument2_end:
            printf("state_argument2_end\n");
            break;
        case state_argument3_begin:
            printf("state_argument3_begin\n");
        case state_argument1_escape:
            printf("state_argument1_escape\n");
            break;
        case state_argument2_escape:
            printf("state_argument2_escape\n");
            break;
        case state_argument3_escape:
            printf("state_argument3_escape\n");
            break;
        case state_macro_end:
            printf("state_macro_end\n");
            break;
        case state_not_alpha_or_escape:
            printf("state_not_alpha_or_escape\n");
            break;
    }
}