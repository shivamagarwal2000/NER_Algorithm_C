/* Algorithms are fun!
 * Program implementing NER algorithm using dictionary
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/* Constant definitions */

#define MAX_SIZE      30  /* max size of the names in array */
#define CASES_NO      3   /* size of probability array */
#define CH_ZERO      '0'  /* character 0 */
#define NULL_CH      '\0' /* null character */
#define HASH         '#'  /* hash character */
#define NEW_LINE     '\n' /* new line */
#define MAX_ENTRIES  100  /* max no of entries in structure array */
#define PERC         '%'  /* percentage sign */
#define SPACE        ' '  /* space character */
#define BS_NOT_FOUND (-1) /* to be used in binary search */
#define BS_FOUND      0

/* type definitions */

typedef struct node node_t;

typedef char data_t[MAX_SIZE+1];   /* to store word as string */

typedef struct{					   /* to store names and */ 
	char name[MAX_SIZE+1];		   /* probababilities */
	int prob[CASES_NO];
}sample_data_t;

struct node{
	data_t data;
	node_t *next;
};

typedef struct{
	node_t *head;
	node_t *foot;
} list_t;

/* function prototypes */
int get_values(sample_data_t dict[], int counter[]);
int input_names(sample_data_t *input);
void putzero(int arr[]);
list_t *make_empty_list(void);
int getword(char W[], int limit);
list_t *insert_at_foot(list_t *list, data_t value);
void print_first_element(sample_data_t A);
void print_stats(int counter[], int no_of_ent);
void print_list(list_t *list);
void find_word(list_t *list, sample_data_t dict[], int hi);
int binary_search(sample_data_t A[], int lo, int hi, 
		data_t key, int *locn);
int cmp(data_t s1, data_t s2);
void print_label(data_t str, int code);
void free_list(list_t *list);


int
main(int argc, char *argv[]){
	sample_data_t dict[MAX_ENTRIES]; /* structure array of the dictionary */

	int counter[MAX_ENTRIES];        /* array to store length of each word */

	int no_of_ent;				     /* to store the no. of entries in dict */

	int count = 0;					 /* to count EOF as guard of loop */

	char ch;						 /* to store each character of word */

	data_t word; 					 /* to store the individual words */

	no_of_ent = get_values(dict, counter); /* to fill dict */

	list_t *list;					       /* to hold the sentence as list */

	list = make_empty_list();

	while((ch = getchar()) != NEW_LINE);   /* to seperate the list and dict */

	while(count != EOF){
		count = getword(word, MAX_SIZE);   /* to input the word */
		if(count == EOF)
			break;
		list = insert_at_foot(list, word); /* to put the word at the end of list */
	}
	print_first_element(dict[0]);          /* to print first element in dict */

	print_stats(counter, no_of_ent);	   /* to print average and no. of names */

	print_list(list);				       /* to print names in list */

	find_word(list, dict, no_of_ent);      /* to search names in dictionary and print label */

	free_list(list);                       /* to free out the list */

	list = NULL;

	return 0;
}

/* read the dictionary */
int
get_values(sample_data_t dict[], int counter[]){

	char txt;
	int i = 0;

	while((txt = getchar())!= PERC){
		if(txt == HASH){
			counter[i] = input_names(&dict[i]); /* to store no of characters in word and fill the dic */
			i++;
		}
	}
	return i;                                   /* returns the no of entries in dict */
}

/* to actually input the values into the dict */
int
input_names(sample_data_t *input){

	char txt;
	int i = 0, j = 0;                                 
	int flag = 0;                              /* stores the number of newlines */

	putzero(input->prob);                      /* to initialize the probability array as 0 */

	while(flag == 0){						   /* to run the loop till newline */
		txt = getchar();
		if(txt == NEW_LINE){				   /* to store probabilities if new line is encountered */
			flag++;
			while((txt = getchar()) != NEW_LINE){
				if(txt == SPACE){			   /* each space marks the end of one probability */
					txt = getchar();		   /* to omit the space and move one character */
					i++;
				}
				input->prob[i] = (input->prob[i] * 10) + (txt - CH_ZERO); /* to convert char to int and store each number */
			}
		}
		else{
			input->name[j] = txt;              /* to simply put the character into the dictionary untill new line */ 
			j++;
		}
		input->name[j] = NULL_CH;			   /* to put null character at the end of each name */
	}
	return j;
}

/* to initialize array to 0 */
void
putzero(int arr[]){

	int i;
	for(i = 0; i < CASES_NO; i++){
		arr[i] = 0;
	}
}

