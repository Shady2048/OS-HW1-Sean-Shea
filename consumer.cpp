// Sean Shea
// OS HW1 
// 10/23/25
// consumer.cpp

#include <iostream>
#include <semaphore.h>
#include <fcntl.h>
#include <thread>
#include <chrono>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string>
#include <ctime>
#include <random>


int main() {

    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Open Semaphore and Shared Memory
    sem_t *my_sem = sem_open("/prod_sem", 0);
    if (my_sem == SEM_FAILED) {
        std::cout << "The Sem Failed Mate" << std::endl; 
        return 1;
    }

    int SM = shm_open("/my_shared_memory", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (SM == -1) {
        std::cout << "The Shared Memory Failed :(" << std::endl;
        return 1;
    }

    int size_of[2];

    void* SMmap = mmap(NULL, sizeof(size_of), PROT_READ | PROT_WRITE, MAP_SHARED, SM, 0);
    if (SMmap == MAP_FAILED) {
        std::cout << "The Map Failed" << std::endl;
        return 1;
    }

    int* sharedArray = static_cast<int*>(SMmap);

    int sum = 0;    // a sum of ints that will be added to from the producer process

    for(int i = 0; i < 3; i++) {
        
        std::cout << "[C]: waiting" << std::endl;
        sem_wait(my_sem);
        // critical section goes here

        std::cout << "[C]: Entered Critical Section " << std::endl;

        for (int i = 0; i < 2; i++) {
            if (sharedArray[i] != -1) { //if there is something in the shared array to consume...
                sum += sharedArray[i];  //consume the value (add it to sum),
                std::cout << "[C]: consumed value " << sharedArray[i] << ", sum is now " << sum << std::endl;
                sharedArray[i] = -1;    //then change the value to -1 to denote that it's empty.
            }
        }


        sem_post(my_sem);
        std::cout << "[C]: Exited Critical Section " << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    
    std::cout << std::endl << "[C]: Final Sum = " << sum << std::endl;

    // unlink semaphores and Shared Memory
    if(munmap(SMmap, sizeof(size_of)) == -1) {
        std::cout << "munmap didn't work" << std::endl;
        return 1;
    }

    if (close(SM) == -1) {
        std::cout << "Not Closed Properly" << std::endl;
        return 1;
    }


    sem_close(my_sem); // closes semaphore and deletes it

    return 0;
}