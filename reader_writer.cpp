#include <bits/stdc++.h>
#include <semaphore.h>
#include <chrono>
#include <mutex>
using namespace std;

sem_t resourceAccess;
sem_t readCountAccess;
int readCount = 0;
mutex printMutex; // Mutex to synchronize output

void reader(int id) {
    while (true) {
        sem_wait(&readCountAccess);
        readCount++;
        if (readCount == 1) {
            sem_wait(&resourceAccess);
        }
        sem_post(&readCountAccess);

        {
            lock_guard<mutex> lock(printMutex);
            cout << "Reader " << id << " is reading." << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(500));

        sem_wait(&readCountAccess);
        readCount--;
        if (readCount == 0) {
            sem_post(&resourceAccess);
        }
        sem_post(&readCountAccess);

        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

void writer(int id) {
    while (true) {
        sem_wait(&resourceAccess);

        {
            lock_guard<mutex> lock(printMutex);
            cout << "Writer " << id << " is writing." << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(1000));

        sem_post(&resourceAccess);
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int main() {
    sem_init(&readCountAccess, 0, 1);
    sem_init(&resourceAccess, 0, 1);

    thread readers[3], writers[2];

    for (int i = 1; i <= 3; i++) {
        readers[i - 1] = thread(reader, i);
    }
    for (int i = 1; i <= 2; i++) {
        writers[i - 1] = thread(writer, i);
    }

    for (int i = 0; i < 3; i++) {
        readers[i].join();
    }
    for (int i = 0; i < 2; i++) {
        writers[i].join();
    }

    sem_destroy(&resourceAccess);
    sem_destroy(&readCountAccess);

    return 0;
}