/* code written from listops.c from the lecture slides */
/* creates an empty list */
list_t
*make_empty_list(void){

	list_t *list;
	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;
	return list;
}

/* code written from listops.c from the lecture slides */
/* extracts each word individually from sentence */
int
getword(char W[], int limit){

	int c, len = 0;
	/* first, skip over any non alphabetics */
	while((c = getchar()) != EOF && !isalpha(c)){
		/* do nothing more */
	}
	if(c == EOF){
		return EOF;
	}
	/* ok, first character of next word has been found */
	W[len++] = c;
	while(len < limit && (c = getchar()) != EOF && isalpha(c)){
		/* another character to be stored */
		W[len++] = c;
	}
	/* now close off the string */
	W[len] = NULL_CH;
	return 0; 
}

/* code written from listops.c from the lecture slides */
/* inserts the name at the last part of the list */ 
list_t
*insert_at_foot(list_t *list, data_t value){

	node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
	strcpy(new->data, value);
	new->next = NULL;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		list->foot->next = new;
		list->foot = new;
	}
	return list;
}

/* prints the first value in the dictionary */
void
print_first_element(sample_data_t A){

	printf("=========================Stage 1=========================\n");
	printf("Name 0: %s\n",A.name);
	printf("Label probabilities: %d%% %d%% %d%%\n",A.prob[0],A.prob[1],A.prob[2]);
	printf("\n");
}

/* prints the average and no of names */
void
print_stats(int counter[], int no_of_ent){

	int i;
	double sum = 0.0, avg = 0.0;
	printf("=========================Stage 2=========================\n");

	for(i = 0; i < no_of_ent; i++){                /* to calculate the sum of lengths of words */
		sum += counter[i];
	}
	avg = sum/no_of_ent;

	printf("Number of names: %d\n", no_of_ent);
	printf("Average number of characters per name: %.2lf\n", avg);
	printf("\n");
}

/* printing of individual words in new lines */
void
print_list(list_t *list){

	printf("=========================Stage 3=========================\n");

	node_t *temp;                                 /* to declare temporary variable to print values */

	assert(list->head != NULL && list != NULL);   /* to check the list is non empty */

	temp = list->head;                            /* to initialize the temporary variable to first element of list */

	while(temp != NULL){
		printf("%s\n", temp->data);
		temp = temp->next;
		}
	printf("\n");
}

/* implements binary search and prints the corresponding labels */
void
find_word(list_t *list, sample_data_t dict[], int hi){

	node_t *temp;
	int locn;
	assert(list->head != NULL && list != NULL);
	temp = list->head;

	printf("=========================Stage 4=========================\n");

	/* passes word and dictionary to binary search function and returns status */
	while(temp != NULL){
		if(binary_search(dict, 0, hi, (temp->data), &locn) == BS_NOT_FOUND){
			print_label(temp->data, 0);          /* print the label and word */
		}
		else{
			if(dict[locn].prob[1] == 0){
				print_label(temp->data, 1);
			}
			else if(dict[locn].prob[0] == 0){
				print_label(temp->data, 2);
			}
			else{
				print_label(temp->data, 3);
			}
		}
		temp = temp->next;
	}
	printf("\n");
}

/* code written and modified from binarysearch.c used in lecture slides */ 
int
binary_search(sample_data_t A[], int lo, int hi, 
		data_t key, int *locn) {

	int mid, outcome;
	/* if key is in A, it is between A[lo] and A[hi-1] */
	if (lo >= hi) {
		return BS_NOT_FOUND;
	}
	mid = (lo + hi) / 2;
	if ((outcome = cmp(key, A[mid].name)) < 0) {
		return binary_search(A, lo, mid, key, locn);
	} else if (outcome > 0) {
		return binary_search(A, mid+1, hi, key, locn);
	} else {
		*locn = mid;
		return BS_FOUND;
	}
}

/* code written and modified from binarysearch.c used in lecture slides */
/* compares two strings and returns dictionary difference of strings */
int
cmp(data_t s1, data_t s2) {

	return (strcmp(s1,s2));
}

/* to recieve the word and code the print the label corresponding to codes */
void
print_label(data_t str, int code){

	if(code == 0){
		printf("%-30s  NOT_NAME", str);
	}
	else if(code == 1){
		printf("%-30s  FIRST_NAME", str);
	}
	else if(code == 2){
		printf("%-30s  LAST_NAME", str);
	}
	else{
		printf("%-30s  FIRST_NAME, LAST_NAME", str);
	}
	printf("\n");
}

/* code written from listops.c from the lecture slides */
/* function to free the list */
void
free_list(list_t *list){

	node_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(list);
}