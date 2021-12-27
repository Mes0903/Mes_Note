#include <iostream>
#include <memory>

namespace detail {
  struct Node {
    int data;

    using node_uptr = std::unique_ptr<Node>;
    node_uptr next;
    Node()
        : data(0) {}
    Node(int _d)
        : data(_d) {}
    Node(int _d, std::unique_ptr<Node> &_next)
        : data(_d), next(std::move(_next)) {}
    ~Node() { puts("~Node"); }
  };
}    // namespace detail

class List {
  using node_uptr = std::unique_ptr<detail::Node>;
  node_uptr *_find(const int target);
  node_uptr *_find(node_uptr &target);

public:
  node_uptr head;

  void erase(const int target);
  void erase(node_uptr &target);
  void insert_before(const int target, const int data);
  void insert_before(node_uptr &target, const int data);
  void output();

  List() = default;
  ~List();
};

List::node_uptr *List::_find(const int target)
{
  node_uptr *indirect = &head;
  while (*indirect && (*indirect)->data != target)
    indirect = &(*indirect)->next;

  return indirect;
}

List::node_uptr *List::_find(node_uptr &target)
{
  node_uptr *indirect = &head;
  while (*indirect && *indirect != target)
    indirect = &(*indirect)->next;

  return indirect;
}

void List::erase(const int target)
{
  node_uptr *indirect = _find(target);

  if (*indirect)
    *indirect = std::move((*indirect)->next);
}

void List::erase(node_uptr &target)
{
  node_uptr *indirect = _find(target);

  if (*indirect)
    *indirect = std::move(target->next);
}

void List::insert_before(const int target, int data)
{
  node_uptr *indirect = _find(target);
  *indirect = std::make_unique<detail::Node>(data, *indirect);
}

void List::insert_before(node_uptr &target, const int data)
{
  node_uptr *indirect = _find(target);
  *indirect = std::make_unique<detail::Node>(data, *indirect);
}

void List::output()
{
  node_uptr *indirect = &head;

  while (*indirect) {
    std::cout << (*indirect)->data << " ";
    indirect = &(*indirect)->next;
  }
  std::cout << '\n';
}

List::~List()
{
  while (head)
    head = std::move(head->next);
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