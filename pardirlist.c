#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<dirent.h>
#include<pthread.h>

#define BUFFER_SIZE 2024

int keyword_search(char *file_name, char *keyword);
int test_str(char *str, char *key);
//void *runner(void *param);

//id is number on the level,(level determined on the fly)
//need keyword number
struct Node {
	char *path;
	int id;
	int keyword_num;
	struct Node *next;
	struct Node *prev;
};
struct List {
	struct Node *head;
	struct Node *tail;
};
struct Node *create_node(char *path, int id, int keyword_num){
	struct Node *node = (struct Node*)malloc(sizeof(struct Node));
	if (node == NULL) {
		printf("ERROR!");
		exit(-1);
	}

	node->path = strdup(path);
	node->id = id;
	node->keyword_num = keyword_num;
	node->next = NULL;
	node->prev = NULL;
	return node;
}
struct List *create_list() {
	struct List *list = (struct List*)malloc(sizeof(struct List));

	if(list == NULL) {
		printf("Couldnt create list, %s", strerror (errno));
		exit(-1);
	}

	list->head = NULL;
	list->tail = NULL;
	return list;
}
void destroy_list(struct List *list) {
	struct Node *ptr = list->head;
	struct Node *tmp;
	while(ptr != NULL) {
		free(ptr->path);
		tmp = ptr;
		ptr = ptr->next;
		free(tmp);
	}

	free(list);
}
void print(struct List *list) {
	struct Node *tmp = list->head;
	while(tmp != NULL) {
		printf("%d:%d:%s\n", tmp->id, tmp->keyword_num, tmp->path);
		tmp = tmp->next;
	}
}
int levels_deep(char *path) {
	int count = 0;
	int i;
	for(i = 0; i < strlen(path); i++){
		if(path[i] == '/' || path[i] == '\\') {
			count++;
		}
	}
	count++;
	//needed since dirs start with one extra in linux, might not work w Window
	return count;
}
void insert_by_level(char *path, int id, struct List *list, int keyword_num) {	
	struct Node *tmp = list->head;
	struct Node *new_node = create_node(path, id, keyword_num);

	if(list->head == NULL && list->tail == NULL) {
		list->head = new_node;
		list->tail = new_node;
		return;
	}

	//only one in list
	if(list->head == list->tail){
		if(strcmp(tmp->path,new_node->path) < 0){
			list->tail = new_node;
			new_node->prev = tmp;
			tmp->next = new_node;
			return;
		} else {
			list->head = new_node;
			new_node->next = tmp;
			tmp->prev = new_node;
			return;
		}
	}

	//case, two
	if(tmp->next->next == NULL) {

		if(tmp->id < new_node->id){
			tmp = tmp->next;
		}
		
		//might need tmp->id < new_node->id
		//need to check if sam level, if not.
		//strcmp(tmp->path,new_node->path)...
		if(strcmp(tmp->path,new_node->path) < 0){
			list->tail = new_node;
			new_node->prev = tmp;
			tmp->next = new_node;
			return;
		} else {
			tmp->prev->next = new_node;
			new_node->prev = tmp->prev;
			tmp->prev = new_node;
			new_node->next = tmp;
			return;
		}
	}

	while(tmp->id < new_node->id && tmp->next != NULL) {
		tmp = tmp->next;
	}

	while(tmp->id == new_node->id && tmp != list->tail) {
		if(strcmp(tmp->path,new_node->path) > 0) {
			new_node->prev = tmp->prev;
			new_node->next = tmp;
			new_node->prev->next = new_node;
			tmp->prev = new_node;
			return;
		}
		tmp = tmp->next;
	}

	//tmp is tail
	if(tmp == list->tail) {
		if(tmp->id > new_node->id || strcmp(tmp->path,new_node->path) > 1) {
			new_node->prev = tmp->prev;
			new_node->next = tmp;
			new_node->prev->next = new_node;
			tmp->prev = new_node;
			return;
		} else {
			list->tail = new_node;
			new_node->prev = tmp;
			tmp->next = new_node;
			return;
		}
	} 
	//tmp is not tail, is greater than
	else {
		new_node->prev = tmp->prev;
		new_node->next = tmp;
		new_node->prev->next = new_node;
		new_node->next->prev = new_node;
		return;
	}
	return;
}
void recurse(char *basepath, int level, struct List *list, int basep_length, char *keyword, int ispar) {
	
	char path[1000];
	struct dirent *dp;
	char new_node[1000];
	//char full_path[1000];
	DIR *dir = opendir(basepath);
	if(!dir)
		return;
	while ((dp = readdir(dir)) != NULL)
	{
		if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..") != 0)
		{
			int p_len;
			char *test_string = malloc(300 * sizeof(char));
			int keyword_num = 0;
			strcpy(test_string, dp->d_name);
			p_len = levels_deep(basepath) - basep_length;
			strcpy(new_node, basepath);
			strcat(new_node,"/");
			strcat(new_node,dp->d_name);
			if(test_string[0] != '.') {
				//dp->d_name is file name
				//current problem with keyword_search
				//thank fux, just need ed to close file pointer
				if(ispar == 0)
					keyword_num = keyword_search(new_node, keyword);
				else
					keyword_num = 0;
				insert_by_level(new_node, p_len, list, keyword_num);
			}
			strcpy(path, basepath);
			strcat(path, "/");
			strcat(path, dp->d_name);
			free(test_string);
			recurse(path, level, list, basep_length, keyword, ispar);
		}
		level++;
	}
	closedir(dir);
}
void print_to_file(struct List *list, char *output_name) {
	FILE *fp;
	struct Node *tmp = list->head;
	int count = 0;
	int current_level = 0;
	fp = fopen(output_name,"w+");
	while(tmp != NULL) {
		if(current_level != tmp->id){
			count = 0;
			current_level = tmp->id;
		}
		count++;
		fprintf(fp, "%d:%d:%d:%s\n", tmp->id, count, tmp->keyword_num, tmp->path);
		tmp = tmp->next;
	}
	fclose(fp);
	return;
}

