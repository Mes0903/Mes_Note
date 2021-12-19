#include <iostream>
#include <memory>

struct Node {
    int data;
    std::unique_ptr<Node> next;
};



int main()
{
}