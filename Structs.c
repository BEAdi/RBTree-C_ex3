/**
* @file TreeAnalyzer.c
* @author  Adi Bamberger Edri <adi.bamberger@mail.huji.ac.il>
* @version 1.0
* @date 12 Dec 2019
*
* @brief System which uses the generic RBTree and implements the functions needed for it
*
* @section DESCRIPTION
* The system holds the casted function to be used in the generic RBtree, for data of type string
 * and of type of a vector.
*/

// ------------------------------ includes ------------------------------
#include "RBTree.h"
#include "Structs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// -------------------------- const definitions -------------------------

// Stands for the being less than, equal to or greater then what we are comparing to
#define LESS (-1)
#define EQUAL (0)
#define GREATER (1)

// The error massage that is to be printed if an error occurred when using malloc
#define ERR_MALLOC "Memory allocation failed\n"

// Stands for success or failure of a function
#define FAILURE (0)
#define SUCCESS (1)

// ------------------------------ functions -----------------------------

/**
 * CompFunc for strings (assumes strings end with "\0")
 * @param a - char* pointer
 * @param b - char* pointer
 * @return equal to 0 iff a == b. lower than 0 if a < b. Greater than 0 iff b < a. (lexicographic
 * order)
 */
int stringCompare(const void *a, const void *b)
{
    return strcmp((char *) a, (char *) b);
}

/**
 * ForEach function that concatenates the given word to pConcatenated. pConcatenated is already allocated with
 * enough space.
 * @param word - char* to add to pConcatenated
 * @param pConcatenated - char*
 * @return 0 on failure, other on success
 */
int concatenate(const void *word, void *pConcatenated)
{
    char *copyTo = (char *) pConcatenated;
    char *toCopy = (char *) word;
    strcat(strcat(copyTo, toCopy), "\n");
    return SUCCESS;
}

/**
 * FreeFunc for strings
 */
void freeString(void *s)
{
    if (s != NULL)
    {
        free((char *) s);
    }
}

/**
 * CompFunc for Vectors, compares element by element, the vector that has the first larger
 * element is considered larger. If vectors are of different lengths and identify for the length
 * of the shorter vector, the shorter vector is considered smaller.
 * @param a - first vector
 * @param b - second vector
 * @return equal to 0 iff a == b. lower than 0 if a < b. Greater than 0 iff b < a.
 */
int vectorCompare1By1(const void *a, const void *b)
{
    if (a == NULL || b == NULL)
    {
        return FAILURE;
    }
    Vector *va = (Vector *) a;
    Vector *vb = (Vector *) b;
    if (vb->vector == NULL || va->vector == NULL)
    {
        return FAILURE;
    }
    int i = 0;
    while (vb->len > i && va->len > i)
    {
        double vaElement = (va->vector)[i];
        double vbElement = (vb->vector)[i];
        if (vaElement < vbElement)
        {
            return LESS;
        }
        if (vaElement > vbElement)
        {
            return GREATER;
        }
        i++;
    }
    if (va->len < vb->len)
    {
        return LESS;
    }
    if (va->len > vb->len)
    {
        return GREATER;
    }
    return EQUAL;
}

/**
 * FreeFunc for vectors
 */
void freeVector(void *pVector)
{
    Vector *theVector = (Vector *) pVector;
    if (theVector != NULL)
    {
        if (theVector->vector != NULL)
        {
            free(theVector->vector);
            theVector->vector = NULL;
        }
        free(theVector);
    }
}

/**
 * @brief calculate the norm (without the root) of the vector given
 * @param pVector the vector to calculate it's norm
 * @return the squared norm of the given vector
 */
double calculateNorm(const Vector *pVector)
{
    double norm = 0;
    for (int i = 0; i < pVector->len; i++)
    {
        norm += ((pVector->vector)[i]) * ((pVector->vector)[i]);
    }
    return norm;
}

/**
 * @brief copies the information from the current vector to the max vector
 * @param curVector the vector to copy the information from
 * @param maxVector the vector to copy the information to
 * @return 1 on success, 0 on failure
 */
int copyLarger(const Vector *curVector, Vector *maxVector)
{
    maxVector->len = curVector->len;
    for (int i = 0; i < curVector->len; i++)
    {
        (maxVector->vector)[i] = (curVector->vector)[i];
    }
    return SUCCESS;
}

/**
 * copy pVector to pMaxVector if : 1. The norm of pVector is greater then the norm of pMaxVector.
 * 								   2. pMaxVector->vector == NULL.
 * @param pVector pointer to Vector
 * @param pMaxVector pointer to Vector
 * @return 1 on success, 0 on failure (if pVector == NULL: failure).
 */
int copyIfNormIsLarger(const void *pVector, void *pMaxVector)
{
    Vector *curVector = (Vector *) pVector;
    Vector *maxVector = (Vector *) pMaxVector;
    if (curVector == NULL || maxVector == NULL)
    {
        return FAILURE;
    }
    if (maxVector->vector == NULL)
    {
        maxVector->vector = (double *) malloc((curVector->len) * sizeof(double));
        if (maxVector->vector == NULL)
        {
            fprintf(stderr, "%s", ERR_MALLOC);
            return FAILURE;
        }
        return copyLarger(curVector, maxVector);
    }
    if (calculateNorm(curVector) > calculateNorm(maxVector))
    {
        maxVector->vector = (double *) realloc(maxVector->vector,
                                               (curVector->len) * sizeof(double));
        if (maxVector->vector == NULL)
        {
            fprintf(stderr, "%s", ERR_MALLOC);
            return FAILURE;
        }
        return copyLarger(curVector, maxVector);
    }
    return SUCCESS;
}

/**
 * @param tree a pointer to a tree of Vectors
 * @return pointer to a *copy* of the vector that has the largest norm (L2 Norm).
 */
Vector *findMaxNormVectorInTree(RBTree *tree)
{
    if (tree == NULL)
    {
        return NULL;
    }
    Vector *newVector = (Vector *) malloc(sizeof(Vector));
    if (newVector == NULL)
    {
        fprintf(stderr, "%s", ERR_MALLOC);
        return NULL;
    }
    newVector->len = 0;
    newVector->vector = NULL;
    if (forEachRBTree(tree, copyIfNormIsLarger, newVector) == 0)
    {
        return NULL;
    }
    return newVector;
}
