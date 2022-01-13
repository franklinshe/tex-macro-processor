#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proj1.h"

#define SIZE 8 // Initial number of items for array


/* ----------------------------------------------------------------------------
String Helpers
---------------------------------------------------------------------------- */

/* Custom string data type */
typedef struct mystring {
	char* data;
	size_t size;
	size_t capacity;
} string_t;

/* Initializes a string_t */
string_t* createString() {
	string_t* s = malloc(sizeof(string_t));
	s->data = malloc(SIZE * sizeof(char));
	s->size = 0;
	s->capacity = SIZE;
	return s;
}

/* Adds a character to a string_t and doubles memory if necessary */
void addChar(string_t* s, char c) {
	s->data[s->size] = c;
	s->size++;
	if (s->size >= s->capacity) {
		s->data = DOUBLE(s->data, s->capacity);
	}
	s->data[s->size] = '\0';
}

/* Appends two strings */
void appendString(string_t* a, string_t* b) {
	for(int i = 0; i < b->size; i++) {
		addChar(a, b->data[i]);
	}
}

/* Reads file and appends to string */
void appendFile(string_t* s, char* file) {
	FILE *fp;
	fp = fopen(file, "r");
	int fpc;
	while((fpc = fgetc(fp)) != EOF) {
		addChar(s, fpc);
	}
	fclose(fp);
}

/* Destroy string */
void destroyString(string_t* s) {
	free(s->data);
	free(s);
}

/* Print string */
void printString(string_t* s) {
	for(int i = 0; i < s->size; i++) {
		putchar(s->data[i]);
	}
}

/* Clears string data and reset size to 0 */
void clearString(string_t* s) {
	free(s->data);
	s->size = 0;
	s->capacity = SIZE;
	s->data = malloc(SIZE * sizeof(char));
}

/* Clears string data and reset size to 0 */
int stringCompare(string_t* a, string_t* b) {
	return strcmp(a->data, b->data);
}

/* Initialize new string_t and copy data */
string_t *copyString(string_t *str) {
	string_t *tmp = createString();
	if (tmp) {
		for(int i = 0; i < str->size; i++) {
			addChar(tmp, str->data[i]);
		}
	}
	return tmp;
}

/* ----------------------------------------------------------------------------
Macro Helpers
---------------------------------------------------------------------------- */

/* User-defined macro data type */
typedef struct {
	string_t* macro_name;
	string_t* definition;
} macro_t;

/* User-defined macro list data type */
typedef struct {
	macro_t** data; 
	size_t size;
	size_t capacity;
} macro_list_t;

/* Initialize macro list */
macro_list_t* createMacroList() {
	macro_list_t* ml = malloc(sizeof(macro_list_t));
	ml->data = malloc(8 * sizeof(macro_t));
	ml->size = 0;
	ml->capacity = 8;
	return ml;
}

/* Destroy macro */
void destroyMacro(macro_t* m) {
	destroyString(m->macro_name);
	destroyString(m->definition);
	free(m);
}

/* Destroy macro list */
void destroyMacroList(macro_list_t* ml) {
	for(int i = 0; i < ml->size; i++) {
		destroyMacro(ml->data[i]);
	}
	free(ml->data);
	free(ml);
}

/* Locates whether macro is defined or not */
int macro_locate(macro_list_t* macro_list, string_t* name) {
	for(int i = 0; i < macro_list->size; i++) {
		if(!strcmp(name->data, macro_list->data[i]->macro_name->data)) {
			return i;
		}
	}
	return -1;
}

/* Defines macro to macro list */
void macro_def(macro_list_t* macro_list, string_t* name, string_t* def) {
	/* Throw error if already defined */
	if(macro_locate(macro_list, name) != -1) {
		DIE("%s", "Cannot redefine macro.");
	}

	string_t *macro_name = copyString(name);
	string_t *definition = copyString(def);
	macro_t *m = malloc(sizeof(macro_t));
	m->macro_name = macro_name;
	m->definition = definition;
	macro_list->data[macro_list->size] = m;
	
	macro_list->size++;
	if (macro_list->size >= macro_list->capacity) {
		macro_list->data = DOUBLE(macro_list->data, macro_list->capacity);
	}
}

/* Undefine macro to macro list */
void macro_undef(macro_list_t* macro_list, string_t* name) {
	/* Throw error if cannot find macro */
	int i = macro_locate(macro_list, name);
	if(i == -1) {
		DIE("%s", "Macro not defined.");
	}
	destroyMacro(macro_list->data[i]);

	/* Shift indices of all macros after undefined macro */
	for (int j = i; j < macro_list->size - 1; j++) {
		macro_list->data[j] = macro_list->data[j+1];
	}
	macro_list->size--;
	return;
}

/* Expand user-defined macro with arguments */
void macro_expand(macro_list_t* macro_list, string_t* name, string_t* arg1, string_t* expansion) {
	/* escape boolean for handeling '\#' */
	static bool escape = false;
	/* Throw error if cannot find macro */
	int i = macro_locate(macro_list, name);
	if(i == -1) {
		DIE("%s", "Macro not defined.");
	}

	/* Add definition to expansion buffer character by character and injecting user argument */
	for(int j = 0; j < macro_list->data[i]->definition->size; j++) {
		if(macro_list->data[i]->definition->data[j] == '#' && !escape) {
			appendString(expansion, arg1);
		} else {
			if(!escape && macro_list->data[i]->definition->data[j] == '\\') {
				escape = true;
			} else {
				escape = false;
			}
			addChar(expansion, macro_list->data[i]->definition->data[j]);
		}
	}
	return;
}

/* ----------------------------------------------------------------------------
Stack Helpers
---------------------------------------------------------------------------- */

/* String stack entry data type */
typedef struct stack_entry {
	string_t *string;
	struct stack_entry *next;
	size_t place;
} stack_entry_t;

/* String stack data type */
typedef struct {
	stack_entry_t *head;
} stack_t;

/* Initialize a stack */
stack_t *createStack(void) {
	stack_t *stack = malloc(sizeof *stack);
	stack->head = NULL;
	return stack;
}

/* Push a string onto the stack */
void push(stack_t *stack, string_t *string) {
	stack_entry_t *entry = malloc(sizeof *entry); 
	entry->string = copyString(string);
	entry->next = stack->head;
	entry->place = 0;
	stack->head = entry;
}

/* Get value of top stack entry */
stack_entry_t *top(stack_t *theStack) {
	if (theStack && theStack->head) {
		return theStack->head;
	}
	else {
		return NULL;
	}
}

/* Pop off top stack entry from stack */
void pop(stack_t *stack) {
	if (stack->head != NULL) {
		stack_entry_t *tmp = stack->head;
		stack->head = stack->head->next;
		destroyString(tmp->string);
		free(tmp);
	}
}

/* Destroy a stack */
void destroyStack(stack_t *theStack) {
	while (theStack->head != NULL) {
		pop(theStack);
	}
	free(theStack);
}