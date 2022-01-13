#include "proj1.h"
#include "statemachine.h"
#include "macros.h"
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/* Finds the correct number of arguments based on a given macro name */
int findArgCount(char* macro_name) {
    if (strcmp(macro_name, "def") == 0 || strcmp(macro_name, "expandafter") == 0) {
        return 2;
    }
    else if (strcmp(macro_name, "if") == 0 || strcmp(macro_name, "ifdef") == 0) {
        return 3;
    }
    else {
        return 1;
    }
}

/* Macro processing function which reads macro name and arguments and expands
into an expansion string or performs built in macros */
string_t *processMacro(string_t* macro_name, string_t* arg1, string_t* arg2,
                       string_t* arg3, macro_list_t* ml, string_t* expansion) {    
    if (!strcmp(macro_name->data, "def")) {
        macro_def(ml, arg1, arg2);
    } else if (!strcmp(macro_name->data, "undef")) {
        macro_undef(ml, arg1);
    } else if (!strcmp(macro_name->data, "if")) {
        if(arg1->size != 0) {
            appendString(expansion, arg2);
        } else {
            appendString(expansion, arg3);
        }
    } else if (!strcmp(macro_name->data, "ifdef")) {
        if(macro_locate(ml, arg1) != -1) {
            appendString(expansion, arg2);
        } else {
            appendString(expansion, arg3);
        }
    } else if (!strcmp(macro_name->data, "include")) {
        appendFile(expansion, arg1->data);
    } else {
        macro_expand(ml, macro_name, arg1, expansion);
    }
    return expansion;
}

/* Main driver function, which establishes state machine and expansion stack,
converting text string into output string */
state_t expand(string_t* text, string_t* output, macro_list_t* ml) {

    /* Initialize stack and stack entry pointer */
    stack_t* es = createStack();
    stack_entry_t* entry;
    
    /* Initialize string buffers */
    string_t* macro_name = createString();
    string_t* arg1 = createString();
    string_t* arg2 = createString();
    string_t* arg3 = createString();
    string_t* expansion = createString();

    /* Initialize state and argument count */
    state_t state = state_plaintext;
    int arg_count = 0;

    /* Push entire text input onto stack */
    push(es, text);

    /* Pop stack until empty */
    LOOP:while((entry = top(es)) != NULL) {
        /* Loop through each character of stack entry and tick state machine */
        for(; entry->place < entry->string->size; entry->place++) {
            char c = entry->string->data[entry->place];
            state = tick(c, arg_count);
            // print_state(state, c);
            
            /* Based on state, add character to output or character buffer until
            enough characters are read to process macro */
            switch(state){
                case state_plaintext:
                    addChar(output, c);
                    break;
                case state_not_alpha_or_escape:
                    addChar(output, '\\');
                    addChar(output, c);
                    break;
                case state_macro:
                    addChar(macro_name, c);
                    break;
                case state_argument1:
                case state_argument1_escape:
                    addChar(arg1, c);
                    break;
                case state_argument2:
                case state_argument2_escape:
                    addChar(arg2, c);
                    break;
                case state_argument3:
                case state_argument3_escape:
                    addChar(arg3, c);
                    break;
                case state_argument1_begin:
                    arg_count = findArgCount(macro_name->data);
                    break;
                case state_argument2_begin:
                case state_argument3_begin:
                case state_argument1_end:
                case state_argument2_end:
                case state_comment:
                case state_after_comment:
                case state_escape:
                    break;
                case state_macro_end:
                    /* Reset argument counter */
                    arg_count = 0;

                    /* Handle "expandafter" macro separately, recursively calling expand
                    on AFTER argument and concatenating on unexpanded BEFORE argument */
                    if (!strcmp(macro_name->data, "expandafter")) {
                        /* Initialize temporary buffer for expanded AFTER argument */
                        string_t* after_buffer = createString();
                        
                        /* Recursive expand call on AFTER argument */
                        state = expand(arg2, after_buffer, ml);

                        /* Concatenating BEFORE argument and expanded AFTER argument */
                        appendString(expansion, arg1);
                        appendString(expansion, after_buffer);
                        
                        destroyString(after_buffer);
                    } else {
                        /* Call general macro processing funcion */
                        expansion = processMacro(macro_name, arg1, arg2, arg3, ml, expansion);
                    }

                    /* Reset buffer strings */
                    clearString(macro_name);
                    clearString(arg1);
                    clearString(arg2);
                    clearString(arg3);

                    /* Push new expansion onto stack if necessary and break out of
                    both loops to immediately expand top of stack */
                    if(expansion->size > 0) {
                        push(es, expansion);
                        clearString(expansion);
                        entry->place++;
                        goto LOOP;
                    }

                    break;
            }            
        }
        
        /* Pop expansion stack when finished top stack entry */
        pop(es);
    }

    /* Destroy string buffers and expansion stack */
    destroyString(macro_name);
    destroyString(expansion);
    destroyString(arg1);
    destroyString(arg2);
    destroyString(arg3);
    destroyStack(es);

    return state;
}

/* Main function responsible for reading input and printing output */
int main(int argc, char **argv) {
    /* Initialize input and output buffers and user-defined macro list */
    string_t* input = createString();
    string_t* output = createString();
    macro_list_t* ml = createMacroList();

    int c;

    /* Read from standard input */
    if(argc == 1) {
        while((c = getchar()) != EOF) {
            addChar(input, c);
        }
    /* Read from files */
    } else if (argc > 1) {
        for(int i = 1; i < argc; i++) {
            appendFile(input, argv[i]);
        }
    }

    /* Call expand functino on entire input */
    state_t state = expand(input, output, ml);

    /* Check if ending state is valid */
    if(state != state_plaintext && state != state_comment && state != state_after_comment
    && state != state_macro_end && state != state_not_alpha_or_escape) {
        DIE("%s", "invalid end");
    }
    
    /* Print output buffer */
    printString(output);

    /* Destroy input and output buffers and user-defined macro list */
    destroyString(input);
    destroyString(output);
    destroyMacroList(ml);

    return 0;
}