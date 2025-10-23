// Sean Shea
// OS HW1 
// 10/23/25
// producer.cpp

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
    srand(time(nullptr));

    // creates a semaphore to be shared and shared memory 
    sem_t *my_sem = sem_open("/prod_sem", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);

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

    if( ftruncate(SM, sizeof(size_of)) == -1) {
        std::cout << "ftruncate didn't work" << std::endl;
        return 1;
    }

    void* SMmap = mmap(NULL, sizeof(size_of), PROT_READ | PROT_WRITE, MAP_SHARED, SM, 0);
    if (SMmap == MAP_FAILED) {
        std::cout << "The Map Failed" << std::endl;
        return 1;
    }

    int* sharedArray = static_cast<int*>(SMmap);

    int prodArray[2] = {-1,-1};
    bool setup = false;
    
    for(int i = 0; i < 3; i++) {

        // generates two numbers to the producer's array    
        for(int i = 0; i < 2; i++) {
            prodArray[i] = rand() % 100 + 1;
            std::cout << "[P]: Generated value " << prodArray[i] << std::endl;
        }

        std::cout << "[P]: waiting" << std::endl;
        
        sem_wait(my_sem);
        // critical section goes here

        std::cout << "[P]: Entered Critical Section" << std::endl;
        
        if(setup == false) {    //only run th first time the producer acceses the shared array, to set all array values to -1 to denote they're empty.
            for(int i = 0; i < 2; i++) {
                sharedArray[i] = -1;
            }
            std::cout << "[P]: Setup Shared Array." << std::endl;
            setup = true;
        }

        for(int i = 0; i < 2; i++) {
            
            if (sharedArray[i] == -1) {
                sharedArray[i] = prodArray[i];
                std::cout << "[P]: put value " << prodArray[i] << " in Shared Memory" << std::endl;
            }
        }
        
        sem_post(my_sem);
        std::cout << "[P]: Exited Critical Section" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(3));

    }


    sem_close(my_sem); // closes semaphore and deletes it
    sem_unlink("/prod_sem");

    if(munmap(SMmap, sizeof(size_of)) == -1) {
        std::cout << "munmap didn't work" << std::endl;
        return 1;
    }

    if (close(SM) == -1) {
        std::cout << "Not Closed Properly" << std::endl;
        return 1;
    }

    if (shm_unlink("/my_shared_memory") == -1) {
        std::cout << "Unlink Failed" << std::endl;
    }

    // std::cout << "unlinked properly" << std::endl;
    return 0;
}