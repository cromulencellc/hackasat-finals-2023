#ifndef bst_h
#define bst_h


typedef struct bstStruct { 

    unsigned int key;
    int count;
    void *data;

    struct bstStruct *left;
    struct bstStruct *right;


} bstType;


int addBstNode( bstType **bst, unsigned int key, void *value);
int incBstNode( bstType **bst, unsigned int key);
void *findBstNode( bstType *bst, unsigned int key);
int deleteBstNode( bstType **bst, unsigned int key );
int deleteBst( bstType **bst);
int printBst( int fd, bstType *bst, void (*printfunct)(int fd, bstType *));

#endif
