//
// Created by joachim on 06/05/2020.
//

#ifndef BHM_KMERQUEUE_H
#define BHM_KMERQUEUE_H

#include <lzma.h>
#include <cstdint>
#include <cstdlib>
#include <iostream>


using namespace std;

template<int M>
struct Kmer {
    uint8_t kv[M];
    struct Kmer * next = nullptr;
};

template<int M>
class KmerQueue {
private:
    Kmer<M> * first = nullptr;
    Kmer<M> * last = nullptr;

public:
    void push(uint8_t * kv);
    void pop(uint8_t (&target)[M]);
    void print();
    bool isEmpty();
};

template<int M>
void KmerQueue<M>::push(uint8_t *kv) {
    Kmer<M> * kmer = (Kmer<M>*)malloc(sizeof(Kmer<M>));

    for (int i = 0; i < M; i++)
        kmer->kv[i] = kv[i];

    if (!first) {
        first = kmer;
    }
    else {
        last->next = kmer;
    }
    last = kmer;
}

template<int M>
void KmerQueue<M>::pop(uint8_t (&target)[M]) {
    for (int i = 0; i < M; i++)
        target[i] = first->kv[i];
    Kmer<M> * temp = first;
    if (first != last)
        first = first->next;
    else {
        first = last = nullptr;
    }
    free(temp);
}

template<int M>
void KmerQueue<M>::print() {
    Kmer<M> * kmerptr = first;

    cout << "first--> ";
    while (kmerptr) {
        cout << " ";
        for (int i = 0; i < M; i++) {
            cout << kmerptr->kv[i];
        }
        cout << " ";
        kmerptr = kmerptr->next;
    }
    cout << " <--last" << endl;
}

template<int M>
bool KmerQueue<M>::isEmpty() {
    return !first;
}


#endif //BHM_KMERQUEUE_H
