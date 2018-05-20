#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"
#include <assert.h>
#include <stdbool.h>

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */


static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

char *readJSONFile() {
	int count;
	FILE *fp = fopen("data.json", "r");
	assert(fp!=NULL);
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
	//printf("%s", JSON_STRING);
	fclose(fp);
	return JSON_STRING;
}

void jsonNameList(char *jsonstr, jsmntok_t *t, int tokcount, int *nameTokIndex){
	char nameList[100][100];
	int c=0, i=0;
	for(i=0; i<tokcount; i++){
		if(t[i].size>0 && t[i].type==JSMN_STRING){
			strncpy(nameList[c], jsonstr + t[i].start,  t[i].end-t[i].start);
			nameTokIndex[c]=i;
			c++;
		}
	}
	/*for(i=0; i<c; i++){
		if(i==0){
			printf("***** Name List *******\n");
		}
		printf("[NAME %d] %s\n", i+1, nameList[i]);
	}
	printf("\n");*/
}

void printNameList(char *jsonstr, jsmntok_t *t, int *nameTokIndex){
	int i=0, c=0;
	/*
	for(i=0; i<100; i++){
		if(i==0){
			printf("***** Name List *******\n");
		}
		if(i==nameTokIndex[c]){
			printf("[NAME %d] %.*s\n", c+1, t[i].end-t[i].start, jsonstr + t[i].start);
			c++;
		}
	}
	printf("\n");*/
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

int main() {
	int i;
	int r;
	int nameTokIndex[100];
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	char *JSON_STRING = readJSONFile();

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));

	jsonNameList(JSON_STRING, t, r, nameTokIndex);
	printNameList(JSON_STRING, t, nameTokIndex);
	selectNameList(JSON_STRING, t, nameTokIndex);

	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}

	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STRING, &t[i], "name") == 0) {
			/* We may use strndup() to fetch string value */
			printf("- name: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "keywords") == 0) {
			/* We may additionally check if the value is either "true" or "false" */
			printf("- keywords: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "description") == 0) {
			/* We may want to do strtol() here to get numeric value */
			printf("- UID: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "examples") == 0) {
			int j;
			printf("- examples:\n");
			if (t[i+1].type != JSMN_ARRAY) {
				continue; /* We expect groups to be an array of strings */
			}
			for (j = 0; j < t[i+1].size; j++) {
				jsmntok_t *g = &t[i+j+2];
				printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
			}
			i += t[i+1].size + 1;
		} /*else {
			printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
					JSON_STRING + t[i].start);
		}*/
	}
	return EXIT_SUCCESS;
}
