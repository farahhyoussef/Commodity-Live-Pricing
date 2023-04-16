#include "queue.h"
#include <iostream>
using namespace std;
Queue::Queue()
{
    q_front = 0;
     rear = 1;
     fixed_queue[0]=0.0;


}
void Queue::insert_queue(double price)
{
    if(rear==QUEUE_SIZE)
    {
        for(int i=0 ; i< QUEUE_SIZE-1 ; i++)
        {
            fixed_queue[i]=fixed_queue[i+1];
        }
        fixed_queue[QUEUE_SIZE-1]=price;
    }
    else
    {
       fixed_queue[rear]= price;
        rear++;
    }
}
void Queue::dequeue_queue()
{
    if (q_front == rear)
    {
        return;
    }
    for(int i=0 ; i< QUEUE_SIZE-1; i++)
    {
        fixed_queue[i]=fixed_queue[i+1];
    }
    rear --;

}

void Queue::display()
{
    //if(q_front == rear)
    //{
    //    return;
    //}
    for(int i=0 ; i< QUEUE_SIZE; i++)
    {
        printf("%f   ",fixed_queue[i]);
    }
}
void Queue::test(){
printf("hi");}


double Queue::average(){
double sum = 0;
for(int i=0 ; i< rear; i++)
    {
        sum+=fixed_queue[i];
    }
return sum/rear;
}

