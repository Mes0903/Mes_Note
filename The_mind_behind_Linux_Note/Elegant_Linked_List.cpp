#include <iostream>

class List {
    struct Node {
        int data;
        Node *next;
        Node()
            : data(0), next(nullptr) {}
        Node(int _d, Node *_next = nullptr)
            : data(_d), next(_next) {}
        ~Node() { puts("~Node"); }
    };

    Node **_find(const int target);
    Node **_find(Node *target);

  public:
    Node *head;

    void erase(const int target);
    void erase(Node *target);
    void insert_before(const int target, const int data);
    void insert_before(Node *target, const int data);
    void output();

    List()
        : head(nullptr) {}
};

List::Node **List::_find(const int target)
{
    Node **indirect = &head;
    while (*indirect && (*indirect)->data != target)
        indirect = &(*indirect)->next;

    return indirect;
}

List::Node **List::_find(Node *target)
{
    Node **indirect = &head;
    while (*indirect && *indirect != target)
        indirect = &(*indirect)->next;

    return indirect;
}

void List::erase(const int target)
{
    Node **indirect = _find(target);

    if (*indirect) {
        Node *tmp = *indirect;
        *indirect = tmp->next;
        delete tmp;
    }
}

void List::erase(Node *target)
{
    Node **indirect = _find(target);

    if (*indirect) {
        Node *tmp = *indirect;
        *indirect = target->next;
        delete tmp;
    }
}

void List::insert_before(const int target, const int data)
{
    Node **indirect = _find(target);
    *indirect = new Node(data, *indirect);
}

void List::insert_before(Node *target, const int data)
{
    Node **indirect = _find(target);
    *indirect = new Node(data, *indirect);
}

void List::output()
{
    Node **indirect = &head;

    while (*indirect) {
        std::cout << (*indirect)->data << " ";
        indirect = &(*indirect)->next;
    }
    std::cout << '\n';
}

int main()
{
    List list;

    list.erase(5);

    for (std::size_t i{}; i < 20; ++i)
        list.insert_before(list.head, i);

    list.output();

    list.erase(5);
    list.erase(5);
    list.erase(list.head);

    list.output();
}