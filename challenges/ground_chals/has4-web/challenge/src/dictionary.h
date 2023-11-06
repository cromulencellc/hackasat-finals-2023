
#ifndef dictionary_h

#define dictionary_h

typedef struct dict {
	
	char *key;
	char *value;

	struct dict *next;
} dictionaryType;

int addDictEntry( dictionaryType **dict, char *key, char*value);
int addDictbyLine( dictionaryType **dict, char *line, char delim);
char *findDict( dictionaryType *dict, char *key);
void deleteDict( dictionaryType **dict);
void printDict( dictionaryType *dict);
dictionaryType *loadINI(char *filename);

#endif
