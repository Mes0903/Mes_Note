#include <stdio.h>
#include <stdlib.h>

#define N 20    //the number of Nodes

typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct List {
    Node *head;
} List;

/**
 * @brief Find the target in the list and return it.
 * 
 * @param list Your list.
 * @param target The element you wanna find in the list.
 * @return The indirect pointer to the target, which means *indirect == target.
 */
Node **find(List *list, Node *target)
{
    Node **indirect = &list->head;
    while (*indirect && *indirect != target)
        indirect = &(*indirect)->next;

    return indirect;
}

/**
 * @brief Delete the target in the element, if the target is not in the list, function won't do anything.
 * 
 * @param list Your list.
 * @param target The element you wanna delete in the list.
 */
void erase(List *list, Node *target)
{
    Node **indirect = find(list, target);

    if (*indirect)
        *indirect = target->next;
}

/**
 * @brief Insert the item before the taeget into the list.
 * 
 * @param list Your list.
 * @param target The element behind the Node you wanna insert.
 * @param item The element you wanna insert into the list.
 */
void insert_before(List *list, Node *target, Node *item)
{
    Node **indirect = find(list, target);

    item->next = *indirect;
    *indirect = item;
}

/**
 * @brief Print the whole list.
 * 
 * @param list Your list.
 */
void output(List *list)
{
    Node **indirect = &list->head;

    while (*indirect) {
        printf("%d ", (*indirect)->data);
        indirect = &(*indirect)->next;
    }
}

int main()
{
    Node items[N];
    List list;

    // initialize items and head pointer
    for (size_t i = 0; i < N; ++i) {
        items[i].data = i;
        items[i].next = NULL;
    }
    list.head = NULL;

    erase(&list, &items[0]);    // delete when list is empty

    // insert element from head pointer
    for (size_t i = 0; i < N; i++)
        insert_before(&list, list.head, &items[i]);

    erase(&list, &items[5]);    // normal delete element

    erase(&list, &items[5]);    // delete element which is not in list
    erase(&list, list.head);    // delete the head node

    output(&list);
}