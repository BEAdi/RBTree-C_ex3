/**
* @file RBTree.c
* @author  Adi Bamberger Edri <adi.bamberger@mail.huji.ac.il>
* @version 1.0
* @date 12 Dec 2019
*
* @brief System to make a generic Red Black Tree with insertion, with finding duplicates and with
* the functionality of going over the whole tree.
*
* @section DESCRIPTION
* The system adds the given data to the tree if it is not in it yet, and can find whether a given
* data is in the tree, and can go over the tree with the wanted functionality.
*/

// ------------------------------ includes ------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "RBTree.h"

// -------------------------- const definitions -------------------------

// The start size of a tree
#define START_SIZE (0)

// The error massage that is to be printed if an error occurred when using malloc
#define ERR_MALLOC "Memory allocation failed\n"

// Stands for being a right or left child
#define RIGHT_CHILD (1)
#define LEFT_CHILD (-1)

// Stands for success or failure of a function
#define FAILURE (0)
#define SUCCESS (1)

// ------------------------------ functions -----------------------------

/**
 * constructs a new RBTree with the given CompareFunc.
 * comp: functions to compare two variables.
 */
RBTree *newRBTree(CompareFunc compFunc, FreeFunc freeFunc)
{
    RBTree *newTree = (RBTree *) malloc(sizeof(RBTree));
    if (newTree == NULL)
    {
        fprintf(stderr, "%s", ERR_MALLOC);
        return NULL;
    }
    newTree->root = NULL;
    newTree->compFunc = compFunc;
    newTree->freeFunc = freeFunc;
    newTree->size = START_SIZE;
    return newTree;
}

/**
 * @brief checks whether the given node is a right child according to the given compFunc
 * @param compFunc the compare function to check with
 * @param toCheck the Node to check what side it is
 * @return 1 if it is a right child, 0 if it doesn't have a parent, -1 if it isn't those two- so
 * it is a left child
 */
int isRightChild(CompareFunc compFunc, Node *toCheck)
{
    Node *parent = toCheck->parent;
    if (parent == NULL)
    {
        return 0;
    }
    if (parent->right != NULL && compFunc(parent->right->data, toCheck->data) == 0)
    {
        return RIGHT_CHILD;
    }
    return LEFT_CHILD;
}

/**
 * @brief finds and returns the uncle of the given Node
 * @param toFind the node to find it's uncle
 * @param tree the tree we work in
 * @return the uncle of the given node
 */
Node *findUncle(Node *toFind, RBTree *tree)
{
    if (isRightChild(tree->compFunc, toFind->parent) == RIGHT_CHILD)
    {
        return toFind->parent->parent->left;
    }
    return toFind->parent->parent->right;
}

/**
* @brief starts the rotation for the left-right case
* @param parent the parent of the added node
* @param parent the parent of the added node
* @param grandparent the grandparent of the added node
*/
void leftRightSwitch(Node *newNode, Node *parent, Node *grandparent)
{
    parent->right = newNode->left;
    if (newNode->left != NULL)
    {
        newNode->left->parent = parent;
    }
    newNode->left = parent;
    parent->parent = newNode;
    newNode->parent = grandparent;
    grandparent->left = newNode;
}

/**
 * @brief does the rotation for the left-left case
 * @param parent the parent of the added node
 * @param grandparent the grandparent of the added node
 * @param tree the tree to do the change in
 */
void leftLeftSwitch(Node *parent, Node *grandparent, RBTree *tree)
{
    parent->parent = grandparent->parent;
    if (parent->parent == NULL)
    {
        tree->root = parent;
    }
    else
    {
        if (isRightChild(tree->compFunc, grandparent) == RIGHT_CHILD)
        {
            grandparent->parent->right = parent;
        }
        else
        {
            grandparent->parent->left = parent;
        }
    }
    grandparent->left = parent->right;
    if (parent->right != NULL)
    {
        parent->right->parent = grandparent;
    }
    parent->right = grandparent;
    grandparent->parent = parent;
}

/**
 * @brief starts the rotation for the right-left case
 * @param parent the parent of the added node
 * @param parent the parent of the added node
 * @param grandparent the grandparent of the added node
 */
