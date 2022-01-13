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

state_t tick(char c, int arg_max);

void print_state(state_t state, char c);