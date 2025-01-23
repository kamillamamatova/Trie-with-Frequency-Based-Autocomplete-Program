/* This code manages vehicle owners and their fines. The system keeps track 
of owner names, their fines, and their position in the tree. It provides 
functionalities for adding, searching, deducting, deleting, calculating, 
and balancing nodes (representing owners). */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct trieNode{
    // (1/0) 1: if it is the end of a valid word
    int isWord;
    // How many times this specific string has been added to the dictionary
    int freq;
    // Sum of words that are sharing a prefix
    int sumPrefixFreq;
    // The highest frequency among all child nodes of the current node
    int maxFreq;
    struct trieNode * children[26];
} trieNode;

// Prototypes
trieNode * initialize();
void insert(trieNode * root, char word[], int freq, int k);
char * query(trieNode * root, char prefix[], int k);
void freeDictionary(trieNode * root);
int isEmpty(trieNode * root);

int main(){
    struct trieNode * root = initialize();
    int n;

    // Read in the number of commands
    scanf("%d", &n);

    for(int i = 0; i < n; i++){
        int command;
        scanf("%d", &command);
        // If user inputs 1, the insert function is implemented
        if(command == 1){
            char word[100];
            int frequency;
            scanf("%s %d", word, &frequency);
            insert(root, word, frequency, 0);
        }
        // If user inputs 2, the query function is implemented
        else if(command == 2){
            char prefix[100];
            scanf("%s", prefix);
            char * result = query(root, prefix, 0);
            printf("%s\n", result);
            free(result);
        }
    }

    freeDictionary(root);

    return 0;
}

trieNode * initialize(){
    // Memory allocation
    trieNode * node = (trieNode*)malloc(sizeof(trieNode));

    // Initialize to 0
    node->isWord = 0;
    node->freq = 0;
    node->sumPrefixFreq = 0;

    // Initialize all child pointers to NULL
    for(int i = 0; i < 26; i++){
        node->children[i] = NULL;
    }

    return node;
}

/* The word has been used an addition freq times. The same word
may appear multiple times in the input. */
void insert(trieNode * root, char word[], int freq, int k){
    // update existing nodes instead of creating duplicates unnecessarily
    if(k == strlen(word)){
        // Current node is marked as the end of a valid word
        root->isWord = 1;
        // Add to the word's frequency with the given freq
        root->freq += freq;
        // Add to the sum of the prefix frequency
        root->sumPrefixFreq += freq;
        return;
    }

    // Find the index of the letter
    int childIndex = word[k] - 'a';

    // Make sure it's within bounds
    if(childIndex < 0 || childIndex >= 26){
        return;
    }

    // If the next node doesn't exist,
    if(root->children[childIndex] == NULL){
        // then allocate memory for a new node 
        root->children[childIndex] = initialize();
    }

    root->sumPrefixFreq += freq;

    // Update the maximum frequency in the current node
    int maxChildFreq = 0;
    for(int i = 0; i < 26; i++){
        if(root->children[i]){
            if(root->children[i]->sumPrefixFreq > root->maxFreq){
                maxChildFreq = root->children[childIndex]->sumPrefixFreq;
            }
        }
    }    

    root->maxFreq = maxChildFreq;
    
    // Recursion
    // Insert at the next index of the word
    insert(root->children[childIndex], word, freq, k + 1);
}

/* Asks the program to find the most likely next letter(s) based
on the current dictionary. A query does not modify the dictionary. */
char * query(trieNode * root, char prefix[], int k){
    // Base case
    if(root == NULL){
        // 20 because "unrecognized prefix" is 19 indexes + 1 (NULL terminator)
        char * result = (char*)malloc(20 * sizeof(char)); // sizeof(char) is always 1
        strcpy(result, "unrecognized prefix");
        return result;
    }

    if(k == strlen(prefix)){
        // 26 letters + 1 (NULL terminator)
        char * result = (char*)malloc(27 * sizeof(char));
        int pos = 0;

        // Find the maximum frequency
        int maxFreq = 0;
        for(int i = 0; i < 26; i++){
            if(root->children[i] && root->children[i]->sumPrefixFreq > maxFreq){
                /* Set the maximum frequency to the cumulative frequency of all words 
                whose prefixes include the current character and continue further down
                the trie. */
                maxFreq = root->children[i]->sumPrefixFreq;
            }
        }

        // Store letters if sum == max
        for(int i = 0; i < 26; i++){
            if(root->children[i] && root->children[i]->sumPrefixFreq == maxFreq){
                // Find what the character is based on the index
                char character = 'a' + i;
                // The character is stored in the result array at position pos
                result[pos] = character;
                // Increment the position index
                pos++;
            }
        }

        // If there isn't a next character
        if(pos == 0){
            free(result);
            char * unrecognized = (char*)malloc(20 * sizeof(char));
            strcpy(unrecognized, "unrecognized prefix");
            return unrecognized;
        }

        // The result at position pos is the NULL terminator now
        result[pos] = '\0';

        return result;
    }

    int childIndex = prefix[k] - 'a';

    // The prefix is unrecognized if the next node doesn't exist
    if(root->children[childIndex] == NULL){
        char * result = (char*)malloc(20 * sizeof(char));
        strcpy(result, "unrecognized prefix");
        return result;
        // return "unrecognized prefix";  // Return a static string
    }

    // Edge case
    if(childIndex < 0 || childIndex >= 26){
        char * result = (char*)malloc(20 * sizeof(char));
        strcpy(result, "unrecognized prefix");
        return result;
    }

    // Recursion
    // Next character in the prefix
    return query(root->children[childIndex], prefix, k + 1);
}

// Free memory
void freeDictionary(trieNode * root){
    // Base case
    if(root == NULL){
        return;
    }

    // Use for-loop to free every index in the children array recursively
    for(int i = 0; i < 26; i++){
        freeDictionary(root->children[i]);
    }

    free(root);
}

// Checks whether the trie node has no children
int isEmpty(trieNode * root){
    for(int i = 0; i < 26; i++){
        if(root->children[i]){
            return 0;
        }
    }
    return 1;
}
