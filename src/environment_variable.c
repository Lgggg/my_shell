#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "environment_variable.h"


int process_environment_variable(char** arglist, int* ret_of_process) {	//处理set、name=value、export等命令
	if (strcmp(arglist[0], "set") == 0) {
		show_environment_variable();
		*ret_of_process = 0;
		return 1;
	}
	else if (strcmp(arglist[0], "unset") == 0) {
		//code for unset
		if (arglist[1] != NULL) {
			delete_var(arglist[1]);
		}
		*ret_of_process = 0;
		return 1;
	}
	else if (strchr(arglist[0], '=') != NULL) {
		*ret_of_process = 0;
		return 1;
	}
	else if (strcmp(arglist[0], "export") == 0) {
		if (arglist[1] != NULL && legal_name(arglist[1])) {
			*ret_of_process = export_environment_variable(arglist[1]);
		}
		else {
			*ret_of_process = 1;
		}
		return 1;
	}
	return 0;
}


int assign(char* cmd) {					//提取 name=value 命令的name和value, 再利用add_envionment_variable加入到tab中
	char* cp;
	int ret;
	cp = strchr(cmd, '=');
	*cp = '\0';
	if (legal_name(cmd)) {
		ret = add_environment_variable(cmd, cp + 1);
	}
	else {
		ret = 1;
	}
	*cp = '=';
	return ret;
}		


int legal_name(char* name) {			//判断name是否合法
	char* cp = name;
	if (isdigit(*cp) && cp == name) return 0;
	for (cp = name; *cp; cp++) {
		if (!isalnum(*cp) || *cp == '_') {
			return 0;
		}
	}
	return (cp != name);
}


int add_environment_variable(char* name, char* value) {		//添加或者修改变量
	var* temp = NULL;
	char* str = NULL;
	int ret = 1;		//not ok

	if((str = new_string(name, value)) != NULL) {
		if ((temp = find_item(name)) != NULL) {
			if (temp->str != NULL) {
				free(temp->str);
			}
			temp->str = str;
			ret = 0;			//ok
		}
		else {
			var* new_var = (var*)malloc(sizeof(var));
			if (new_var != NULL) {
				new_var->str = str;
				new_var->global = 0;
				new_var->next = NULL;
				insert_var(new_var);
				ret = 0;
			}
		}
	}

	return ret;
}

static var* find_item(char* name) {			//查找变量
	var* current = tab.head;
	int len = strlen(name);
	char* str;
	
	while (current != NULL) {
		if (current->str != NULL) {
			str = current->str;
			if (strncmp(str, name, len) == 0 && str[len] == '=') {
				return current;
			}
		}
		current = current->next;
	}
	return NULL;
}


char* new_string(char* name, char* val) {			//为name=value这一字符串分配空间
	char* ret;
	ret = (char*)malloc(strlen(name) + strlen(val) + 2);
	if (ret != NULL) {
		sprintf(ret, "%s=%s", name, val);
	}
	else {
		perror("new_string error");
	}
	return ret;
}


void insert_var(var* value) {						//将var节点插入tab
	if (tab.head == NULL) {
		tab.head = tab.tail = value;
	}
	else {
		tab.tail->next = value;
		tab.tail = value;
	}
}

void delete_var(char* name) {
	var* pre = tab.head;
	var* target = find_item(name);
	if (target != NULL) {
		if (target == tab.head) {
			tab.head = target->next;
			if (tab.head == tab.tail) {
				tab.tail = NULL;
			}
		}
		else {
			while (pre->next != target) {
				pre = pre->next;
			}
			pre->next = target->next;
		}
		free(target->str);
		free(target);
	}
}
	

		
	


char* find_environment_variable(char* name) {		//给定name, 返回value或者""
	var* itemp;
	if((itemp = find_item(name)) != NULL) {
		return itemp->str + 1 + strlen(name);
	}
	return "";
}


int export_environment_variable(char* name) {		//将存在的变量变为global,不存在则添加,值为""
	var* itemp;
	int ret = 1;
	if ((itemp = find_item(name)) != NULL) {
		itemp->global = 1;
		ret = 0;
	}
	else if (add_environment_variable(name, "") == 1) {
		ret = export_environment_variable(name);
	}
	return ret;
}
	
	
void show_environment_variable() {					//打印table
	var* current = tab.head;
	while (current != NULL) {
		if (current->global) {
			printf("# %s\n", current->str);	
		}
		else {
			printf("%s\n", current->str);
		}
		current = current->next;
	}
}
		

int environ2table(char** env) {				//把envrion记入table
	int i;
	char* newstring;
	var* new_var;
	for (i = 0; env[i] != NULL; i++) {
		newstring = malloc(1 + strlen(env[i]));
		if (newstring == NULL) {
			return 0;
		}
		strcpy(newstring, env[i]);
		new_var = (var*)malloc(sizeof(var));
		new_var->str = newstring;
		new_var->global = 1;
		insert_var(new_var);
	}
	return 1;
}

char** table2environ() {					//统计table中的全局变量数，新malloc一个env，把全局变量拷贝进去，返回新的env表
	int i, n = 0;
	char** env;
	var* current = tab.head;

	while (current != NULL && current->str != NULL) {
		if (current->global == 1) {
			++n;
		}
		current = current->next;
	}
	
	env = (char**)malloc((n + 1) * sizeof(char* ));
	if (env == NULL) {
		return NULL;
	}

	current = tab.head;
	i = 0;
	while (current != NULL && current->str != NULL) {
		if (current->global == 1) {
			env[i++] = current->str;
		}
		current = current->next;
	}
	env[i] = NULL;
	return env;
}
	
int process_read(char** arglist, int* ret_of_process) {
	char value[128];
	char buf[256];
	if (strcmp(arglist[0], "read") != 0) {
		return 0;
	}
	
	if (arglist[1] != NULL) {
		if (!legal_name(arglist[1])) {
			printf("illegal name\n");
			*ret_of_process = 1;
		}
		else {
			fgets(value, 256, stdin);
			value[strlen(value) - 1] = '\0';
			sprintf(buf, "%s=%s", arglist[1], value);
			*ret_of_process = assign(buf);
		}
	}
	else {
		*ret_of_process = 1;
		printf("Error format! Usage: read name\n");
	}
	return 1;
}