//Take file name, and keyword. Search file,
//return number of times keyword occurs, separated by tabs/spaces
//calls test_str
int keyword_search(char *file_name, char *keyword) {
	
	//printf("%s\n", file_name);
	int count = 0;
	FILE *fp;
	char str[BUFFER_SIZE];
	fp = fopen(file_name, "r");
	
	//char *pos;
	//int index;

	//int ch, len;

	//len = strlen(keyword);
	/*	
	while ((fgets(str, BUFFER_SIZE, fp)) != NULL){

	}
	*/
	/*

	for(;;){
		int i;
		if(EOF==(ch=fgetc(fp))) break;
		if((char)ch != *keyword) continue;

		for(i = 1; i < len; ++i){
			if(EOF==(ch = fgetc(fp)))
				goto end;
			if((char)ch != keyword[i]){
				fseek(fp, 1-i, SEEK_CUR);
				goto next;
			}
		}
		++count;
		next: ;
	}

	end:
	*/

	/*
	while ((fgets(str, BUFFER_SIZE, fp)) != NULL) {

		index = 0;
		
		while ((pos = strstr(str + index, keyword)) != NULL){
			index = (pos - str) + 1;
			count++;
		}

	}
	*/
	
	while( fgets(str, BUFFER_SIZE, fp) != NULL) {
		count += test_str(str, keyword);
	}
	
	fclose(fp);
	return count;
}

//take string, and keyword. Return number of instances of keyword
//separated by spaces and tabs
int test_str(char *str, char *key) {

	const char delim[] = " \t\n";
	char *rest = str;
	int count = 0;
	str = strtok_r(str,delim, &rest);

	while(str!=NULL){
		if(strcmp(key, str) == 0)
			count++;
		str = strtok_r(NULL, delim, &rest);
	}
	return count;
}

struct Thread_data {
	//int count;
	char *key;
	struct Node *node;
};

struct Thread_data *create_thread_data(/*int count, */char *key, struct Node *node){
	struct Thread_data *thread_data = (struct Thread_data *)malloc(sizeof(struct Thread_data));
	//thread_data->count = count;
	thread_data->key = key;
	thread_data->node = node;
	return thread_data;
};

void destroy_thread_data(struct Thread_data *thread_data){
	free(thread_data->key);
	free(thread_data);
}

//getting struct, with keyword and node, can THEN search.
void *runner(void *args) {
	pthread_detach(pthread_self());
	int count = 0;
	struct Thread_data *thread_data = (struct Thread_data*)args;
	count = keyword_search(thread_data->node->path, thread_data->key);
	thread_data->node->keyword_num = count;	
	
	free(thread_data);
	pthread_exit(0);
}

//need pthread, number count
//1 is path, 2 is keyword, 3 is output file, 4 is multithread
int main(int argc, char *argv[]) {

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	char path[300];
	char output_name[100];
	int basepath_depth;
	int ispar = atoi(argv[4]);
	char keyword[100];
	struct List *list = create_list();
	strcpy(path, argv[1]);
	strcpy(output_name, argv[3]);
	strcpy(keyword, argv[2]);
	//-1 for linux, +0 for windows
	basepath_depth = levels_deep(path) -1;
	insert_by_level(path,1,list, 0);

	recurse(path, 0, list, basepath_depth-1, keyword, ispar);

	pthread_t tid;	

	if(ispar == 1){
		struct Node *tmp = list->head;
		//pthread_t tid;
		while(tmp != NULL){
			//pthread_t tid;
			struct Thread_data *thread_data = create_thread_data(keyword, tmp);
			pthread_create(&tid, NULL, runner, (void *)thread_data);
			tmp = tmp->next;
			//remove for no leaks, but slow
			pthread_join(tid, NULL);
		}
		pthread_join(tid, NULL);
		//print_to_file(list, output_name);
		//destroy_list(list);
		//pthread_exit(NULL);
		
	}

	//pthread_join(tid, NULL);
	print_to_file(list, output_name);
	destroy_list(list);
	//pthread_exit(NULL);
	return 0;
}
