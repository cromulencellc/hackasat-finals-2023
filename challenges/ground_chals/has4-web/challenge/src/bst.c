#include "bst.h"
#include <malloc.h>
#include "debug.h"


void bst_print( bstType *bst) {

    if ( bst == 0 ) 
        return;

    if ( bst->data) {

        debug( "%d, %s, %d\n", bst->key, (char *)bst->data, bst->count);
    }
    else {

        debug( "%d - %d\n", bst->key, bst->count);
    }

	return;

}

int addBstNode( bstType **bst, unsigned int key, void *value) {

bstType *temp;


    if ( *bst == 0 ) {

        *bst = calloc( sizeof(bstType), 1);

        if (*bst == NULL ) {

            debug( "Unable to malloc memory for BST node\n");
            return -1;

        }

        (*bst)->key = key;

        if ( value != 0 ) {
            (*bst)->data = value;
        }
        else {
            (*bst)->data = 0;
        }
            

        (*bst)->count = 1;

        return 0;
    }

    temp = *bst;

    while(1) {

        if (key == temp->key ) {

            temp->count++;
            return 0;
        }
        else if (key > temp->key) {

            if ( temp->right == 0 ) {

                temp->right = calloc(sizeof(bstType), 1);

                if (temp->right ) {

                    temp->right->key = key;

                    if ( value != 0 ) {
                        temp->right->data = value;
                    }
                    else {

                        temp->right->data = 0;
                    }
                    temp->right->count = 1;
                    return 0;
                }
                else {

                    return -1;
                }
            }
            else {

                temp = temp->right;
                continue;
            }
        }
        else { // key < temp->key 

            if ( temp->left == 0 ) {

                temp->left = calloc(sizeof(bstType), 1);

                if (temp->left ) {

                    temp->left->key = key;

                    if ( value != 0 ) {
                        temp->left->data = value;
                    }
                    else {
                        temp->left->data = 0;
                    }

                    temp->left->count = 1;
                    return 0;
                }
                else {

                    return -1;
                }
            }
            else {

                temp = temp->left;
                continue;
            }

        }

    } // while(1)

}

int incBstNode( bstType **bst, unsigned int key) {

bstType *temp;

    if (*bst == 0 ) 
        return -1;

    temp = *bst;

    while (1) {

        if (temp->key == key ) {

            temp->count ++;

            return 0;
        }

        if ( key > temp->key )
            temp = temp->right;
        else
            temp = temp->left;

        // didn't find the key
        if ( temp == 0 ) 
            return -1;

    }

    return 0;
}

void *findBstNode( bstType *bst, unsigned int key) {

bstType *temp;

    temp = bst;

    if ( temp == 0 ) {

        return 0;
    }

    while (1) {

        if (temp->key == key ) {

            return (void *)temp;
        }

        if ( key < temp->key )  {

            if ( temp->left ) {

                temp = temp->left;
            }
            else { 

                return (void *)0;
            }
        }
        else { 

            if ( temp->right ) {

                temp = temp->right;

            }
            else {

                return (void *)0;
            }
        }

    }

    return (void *)0;
}

int deleteBstNode( bstType **bst, unsigned int key ) {


    if (*bst == 0) 
        return -1;


    
return 0;
}

// delete the entire BST
int deleteBst( bstType **bst) {


    if (*bst == 0)
        return 0;

    if ( (*bst)->left == 0 ) {

        if ( (*bst)->right == 0 ) {

            if ( (*bst)->data ) {

                free((*bst)->data);
            }

            free(*bst);
            *bst = 0;
            return 0;
        }
    }
    else {

        deleteBst( &((*bst)->left) );

    }

    if ( (*bst)->right) {

        deleteBst( &((*bst)->right));

    } 

    if ( (*bst)->data) {

        free((*bst)->data);
    }

        free(*bst);
        *bst = 0;
        return 0;

return 0;

}

int printBst( int fd, bstType *bst, void (*printfunct)(int, bstType *)) {

bstType *temp;

    temp = bst;

    if ( temp == 0 ) 
        return -1;

    if ( temp->left ) {

        printBst( fd, temp->left, printfunct );
    }

    if (printfunct)
        printfunct(fd, temp);
    else
    {
        bst_print(temp);
    }
    
    if ( temp->right ) {

        printBst(fd, temp->right, printfunct);
    }


return 0;
}