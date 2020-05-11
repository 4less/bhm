//
// Created by joachim on 06/05/2020.
//

#ifndef BHM_DLLIST_H
#define BHM_DLLIST_H

#include <lzma.h>
#include <cstdint>
#include <iostream>

using namespace std;

template<typename T>
struct Node {
    T data;
    struct Node * prev;
    struct Node * next;
};


template<typename T>
class DLList {
private:
    struct Node<T> * head = nullptr;
    struct Node<T> * tail = nullptr;

public:
    void insert(T data);
    void remove(T data);
    bool contains(T data);
    void removeLT(T data);
    //T popL(T data);
    //T popF(T data);
    //T peekL(T data);
    //T peekF(T data);

    void print();
};


template<class T>
void DLList<T>::insert(T data) {
    cout << "insert: " << data << endl;

    auto node = (Node<T>*)malloc(sizeof(Node<T>));
    node->data = data;

    if (!head) {
        head = node;
        tail = node;
        node->next = nullptr;
        node->prev = nullptr;
    } else {

        Node<T> * nptr;
        nptr = head;
        while (nptr) {
            if (node->data < nptr->data) {
                if (!nptr->prev) {
                    // append at front
                    head = node;
                    node->prev = nullptr;
                } else {
                    // append in between
                    nptr->prev->next = node;
                    node->prev = nptr->prev;
                }
                node->next = nptr;
                nptr->prev = node;
                return;
            }
            nptr = nptr->next;
        }
        // append at the end
        tail->next = node;
        node->prev = tail;
        node->next = nullptr;
        tail = node;

    }
}

template<class T>
void DLList<T>::remove(T data) {
    Node<T> * nptr = head;

    // iterate through DLL till end or data found
    while (nptr && nptr->data != data) {
        nptr = nptr->next;
    };

    // no matching element found
    if (!nptr) return;


    if (head == tail) {
        head = nullptr;
        tail = nullptr;
        free(nptr);
        return;
    }
    // If nptr is tail
    if (!nptr->next) {
        nptr->prev->next = nullptr;
        tail = nptr->prev;
        free(nptr);
        return;
    }
    // If nptr is head
    if (!nptr->prev) {
        nptr->next->prev = nullptr;
        head = nptr->next;
        free(nptr);
        return;
    }

    nptr->next->prev = nptr->prev;
    nptr->prev->next = nptr->next;
    free(nptr);
}

template<class T>
void DLList<T>::print() {
    cout << "head--> ";
    Node<T> * nptr = head;

    int count = 0;

    while (nptr) {
        cout << " " << nptr->data << " ";
        nptr = nptr->next;
        if (count++ > 20) return;
    }

    cout << " <--tail" << endl;
}

template<typename T>
bool DLList<T>::contains(T data) {
    Node<T> * node = head;

    while (node && node->data <= data) {
        if (node->data == data)
            return true;
        node = node->next;
    }
    return false;
}

template<typename T>
void DLList<T>::removeLT(T data) {
    Node<T> * temp = head;

    while (temp && temp->data < data) {
        head = temp->next;
        free(temp);
        temp = head;
    }
    if (!head) tail = nullptr;
    else head->prev = nullptr;
}


#endif //BHM_DLLIST_H
