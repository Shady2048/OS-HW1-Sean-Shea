# OS-HW1-Sean-Shea

File Description:

producer.cpp: 
Creates the shared semaphore and shared memory for the consumer.cpp  file to 
link to, and also generates random numbers to an array to be stored in the 
shared memory in its critical section.

consumer.cpp: 
Links to the semaphore and shared memory created by producer.cpp and takes 
the numbers from the shared memory and adds them to a sum variable. At the 
end of the communication with the producer.cpp, it prints the final sum.

Usage Instructions:

1.) Type the following commands to compile the files:

    g++ producer.cpp -pthread -lrt -o producer
    g++ consumer.cpp -pthread -lrt -o consumer

2.) Type the following command to run the two programs at the same time:

    ./producer & ./consumer &

3.) When the processes are finished (i.e., when the consumer prints the 
    final sum) press Ctrl+C to end the processes.
