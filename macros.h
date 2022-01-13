
/* ----------------------------------------------------------------------------
String Helpers
---------------------------------------------------------------------------- */

typedef struct mystring {
    char* data; 
    size_t size;
    size_t capacity;
} string_t;

string_t* createString();

void addChar(string_t* s, char c);

void appendString(string_t* a, string_t* b);

void appendFile(string_t* s, char* file);

void destroyString(string_t* s);

void printString(string_t* s);

void clearString(string_t* s);

int stringCompare(string_t* a, string_t* b);

/* ----------------------------------------------------------------------------
Macro Helpers
---------------------------------------------------------------------------- */

typedef struct {
    string_t* macro_name; 
    string_t* definition;
} macro_t;

typedef struct {
	macro_t** data; 
	size_t size;
	size_t capacity;
} macro_list_t;

macro_list_t* createMacroList();

void destroyMacro(macro_t m);

void macro_def(macro_list_t* macro_list, string_t* name, string_t* definition);

void macro_undef(macro_list_t* macro_list, string_t* name);

void destroyMacroList(macro_list_t* ml);

void printMacroList(macro_list_t* ml);

int macro_locate(macro_list_t* macro_list, string_t* name);

string_t* macro_expand(macro_list_t* macro_list, string_t* name, string_t* arg1, string_t* expansion);

/* ----------------------------------------------------------------------------
Stack Helpers
---------------------------------------------------------------------------- */


typedef struct stack_entry {
  string_t *string;
  struct stack_entry *next;
  size_t place;
} stack_entry_t;

typedef struct {
  stack_entry_t *head;
} stack_t;

stack_t *createStack(void);

void push(stack_t *stack, string_t *string);

stack_entry_t *top(stack_t *stack);

void pop(stack_t *stack);

void clear(stack_t *stack);

void destroyStack(stack_t *stack);