#pragma once
#include <queue>
#include <thread>
#include <mutex>
//#include <condition_variable>
#include <iostream>

using namespace std;

class Node
{
public:
    Node(int data) : value(data), next(nullptr) {}
    int value;
    Node* next;
    std::mutex node_mutex;
};

class FineGrainedQueue
{
public:
    FineGrainedQueue() : head(nullptr) {}
    void push_back(int data)
    {
        // ������� ����� ����
        Node* node = new Node(data);
        if (this) this->queue_mutex.lock();
        // ���� ������ ����, ���������� ����
        if (this->head == nullptr)
        {
            head = node;
            queue_mutex.unlock();
            return;
        }
        // � ����� ���� ��������� ������� ������
        this->head->node_mutex.lock();
        queue_mutex.unlock();
        Node* last = this->head;
        while (last->next != nullptr)
        {
            Node* old = last;
            last->next->node_mutex.lock();
            last = last->next;
            old->node_mutex.unlock();
        }
        // ��������� ��������� next ���������� ���� �� ��������� �� ����� ����
        last->next = node;
        last->node_mutex.unlock();
        return;
    }
    void push_front(int data)
    {
        Node* node = new Node(data);
        head->node_mutex.lock();
        node->next = head;
        head->next->node_mutex.unlock();
        head = node;
    }
    void insertIntoMiddle(int pos, int data)
    {
        // ������� ����� ���� 
        Node* newNode = new Node(data);
        if (this) this->queue_mutex.lock();

        // ������� ������ - �������  � ������ ������
        if (pos == 0)
        {
            newNode->next = head;
            head = newNode;
            this->queue_mutex.unlock();
            return;
        }

        int currPos = 0;

        Node* current = this->head;
        this->queue_mutex.unlock();
        current->node_mutex.lock();
        // � ����� ���� �� ������, ���� ������ �� ��������, ��� ���� �� ������ �� �������
        while (currPos < pos - 1 && current->next != nullptr)
        {
            Node* next = current->next;
            next->node_mutex.lock();
            current->node_mutex.unlock();
            current = next;
            currPos++;
        }
        // ������ ��������� �� ��������� ���� �� ��������� �� ����� ����
        bool ind = false;
        Node* Next = nullptr;
        if (current->next)
        {
            current->next->node_mutex.lock();
            ind = true;
            Next = current->next;
        }
        current->next = newNode;
        current->node_mutex.unlock();
        current = current->next;
        // ��������� ������ �������, ������ ��������� �� ����, ��������� ����� ������ ����, �� ��������� �� ����, ��������� �� current
        if (ind)
        {

            current->next = Next;
            current->next->node_mutex.unlock();
        }
    }
    void show()
    {
        queue_mutex.lock();
        for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) {
            std::cout << ptr->value << " ";
        }
        std::cout << std::endl;
        queue_mutex.unlock();
    }
    void remove(int data)
    {
        Node* prev, * cur; // ��������� �� ���������� � ������� �������
        queue_mutex.lock();

        //��������� ������ ������� ������
        if (!this->head)
        {
            std::cout << "������ ������!\n";
            queue_mutex.unlock();
            return;
        }

        prev = this->head; // �����: ���������� = ������ ������
        cur = this->head->next; // �����: ������� = ������ ������� ������

        prev->node_mutex.lock();
        queue_mutex.unlock();

        // ��������� ������ �������� ������� ��������
        if (prev->value == data)
        {
            if (cur)
            {
                cur->node_mutex.lock();//��������� � ������ ����� ��������
                head = cur;
                cur->node_mutex.unlock();
                prev->node_mutex.unlock();
                delete prev;
                return;
            }
            head = cur;
            prev->node_mutex.unlock();
            delete prev;
            return;
        }
        if (cur) cur->node_mutex.lock();//��������� � ������ ����� ��������
        while (cur) // ���� ��������� �� ������� ������� �� �������... 
        {
            if (cur->value == data) // ���� ����� ������� ��� ��������... 
            {
                // �� ������� ���
                prev->next = cur->next;
                prev->node_mutex.unlock();
                cur->node_mutex.unlock();
                delete cur;
                return;
            }

            // ���� �� ����� ������� ��� ��������, �� ��������� ������ �� �������
            Node* old_prev = prev;
            prev = cur;
            cur = cur->next;

            old_prev->node_mutex.unlock(); // ��������� "������" ���������� �������
            if (cur) cur->node_mutex.lock(); // ������� ����� ������� �������
        }
        prev->node_mutex.unlock();
    }


private:
    Node* head;
    std::mutex queue_mutex;
};