void rightLeftSwitch(Node *newNode, Node *parent, Node *grandparent)
{
    parent->left = newNode->right;
    if (newNode->right != NULL)
    {
        newNode->right->parent = parent;
    }
    newNode->right = parent;
    parent->parent = newNode;
    grandparent->right = newNode;
    newNode->parent = grandparent;
}

/**
 * @brief does the rotation for the right-right case
 * @param parent the parent of the added node
 * @param grandparent the grandparent of the added node
 * @param tree the tree to do the change in
 */
void rightRightSwitch(Node *parent, Node *grandparent, RBTree *tree)
{
    parent->parent = grandparent->parent;
    if (parent->parent == NULL)
    {
        tree->root = parent;
    }
    else
    {
        if (isRightChild(tree->compFunc, grandparent) == RIGHT_CHILD)
        {
            grandparent->parent->right = parent;
        }
        else
        {
            grandparent->parent->left = parent;
        }
    }
    grandparent->right = parent->left;
    if (parent->left != NULL)
    {
        parent->left->parent = grandparent;
    }
    parent->left = grandparent;
    grandparent->parent = parent;
}

/**
 * @brief does the rotation for when the parent is red and the uncle id black
 * @param newNode the node that was added and made us do this
 * @param parent the parent of the added node
 * @param grandparent the grandparent of the added node
 * @param tree the tree to do the change in
 */
void modifyRedBlack(Node *newNode, Node *parent, Node *grandparent, RBTree *tree)
{
    int nChildType = isRightChild(tree->compFunc, newNode);
    int pChildType = isRightChild(tree->compFunc, parent);
    if (pChildType == LEFT_CHILD)
    {
        if (nChildType == RIGHT_CHILD)
        {
            leftRightSwitch(newNode, parent, grandparent);
            parent = newNode;
        }
        leftLeftSwitch(parent, grandparent, tree);
    }
    else if (pChildType == RIGHT_CHILD)
    {
        if (nChildType == LEFT_CHILD)
        {
            rightLeftSwitch(newNode, parent, grandparent);
            parent = newNode;
        }
        rightRightSwitch(parent, grandparent, tree);
    }
    parent->color = BLACK;
    grandparent->color = RED;
}

/**
 * @brief modifies the given tree that was made unbalanced when the node that is given was added
 * @param toModify the node to modify it's place
 * @param tree the tree to do the changes in
 */
void modifyNode(Node *toModify, RBTree *tree)
{
    // we inserted the root
    if (toModify == tree->root)
    {
        toModify->color = BLACK;
        return;
    }
    Node *parent = toModify->parent;
    // the parent is black
    if (parent->color == BLACK)
    {
        return;
    }
    Node *uncle = findUncle(toModify, tree); // must be (real or NULL) because parent is red
    Node *grandparent = parent->parent;
    // parent is red and uncle is red
    if (uncle != NULL)
    {
        if (uncle->color == RED)
        {
            parent->color = BLACK;
            uncle->color = BLACK;
            grandparent->color = RED;
            modifyNode(grandparent, tree);
            return;
        }
    }
    // parent is red and uncle is black
    modifyRedBlack(toModify, parent, grandparent, tree);
}

/**
 * @brief adds the new Node given to the correct place in relative to the given compare to Node
 * @param compToNode a not NULL Node to put the Node that we want to add in relative to
 * @param addNode the new Node to add to the tree
 * @return 1 if was added well, 0 if didn't (there was already another node with the same data)
 */
int addNewNode(Node *compToNode, Node *addNode, CompareFunc compFunc)
{
    int compare = compFunc(addNode->data, compToNode->data);
    if (compare == 0)
    {
        return FAILURE;
    }
    if (compare < 0)
    {
        if (compToNode->left == NULL)
        {
            compToNode->left = addNode;
            addNode->parent = compToNode;
            return SUCCESS;
        }
        return addNewNode(compToNode->left, addNode, compFunc);
    }
    // else: compare > 0
    if (compToNode->right == NULL)
    {
        compToNode->right = addNode;
        addNode->parent = compToNode;
        return SUCCESS;
    }
    return addNewNode(compToNode->right, addNode, compFunc);
}

