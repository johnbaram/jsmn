#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"
#include <assert.h>

typedef enum {
   NONGSHIM = 1,
   PALDO = 2,
   ODDUGI =3,
   SAMYANG = 4
}comp_t;

typedef struct{
   comp_t comp;
   char name[10];
   int price;
   int count;
}product_t;

char *readJsonFile(FILE *fp){
  int count;
	char filename[30];
	//FILE *fp;

  /*while(1){
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
  */
  fp = fopen("data3.json", "r");
  if(fp==NULL) printf("FILE IS NULL\n");

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

void jsonList(const char *jsonstr, jsmntok_t *t, int tokcount, int *List){
   int count=0, i=0;
   if(t[1].type==JSMN_STRING&&t[2].type==JSMN_ARRAY) i=3;
   for(; i<tokcount; i++){
      if(t[i].size>0&&t[i].type==JSMN_STRING){
         List=(int*)realloc(List, sizeof(int)*(count+2));
         List[count]=i;
         count++;
         if(t[i+1].type==JSMN_ARRAY||t[i+1].type==JSMN_OBJECT){
            int skip=1, j=i+1;
            if(t[j+1].type==JSMN_OBJECT) skip++;
            while(t[j].size!=0){
               int mult=t[j].size==1? 2: t[j].size;
               skip*=mult;
               j++;
            }
            i+=skip;
         }
      }
   }
   List[count]=0;
}

int jsonObjectList(const char *jsonstr, jsmntok_t *t, int tokcount, int *objectList){
   int count=0, i;
   for(i=0; i<tokcount; i++){
      if(t[i].type==JSMN_OBJECT && t[t[i].parent].type==JSMN_ARRAY){
         objectList=(int*)realloc(objectList, sizeof(int)*(count+2));
         objectList[count]=i;
         count++;
      }
   }
   objectList[count]=0;
   return count;
}

void searchByName(const char *JSON_STRING, jsmntok_t *t, int *List, int objNum, int triggerEnd, char *tag, char *target){
   int i=0;
   while(List[i]<objNum) i++;
   while(List[i]<triggerEnd){
      if(strncmp(tag, JSON_STRING+t[List[i]].start, t[List[i]].end-t[List[i]].start)==0) break;
      i++;
   }
   int lengthh= t[List[i]+1].end-t[List[i]+1].start;
   strncpy(target, JSON_STRING+t[List[i]+1].start, lengthh);
   target[lengthh]='\0';
}

product_t* makeProduct(const char *JSON_STRING, jsmntok_t *t, int *List, int objNum, int triggerEnd, product_t *prod){
   char price[100], count[100], comp[100];
   prod=(product_t*)malloc(sizeof(product_t));
   assert(prod!=NULL);

   searchByName(JSON_STRING, t, List, objNum, triggerEnd, "company", comp);
   if(strcmp("농심", comp)==0) prod->comp = NONGSHIM;
   else if(strcmp("팔도", comp)==0) prod->comp = PALDO;
   else if(strcmp("오뚜기", comp)==0) prod->comp = ODDUGI;
   else if(strcmp("삼양", comp)==0) prod->comp = SAMYANG;

   searchByName(JSON_STRING, t, List, objNum, triggerEnd, "price", price);
   prod->price=atoi(price);

   searchByName(JSON_STRING, t, List, objNum, triggerEnd, "count", count);
   prod->count=atoi(count);

   searchByName(JSON_STRING, t, List, objNum, triggerEnd, "name", prod->name);

   return prod;
}

void change(int enumNo, char *compName){
  if(enumNo==NONGSHIM) strcpy(compName, "NNGSHM");
  else if(enumNo==PALDO) strcpy(compName, "PALDO");
  else if(enumNo==ODDUGI) strcpy(compName, "ODDUGI");
  else if(enumNo==SAMYANG) strcpy(compName, "SAMYANG");
}

void printProduct(product_t *prod[], int pcount){
   int i;
   char compName[30];
   printf("*************************************************\n");
   printf("번호\t제품명\t제조사\t가격\t개수\t총가격\n");
   printf("*************************************************\n");
   for( i =0; i<pcount; i++){
     //printf("%s  ", compName);
     change(prod[i]->comp, compName);
     printf("%d\t%s\t\t%s\t%d\t%d\t%d\n", i+1, prod[i]->name, compName, prod[i]->price, prod[i]->count, prod[i]->count*prod[i]->price);
   }
}

void prodmain(const char *JSON_STRING, jsmntok_t *t, int *List, product_t *prod[], int tokcount){
   int i;
   int *objectList=(int*)malloc(sizeof(int));
   int objLength=jsonObjectList(JSON_STRING, t, tokcount, objectList);
   for(i =0; i<objLength; i++){
      int triggerEnd= objectList[i+1]==0? tokcount: objectList[i+1];
      prod[i]=makeProduct(JSON_STRING, t, List, objectList[i], triggerEnd, prod[i]);
   }
   printProduct(prod, objLength);
}

void selectMenu(const char *JSON_STRING, jsmntok_t *t, int *List, int *fieldList, int Length, int tokcount, product_t *prod[], int objLength){
   int num=0,i=0;
   do{
		 	printf("select 1 to read\n");
		 	printf("select 2 to print\n");
		 	printf("select 3 to edit\n");
		 	printf("select 4 to write\n");
      printf("Select Index Number (0 to exit): ");
      int tester=scanf("%d", &num);
      if(tester==0||tester==EOF){
         printf("pls input number\n");
         char stringerror[20];
         scanf("%s", stringerror);
      } else if(num==0) ;
      else if(Length<=num||num<0){
				printf("wrong index number\n");
      }
   }while(num!=0);
}

int main() {
   int i,r;
   jsmn_parser p;
   jsmntok_t t[128];
   product_t *prod[20];
   FILE *fp;

   int *List=(int*)malloc(sizeof(int));

   const char *JSON_STRING = readJsonFile(fp);

   jsmn_init(&p);
   r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
   if (r < 0) {
      printf("Failed to parse JSON: %d\n", r);
      return 1;
   }

   jsonList(JSON_STRING, t, r, List);

   prodmain(JSON_STRING, t, List, prod, r);

   return EXIT_SUCCESS;
}
