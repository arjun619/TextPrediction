#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "TriePrediction.h"


//Creates/inserts words to my trie
TrieNode *createNode() {
	int i;
	TrieNode *new = malloc(sizeof(TrieNode));
	for (i = 0; i < 26; i++) {
		new->children[i] = NULL;
	}
	new->subtrie = NULL;
	new->count = 0;
	//new->subtrie->children[i] = NULL
	//printf("I got here");
	return new;
}

//This function goes to the end of the string and checks if there
//is some punctuation.  If so, it returns a flag.
int checkPunctuation(char *inputString) {
	//char *str;
	int length = strlen(inputString);

	//str  = malloc(sizeof(inputString) + 1);
	//strcpy(str, inputString);

	if(inputString[length-1] == '!') {
		return 1;
	}
	if(inputString[length-1] == '?') {
		return 1;
	}
	if(inputString[length-1] == '.') {
		//printf("GOT A PERIOD\n");
		return 1;
	}
	return 0;

}

//This fucntion uses isalpha() and makes a new string 
//that is ONLY lower case letters. Removes anythng that
//is not a letter
void stripPunctuation(char *inputString) {
	int i, j = 0, length = strlen(inputString);
	char temp[1026];
	//printf("Word... %s", inputString);

	for(i = 0; i < length; i++){
		if(isalpha(inputString[i])) {
			temp[j] = inputString[i];
			temp[j] = (tolower(temp[j]));
			j++;
		}
	}
	//We have to include the null terminator
	temp[j] = '\0';
	strcpy(inputString, temp);
}

//This is the heart of the program, it inserts stuff to the trie.
void insertToTrie(TrieNode *curr, char *input, int index) {
	int position;
	int length = strlen(input);
	char next = input[index];
	char buffer[1026];

	if (index > length-1)
		return;
	//Find out where we need to insert
	position = next - 'a';

	if (curr->children[position] == NULL) {
		curr->children[position] = createNode();
	}

	if (index == length - 1) { 
		curr->children[position]->count++;
	}

	//We move by index + 1 recursively and look through each char
	insertToTrie(curr->children[position], input, index+1);
}

//This fucntion retrives the terminal node of a specifc word.
TrieNode *getNode(TrieNode *root, char *str) {

	int lettervalue;
	if(str[0] == '\0') {
		if (root->count == 0)
			return NULL;
		else
			return root;
	}

	lettervalue = str[0] - 'a';

	//Here we are trying to see if we find our char, if we we move on
	//to the next char
	if (root->children[lettervalue] != NULL)
		return getNode(root->children[lettervalue], str + 1);

	return NULL;

}

//The buildtrie fucntion uses the given file to build a trie using 
//several other helper fucntions.
TrieNode *buildTrie(char *filename) {
	TrieNode *root;
	TrieNode *holder;
	TrieNode *temp = NULL;
	char inputTest[1025];
	char str[1026];
	char *str2;
	char *toFree;
	char buffer[1025];
	char previousWord[1025];

	FILE *file;
	file = fopen(filename, "r");

	root = createNode();
	strcpy(previousWord, "");
	
	while(fscanf(file, "%s", inputTest) != EOF) {

		//We need to make everything lowercase letters and remove
		//other symbols.
		strcpy(str, inputTest);
		stripPunctuation(str);

		//This a flag that checks for punctuation.
		//If it is 1, then we want to make sure it has a different
		//insertion algorithm
		if(checkPunctuation(inputTest) == 1) {

			if(temp == NULL)
				insertToTrie(root, str, 0);
			else {
				insertToTrie(root, str, 0);
				if(temp->subtrie == NULL) 
					temp->subtrie = createNode();
				insertToTrie(temp->subtrie, str, 0);
				temp = NULL;
			}
			//printf("Inserting to subtrie (has punctuation)... %s\n", str);
			//This helps to take care of double punctuation words, where
			//The previous word also has pucntuation.
			if(checkPunctuation(previousWord) == 1) {
				strcpy(previousWord, inputTest);
				continue;
			}

		}
		//If the punctuation flag is not 1 we want to proceed with the normal
		//insertion algorithm
		else {
			//printf("Inserting to main tree (no punctuation)...: %s\n", str);
			insertToTrie(root, str, 0);
			if(temp == NULL) {
				temp = getNode(root, str);
			}

			else {
				//A new subtrie needs to be properly allocated.
				if(temp->subtrie == NULL) {
					temp->subtrie = createNode();
				}

				//printf("Inserting to subtrie... %s\n", str);
				insertToTrie(temp->subtrie, str, 0);
				//We want to grab the terminal node of our current word.
				//So we chave access to it later in the loop.
				temp = getNode(root, str);
			}
		}

		//free(toFree);
		//This is to keep out previous word up to date!
		strcpy(previousWord, inputTest);

	}
	fclose(file);
	return root;
}
//Credit to Professor Szumlamnski for this!
void printTrieHelper(TrieNode *root, char *buffer, int k) {
	int i;

	if (root == NULL)
		return;

	if (root->count > 0)
		printf("%s (%d)\n", buffer, root->count);

	buffer[k + 1] = '\0';

	for (i = 0; i < 26; i++) {
		buffer[k] = 'a' + i;

		printTrieHelper(root->children[i], buffer, k + 1);
	}

	buffer[k] = '\0';
}


//Credit to Professor Szumlamnski for this!
// If printing a subtrie, the second parameter should be 1; otherwise, if
// printing the main trie, the second parameter should be 0.
void printTrie(TrieNode *root, int useSubtrieFormatting) {
	char buffer[1026];

	if (useSubtrieFormatting) {
		strcpy(buffer, "- ");
		printTrieHelper(root, buffer, 2);
	}
	else {
		strcpy(buffer, "");
		printTrieHelper(root, buffer, 0);
	}
}