/**
 * @brief Allocates a new Node, with no children nor parent, with the data given and red colored
 * @param data the data the data of the new node
 * @return a pointer to this Node (should be freed!) or NULL if the allocation didn't work
 */
Node *makeNewNode(void *data)
{
    Node *newNode = (Node *) malloc(sizeof(Node));
    if (newNode == NULL)
    {
        fprintf(stderr, "%s", ERR_MALLOC);
        return NULL;
    }
    newNode->right = NULL;
    newNode->left = NULL;
    newNode->parent = NULL;
    newNode->data = data;
    newNode->color = RED;
    return newNode;
}

/**
 * @brief frees the given node by first freeing it's children by recursion
 * @param node the node to free
 * @param freeFunc the function to free the node with
 */
void freeNodes(Node *node, FreeFunc freeFunc)
{
    if (node != NULL)
    {
        if (node->left != NULL)
        {
            freeNodes(node->left, freeFunc);
        }
        if (node->right != NULL)
        {
            freeNodes(node->right, freeFunc);
        }
        if (node->data != NULL)
        {
            freeFunc(node->data);
            node->data = NULL;
        }
        free(node);
    }
}

/**
 * add an item to the tree
 * @param tree: the tree to add an item to.
 * @param data: item to add to the tree.
 * @return: 0 on failure, other on success. (if the item is already in the tree - failure).
 */
int addToRBTree(RBTree *tree, void *data)
{
    Node *newNode = makeNewNode(data);
    if (newNode == NULL)
    {
        return FAILURE;
    }
    if (tree->root == NULL)
    {
        tree->root = newNode;
    }
    else if (addNewNode(tree->root, newNode, tree->compFunc) == 0)
    {
        free(newNode);
        return FAILURE;
    }
    modifyNode(newNode, tree);
    tree->size += 1;
    return SUCCESS;
}

/**
 * check whether the tree contains this item.
 * @param tree: the tree to add an item to.
 * @param data: item to check.
 * @return: 0 if the item is not in the tree, other if it is.
 */
int containsRBTree(RBTree *tree, void *data)
{
    if (tree == NULL || data == NULL || tree->root == NULL)
    {
        return FAILURE;
    }
    Node *curNode = tree->root;
    while (curNode != NULL && curNode->data != NULL)
    {
        int comp = tree->compFunc(curNode->data, data);
        if (comp == 0)
        {
            return SUCCESS;
        }
        else if (comp > 0)
        {
            curNode = curNode->left;
        }
        else if (comp < 0)
        {
            curNode = curNode->right;
        }
    }
    return FAILURE;
}

/**
 * @brief goes over the tree from the smallest node to the biggest one by recursion
 * @param node the current node we are looking at
 * @param func the function to invoke on the node given
 * @param args optional argument that the function might use
 * @return: 0 on failure in any step, other on success.
 */
int forEachNode(Node *node, forEachFunc func, void *args)
{
    if (node->left != NULL)
    {
        if (forEachNode(node->left, func, args) == FAILURE)
        {
            return FAILURE;
        }
    }
    if (func(node->data, args) == 0)
    {
        return FAILURE;
    }
    if (node->right != NULL)
    {
        if (forEachNode(node->right, func, args) == FAILURE)
        {
            return FAILURE;
        }
    }
    return SUCCESS;
}

/**
 * Activate a function on each item of the tree. the order is an ascending order. if one of the activations of the
 * function returns 0, the process stops.
 * @param tree: the tree with all the items.
 * @param func: the function to activate on all items.
 * @param args: more optional arguments to the function (may be null if the given function support it).
 * @return: 0 on failure, other on success.
 */
int forEachRBTree(RBTree *tree, forEachFunc func, void *args)
{
    if (tree == NULL || func == NULL)
    {
        return FAILURE;
    }
    return forEachNode(tree->root, func, args);
}

/**
 * free all memory of the data structure.
 * @param tree: the tree to free.
 */
void freeRBTree(RBTree *tree)
{
    if (tree != NULL)
    {
        if (tree->root != NULL)
        {
            freeNodes(tree->root, tree->freeFunc);
        }
        free(tree);
    }
}