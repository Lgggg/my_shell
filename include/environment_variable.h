#ifndef __ENVIRONMENT_VARIABLE_H
#define __ENVIRONMENT_VARIABLE_H




typedef struct table {
	struct var* head;
	struct var* tail;
}table; 

typedef struct var {
	char* str;
	int global;
	struct var* next;
}var;

static table tab;
extern char** environ;


int process_environment_variable(char** , int*);
int assign(char* );
int legal_name(char* );


int add_environment_variable(char* , char* );
static var* find_item(char*);
char* new_string(char* name, char* val );
char* find_environment_variable(char* );
void show_environment_variable();
int export_envionment_variable();

void insert_var(var*);
void delete_var(char*);

int environ2table(char** );
char** table2environ();

int process_read(char** , int* );

#endif