//So many paramters! Lets go step by step.
void mostFrequentWordSubtrie(TrieNode *root, char *str, int *strCount, char *buffer, int k, int *initialize) {
	int i;

	if (root == NULL)
		return;

	if (root->count > 0) {

		//This sets our base values to comapre, it happens ONCE.
		if(*initialize == 0) {
			strcpy(str, buffer);
			*strCount = root->count;
			//printf("%s %d\n", str, *strCount);
			*initialize = 1;
		}

		//As we get new words we need to compare!
		if(root->count > *strCount) {
			//If the above statement is true, we need to change values.
			*strCount = root->count;
			strcpy(str, buffer);
			//printf("%s %d\n", str, *strCount);
		}
		
	}
	//This all below is taken straight from the Professor's print trie fucntion.
	buffer[k + 1] = '\0';

	for (i = 0; i < 26; i++) {
		buffer[k] = 'a' + i;
		//This is the only thing different.
		mostFrequentWordSubtrie(root->children[i], str, strCount, buffer, k + 1, initialize);
	}
	buffer[k] = '\0';
}
//This function does some basic checks so the above fucntion can work correctly.
void mostFrequentWord(TrieNode *root, char *str) {

	int x;
	int *strCount = NULL;

	int y;
	int *initialize = 0;

	char tester[1026];

	initialize = &y;
	strCount = &x;

	int mostFrequent;
	char buffer[1026];

	//We have to check if what we need is NULL.
	//We don't want to segfault!
	TrieNode *terminalNode = NULL;	
	terminalNode = getNode(root, str);

	//The word is not in our trie
	if(terminalNode == NULL) {
		return;
	}
	//The word is not in the subtrie.
	if(terminalNode->subtrie == NULL)
		return;
	
	//This is set to zero ONCE and is set to one ONCE.
	//It helps in setting base values.
	*initialize = 0;
	//printf("In = %d\n", *initialize);
	mostFrequentWordSubtrie(terminalNode->subtrie, tester, strCount, buffer, 0, initialize);
	strcpy(str, tester);	
	
}

//This is a simple fucntion to read and format everrything from the input.
void readInputFile(char *filename, TrieNode *root) {
	int c;
	int p;
	TrieNode *terminalNode = NULL;	
	char winner[1026];
	char inputTest[1026];
	int i = 0;
	int length;
	char input[1026];
	TrieNode *node = NULL;
	FILE *file;
	file = fopen(filename, "r");

	while(fscanf(file, "%s", inputTest) != EOF) {
		//printf(inputTest, "%s");

		if(strcmp(inputTest, "!") == 0) {
			printTrie(root, 0);
			//printf("%s\n", inputTest);
		}

		//The @ symbol is the only complicated case in this fucntion.
		//This is because it needs to keep track of lots of different
		//nodes as you'll see below.
		else if(strcmp(inputTest, "@") == 0) {
			//printf("TESTING...\n");
			//printf("%s", inputTest);
			fscanf(file, "%s", inputTest);
			fscanf(file, "%d", &i);
			printf("%s", inputTest);
			//printf(" %d\n", i);

			strcpy(input, inputTest);
			stripPunctuation(input);

			//We want to check that the terminal node exists.
			terminalNode = getNode(root, input);
			if(terminalNode == NULL) {
				printf("\n");
				continue;
			}
			//If the node exsists, lets check its most frequent node.
			mostFrequentWord(root, input);
			//Store that word.
			strcpy(winner, input);
			printf(" %s", winner);

			if(i > 1) {
				//This for loop is the heart of the text prediction tool!
				for(p = 1; p < i; p++) {
					//We grab the terminal node, make sure it has a subtrie
					//And then figure out the most frequent for the winner of
					//the previous call.
					terminalNode = getNode(root, winner);
					if(terminalNode->subtrie == NULL) {
						break;
					}
					mostFrequentWord(root, winner);
					printf(" %s", winner);
					//printf("YOOOOO");
				}
			}

			printf("\n");
			//free(winner);
		}
		//This is for the case where the input is just a string.
		//So we need to check if it is in the trie and subtrie
		else {

			printf("%s\n", inputTest);
			length = strlen(inputTest);


			for(i = 0; i < length; i++) {
				inputTest[i] = tolower(inputTest[i]);
			}


			//Below we check if the word is on our trie
			//And if the subtrie has soemthing in it,
			if(getNode(root, inputTest) == NULL) {
				printf("(INVALID STRING)\n");
				continue;
			}
			node = getNode(root, inputTest);
			if(node->subtrie == NULL) {
				printf("(EMPTY)\n");
				continue;
			}
			printTrie(node->subtrie, 1);
		}

	}

	fclose(file);
}

//Simple destory trie fucntion that goes through each child, and each
//subtrie destroying so many families :(
TrieNode *destroyTrie(TrieNode *root) {
	int i;

	for(i = 0; i < 26; i++) 
		if (root->children[i] != NULL) 
			destroyTrie(root->children[i]);

	if (root->subtrie != NULL)
		destroyTrie(root->subtrie);
	
	free(root);
	return NULL;
}


double difficultyRating(void) {
	return 4;
}

double hoursSpent(void) {
	return 20;
}
int main(int argc, char **argv) {
	char *buffer;
	TrieNode *root = NULL;
	TrieNode *test = NULL;
	root = buildTrie(argv[1]);
	//buffer = theStripper("Lol,");
	//printf("Printing...\n");
	//printTrie(root, 0);
	//printf("Printing subtrie\n");
	readInputFile(argv[2], root);
	root = destroyTrie(root);
	free(root);
	//test = getNoderoot, "pie");
	//mostFrequentWord(root, "i");
	//printf("\n");
	//printf("hi");	
	return 0;
}

