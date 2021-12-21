#include <iostream>

class List {
    struct Node {
        int data;
        Node *next;
    };

    Node *head;
    Node **find(Node *target);

  public:
    void erase(Node *target);
    void ins
};
