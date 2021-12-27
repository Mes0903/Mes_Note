#include <iostream>
#include <memory>
class List {
    struct Node {
        int data;
        std::unique_ptr<Node> next;
        Node()
            : data(0) {}
        Node(int _d)
            : data(_d) {}
        Node(int _d, std::unique_ptr<Node> &_next)
            : data(_d), next(std::move(_next)) {}
        ~Node() { puts("~Node"); }
    };

    std::unique_ptr<Node> *_find(const int target);
    std::unique_ptr<Node> *_find(std::unique_ptr<Node> &target);

  public:
    std::unique_ptr<Node> head;

    void erase(const int target);
    void erase(std::unique_ptr<Node> &target);
    void insert_before(const int target, const int data);
    void insert_before(std::unique_ptr<Node> &target, const int data);
    void output();

    List()
        : head(nullptr) {}
};

std::unique_ptr<List::Node> *List::_find(const int target)
{
    std::unique_ptr<List::Node> *indirect = &head;
    while (*indirect && (*indirect)->data != target)
        indirect = &(*indirect)->next;

    return indirect;
}

std::unique_ptr<List::Node> *List::_find(std::unique_ptr<Node> &target)
{
    std::unique_ptr<List::Node> *indirect = &head;
    while (*indirect && *indirect != target)
        indirect = &(*indirect)->next;

    return indirect;
}

void List::erase(const int target)
{
    std::unique_ptr<List::Node> *indirect = _find(target);

    if (*indirect)
        *indirect = std::move((*indirect)->next);
}

void List::erase(std::unique_ptr<Node> &target)
{
    std::unique_ptr<List::Node> *indirect = _find(target);

    if (*indirect)
        *indirect = std::move(target->next);
}

void List::insert_before(const int target, int data)
{
    std::unique_ptr<List::Node> *indirect = _find(target);
    *indirect = std::make_unique<List::Node>(data, *indirect);
}

void List::insert_before(std::unique_ptr<Node> &target, const int data)
{
    std::unique_ptr<List::Node> *indirect = _find(target);
    *indirect = std::make_unique<List::Node>(data, *indirect);
}

void List::output()
{
    std::unique_ptr<List::Node> *indirect = &head;

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