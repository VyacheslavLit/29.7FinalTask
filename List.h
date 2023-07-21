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
        // создаем новый узел
        Node* node = new Node(data);
        if (this) this->queue_mutex.lock();
        // если список пуст, возвращаем узел
        if (this->head == nullptr)
        {
            head = node;
            queue_mutex.unlock();
            return;
        }
        // в цикле ищем последний элемент списка
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
        // Обновляем указатель next последнего узла на указатель на новый узел
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
        // создаем новый узел 
        Node* newNode = new Node(data);
        if (this) this->queue_mutex.lock();

        // крайний случай - вставка  в начало списка
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
        // в цикле идем по списку, пока список не кончится, или пока не дойдем до позиции
        while (currPos < pos - 1 && current->next != nullptr)
        {
            Node* next = current->next;
            next->node_mutex.lock();
            current->node_mutex.unlock();
            current = next;
            currPos++;
        }
        // меняем указатель на следующий узел на указатель на новый узел
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
        // связываем список обратно, меняем указатель на узел, следующий после нового узла, на указатель на узел, следующий за current
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
        Node* prev, * cur; // указатели на предыдущий и текущий элемент
        queue_mutex.lock();

        //обработка случая пустого списка
        if (!this->head)
        {
            std::cout << "Список пустой!\n";
            queue_mutex.unlock();
            return;
        }

        prev = this->head; // старт: предыдущий = начало списка
        cur = this->head->next; // старт: текущий = второй элемент списка

        prev->node_mutex.lock();
        queue_mutex.unlock();

        // обработка случая удаления первого элемента
        if (prev->value == data)
        {
            if (cur)
            {
                cur->node_mutex.lock();//проверили и только потом залочили
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
        if (cur) cur->node_mutex.lock();//проверили и только потом залочили
        while (cur) // пока указатель на текущий элемент не нулевой... 
        {
            if (cur->value == data) // если нашли элемент для удаления... 
            {
                // то удаляем его
                prev->next = cur->next;
                prev->node_mutex.unlock();
                cur->node_mutex.unlock();
                delete cur;
                return;
            }

            // если не нашли элемент для удаления, то двигаемся дальше по очереди
            Node* old_prev = prev;
            prev = cur;
            cur = cur->next;

            old_prev->node_mutex.unlock(); // анлочится "старый" предыдущий элемент
            if (cur) cur->node_mutex.lock(); // лочится новый текущий элемент
        }
        prev->node_mutex.unlock();
    }


private:
    Node* head;
    std::mutex queue_mutex;
};

