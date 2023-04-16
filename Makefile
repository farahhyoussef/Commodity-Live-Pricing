all: run

run: producer.o queue.o consumer.o
	g++ -o producer producer.o queue.o
	g++ -o consumer consumer.o queue.o
	
queue.o: queue.cc
	g++ -c queue.cc
	
producer.o: producer.cpp
	g++ -c producer.cpp
consumer.o: consumer.cpp 	
	g++ -c consumer.cpp
clean:
	rm -f producer *.o
	rm -f consumer *.o
	ipcrm -a

