#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"
#include <assert.h>

char *readJSONFile() {
	int count;
	char filename[30];
	FILE *fp;

	while(1){
		printf("Input filename: ");
		scanf("%s", filename);
		strcat(fileName, ".json");
		fp = fopen("filename", "r");
		if(fp==NULL){
			printf("File is NULL!n");
		} else {
			break;
		}
	}
	char* JSON_STRING;
	char oneLine[255];
	JSON_STRING = (char*)malloc(255);
	assert(JSON_STRING!=NULL);

	while(1){
		fgets(oneLine, 255, fp);
		if(feof(fp)) break;
		count += strlen(oneLine);
		JSON_STRING = (char*)realloc(JSON_STRING, count+1);
		strcat(JSON_STRING, oneLine);
	}
	fclose(fp);
	return JSON_STRING;
}

void printNameList(char *jsonstr, jsmntok_t *t, int *nameTokIndex){
	int i=0, c=0;

	for(i=0; i<100; i++){
		if(i==0){
			printf("***** Name List *******\n");
		}
		if(i==nameTokIndex[c]){
			printf("[NAME %d] %.*s\n", c+1, t[i].end-t[i].start, jsonstr + t[i].start);
			c++;
		}
	}
	printf("\n");
}

void selectNameList(char *jsonstr, jsmntok_t *t, int *nameTokIndex){
	int selection,i=0;
	while(1){
		printf("select Name's no (exit:0) >> ");
		scanf(" %d", &selection);
		if(selection==0){
			break;
		} else {
			printf("[NAME %d] %.*s\n", selection, t[nameTokIndex[selection-1]].end-t[nameTokIndex[selection-1]].start, jsonstr + t[nameTokIndex[selection-1]].start);
			printf("%.*s\n\n", t[nameTokIndex[selection-1]+1].end-t[nameTokIndex[selection-1]+1].start, jsonstr + t[nameTokIndex[selection-1]+1].start);
		}
	}
}

//added function to solve example8
void example8List(char *jsonstr, jsmntok_t *t, int tokcount, int *nameTokIndex){
	int i=0, e=1;
	printf("***** Object List *******\n");
	for (i = 0; i < tokcount; i++) { //repeats by the number of the token
			//if (jsoneq(jsonstr, &t[i], "name") == 0) { //true if the token is "name"
			if(t[i].type==JSMN_OBJECT && t[i].parent==0){ //토큰의 타입의 스트링이면서 토큰의 parent가 전체이면(depth가 1이다.)
				printf("[Name %d] %.*s\n", e, t[i+2].end-t[i+2].start, jsonstr + t[i+2].start); //print the value
				e++;
			}
		}
	printf("\n");
}

void printtoken(const char *JSON_STRING, jsmntok_t *t, int count){
  int i=0;
  for(i=0; i<count; i++){
  printf("index: [%2d] start&end{%d, %d} size <%d> parent-%d type '%d' %.*s\n",
		i, t[i].start, t[i].end, t[i].size, t[i].parent, t[i].type, t[i].end-t[i].start, JSON_STRING + t[i].start);
	}
}

void jsonNameList(char *jsonstr, jsmntok_t *t, int tokcount, int *nameTokIndex){
	char nameList[100][100];
	int c=0, i=0;
	for(i=0; i<tokcount; i++){
		if(t[i].size==1 && t[i].type == JSMN_STRING){
			strncpy(nameList[c], jsonstr + t[i].start,  t[i].end-t[i].start);
			nameTokIndex[c]=i;
			c++;
		}
	}
	printf("***** Name List *******\n");
	for(i=0; i<c; i++){
		printf("[NAME %d] %s\n", i+1, nameList[i]);
	}
	printf("\n");
}

//added function to solve example9
void example9SelectList(char *jsonstr, jsmntok_t *t, int tokcount, int *nameTokIndex){
	int selection, i=0, e=1;
	int *selectList = (int *)malloc(sizeof(int));
	assert(selectList!=NULL);
	while(1){
		printf("원하는 번호 입력 (Exit:0) : ");
		scanf("%d", &selection);
		if(selection==0) break;
		//selectList = realloc(selectList, (*sizeof(int)));
		while(1){
			if(t[i].parent==0){
				printf("PARENT IS 0\n");
				break;
			}
			printf("%.*s = ", t[i].end-t[i].start, jsonstr + t[i].start);
			printf(" %.*s \n", t[i+1].end-t[i+1].start, jsonstr + t[i+1].start);
			i+=2;
		}
		printf("\n\n");
	}
	free(selectList);
}

int main() {
	int i, r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */
	int *nameTokIndex = (int*)malloc(sizeof(int));

	char *JSON_STRING = readJSONFile();

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));

	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	//printtoken(JSON_STRING, t, r);
	//jsonNameList(JSON_STRING, t, r, nameTokIndex);
	//printNameList(JSON_STRING, t, nameTokIndex);
	//selectNameList(JSON_STRING, t, nameTokIndex);
	example8List(JSON_STRING, t, r, nameTokIndex);
	example9SelectList(JSON_STRING, t, r, nameTokIndex);

	return EXIT_SUCCESS;
}
