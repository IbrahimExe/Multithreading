#pragma once

#include <thread>
#include <atomic>

template<typename T>
class LockFreeStack
{
private:
    struct Node
    {
        Node(const T& data)
            : data(std::make_shared<T>(data))
        {

        }
        std::shared_ptr<T> data;
        Node* next = nullptr;
    };

    std::atomic<Node*> mHead;
    std::atomic<Node*> mToBeDeleted; // Nodes need to be deleted when safe
    std::atomic<unsigned> mThreadsInPop; // Number of threads in the process of popping from the stack

public:
    // returns true if stack is emtpy
    bool Empty()
    {
        return mHead.load() == nullptr;
    }
    
    void Push(const T& data)
    {
        const Node* newNode(new Node(data));
        newNode->next = mHead.load(); // Set the new node's next pointer to the current head of the stack
        while (!mHead.compare_exchange_weak(newNode->next, newNode)) // Attempt to update the head of the stack to the new node
        {
            // Empty
        }
    }
    std::shared_ptr<T> Pop()
    {
        ++mThreadsInPop; 
        Node* oldHead(mHead.load()); // Get the current head of the stack
        while (oldHead != nullptr && !mHead.compare_exchange_weak(oldHead, oldHead->next)) // Attempt to update the head of the stack to the next node
        {
            // Empty
        }

        std::shared_ptr<T> returnedValue;
        if (oldHead != nullptr)
        {
            returnedValue.swap(oldHead->data); // Swap the data from the old head to the returned value
            TryReclaim(oldHead); // Try to reclaim the old head node if it's safe to do so
        }

        return returnedValue; // Return the value that was popped from the stack
    }

    void TryReclaim(Node* oldHead)
    {
        if (mThreadsInPop = 1)
        {
            // First check if it is ok to delete other nodes which are waiting to be deleted
            Node* nodesToDelete = mToBeDeleted.exchange(nullptr); // Get the list of nodes that need to be deleted and set it to nullptr
            if (!--mThreadsInPop) // make sure its not 0
            {
                // still the only thread in pop, do delete all nodes waiting to be deleted
                DeleteNodes(nodesToDelete);
            }
            else if (nodesToDelete != nullptr)
            {
                ChainPendingNodes(nodesToDelete);
            }
            // Next reclaim the memory of the oldHead
            delete oldHead;
        }
        else
        {
            ChainPendingNode(oldHead);
            --mThreadsInPop;
        }
    }

    void ChainPendingNodes(Node* nodes)
    {
        Node* last(nodes);
        while (const Node* next = last->next)
        {
            last = next;
        }
        
        EnlistPendingNodes(nodes, last);
    }

    void EnlistPendingNodes(Node* first, Node* last)
    {
        last->next = mToBeDeleted.load();
        while (!mToBeDeleted.compare_exchange_weak(last->next, first))
        {
            // Empty
        }
    }

    void ChainPendingANode(Node* node)
    {
        EnlistPendingNodes(node, node);
    }

    // Delete all the ndoes in the node to delete the last
    static void DeleteNodes(Node* nodes)
    {
        while (nodes != nullptr)
        {
            Node* next(nodes->next); // Get the next node in the list
            delete nodes;
            nodes = next;
        }
    }
};
