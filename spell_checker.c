#include <stddef.h>	/* size_t */
#include <assert.h>	/* assert */
#include <stdlib.h>	/* malloc */
#include <stdio.h>	/* printf, fgets, strcpy */
#include <string.h>	/* strspan, strcmp, strdup */

#include "hashtable.h"	/* hash table function declerations */

#define DICTIONARY_FILE_PATH "/usr/share/dict/words"
#define HASH_TABLE_SIZE 30
#define VALID_WORD_SIZE 256
#define NUM_WORDS_TO_LOAD 33174

int LoadDictionary(hash_table_t *table, const char *filename, char **loaded_words);
static size_t HashFunction(const void *key);
static int MatchWords(const void *item, const void *data_to_compare);
static void FreeWords(char **loaded_words);
char *StrDup(const char *c);
void SpellChecker(hash_table_t *table);


int main(void)
{
	char *loaded_words[NUM_WORDS_TO_LOAD] = {0};
	hash_table_t *table = HashTableCreate(HashFunction, MatchWords, HASH_TABLE_SIZE);
	if(NULL == table)
	{
		fprintf(stdout, "Failed to create hash table\n");
		return 1;
	}
	
	if(0 != LoadDictionary(table, DICTIONARY_FILE_PATH, loaded_words))
	{
		HashTableDestroy(table);
		fprintf(stdout, "Failed to load dictionary\n");
		return 1;
	}
	
	printf("Load factor = %f, Standard deviation = %f\n\n", 
	HashTableLoad(table), HashTableStandardDeviation(table));
	
	SpellChecker(table);
	
	FreeWords(loaded_words);
	HashTableDestroy(table);
	
	return 0;
}


int LoadDictionary(hash_table_t *table, const char *filename, char **loaded_words)
{
	char word[VALID_WORD_SIZE] = {0};
	size_t num_words_loaded = 0;
	char *word_dup = NULL;
	
	FILE *file = fopen(filename, "r");
	if(NULL == file)
	{
		return 1;
	}
	
	while (fgets(word, sizeof(word), file) && num_words_loaded < NUM_WORDS_TO_LOAD)
	{
		word[strcspn(word, "\n")] = '\0';
		word_dup = StrDup(word);
		
		if (NULL == word_dup)
		{
			fclose(file);
			return 1;
		}
		
		if (HashTableInsert(table, word_dup, word_dup) != 0)
		{
			fclose(file);
			return 1;
		}
		
		loaded_words[num_words_loaded] = word_dup;
		++num_words_loaded;
	}

	fclose(file);
	
	return 0;
}


void SpellChecker(hash_table_t *table)
{
	char user_input[VALID_WORD_SIZE] = {0};
	char *search_result = NULL;
	printf("Enter word to look in the dictionary. Enter q to quit.\n");
	if (fgets(user_input, sizeof(user_input), stdin) == NULL)
	{
		return;
	}
	
	while (user_input[0] != 'q')
	{
		search_result = NULL;
		user_input[strcspn(user_input, "\n")] = '\0';

		search_result = (char *)HashTableFind(table, user_input);
		if (NULL == search_result)
		{
			printf("Word not found.\n");
		}
		else
		{
			printf("\nI found: %s\n\n", search_result);
		}
		
		printf("Enter key to look in the dictionary.\nEnter q to quit.\n\n");
		if (fgets(user_input, sizeof(user_input), stdin) == NULL)
		{
			return;
		}
		
	}
}


static size_t HashFunction(const void *key)
{
	char *word = (char *)key;
	return (word[0] % HASH_TABLE_SIZE);
}


static int MatchWords(const void *item, const void *data_to_compare)
{
	return (!strcmp((char *)item, (char *)data_to_compare));
}


char *StrDup(const char *c)
{
	char *dup = malloc(strlen(c) + 1);

	if (dup != NULL)
	{
		strcpy(dup, c);
	}

	return dup;
}


static void FreeWords(char **loaded_words)
{
	size_t i = 0;
	
	for (i = 0; i < NUM_WORDS_TO_LOAD; ++i)
	{
		free(loaded_words[i]);
	}
}

