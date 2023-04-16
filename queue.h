#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

#define QUEUE_SIZE 5

class Queue
{
    int q_front,rear, capacity;
    double fixed_queue[QUEUE_SIZE];
public:
    Queue();
    void insert_queue(double price);
    void dequeue_queue();
    void display();
    void test();
    double average();
};

#endif // QUEUE_H_INCLUDED
