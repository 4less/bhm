//
// Created by joachim on 27/04/2020.
//
#ifndef BHM_BHASHMAP_H
#define BHM_BHASHMAP_H


#include <cmath>
#include "cstdint"
#include "DLList.h"
#include "KmerQueue.h"

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif


using namespace std;


class HashFunction {
public:
    HashFunction() {};
    virtual uint32_t operator() (const uint8_t * data, unsigned int byte) = 0;
};


class SuperFastHash : public HashFunction { ;
    uint32_t operator()(const uint8_t *data, unsigned int byte) override {

    uint32_t hash = byte, tmp;
    unsigned int rem;


    if (byte <= 0 || data == nullptr) return 0;

    rem = byte & 3u;
    byte >>= 2u;


    //Main loop
    for (;byte > 0; byte--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }


    // Handle end cases
    switch (rem) {
        case 3: hash += get16bits (data);
            hash ^= hash << 16;
            hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
            hash += hash >> 11;
            break;
        case 2: hash += get16bits (data);
            hash ^= hash << 11;
            hash += hash >> 17;
            break;
        case 1: hash += (signed char)*data;
            hash ^= hash << 10;
            hash += hash >> 1;
    }

    // Force "avalanching" of final 127 bits
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}
public:
    SuperFastHash() {}
};

//template <int KB, int VB>
template <int KeyBits, int ValueBits>
class BHashMap {
public:
    static const int KEY_BYTES = (KeyBits % 8) == 0 ? KeyBits / 8 : KeyBits / 8 + 1;
    static const int VALUE_BYTES = (ValueBits % 8) == 0 ? ValueBits / 8 : ValueBits / 8 + 1;

private:
    int n;
    static const int OVERLAP_BYTES = (KeyBits % 8) == 0 ? 0 : 1;
    static const bool HAS_OVERLAP = OVERLAP_BYTES == 1 ? true : false;
    static const int OVERLAP_POS = HAS_OVERLAP ? KEY_BYTES - 1 : -1;
    static const int M = KEY_BYTES + VALUE_BYTES - OVERLAP_BYTES;


    uint8_t keyMask;
    uint8_t valMask;
    int valueMask;

    HashFunction * hash;

    int load = 0;
    double loadFactor;
    int loadThreshold;
    double by = 0.5;

    // Pointer to array
    uint8_t * byte;

    bool isKeySame(uint8_t * kv1, uint8_t * kv2);
    bool isEmpty(int pos);
    void resize(double by);
    void initArray(int n);
    int psl(uint8_t * kv, int current);
    void resizeInsert(int index, int oldN, KmerQueue<M> queue, DLList<int> processed);

public:
    BHashMap<KeyBits, ValueBits>(int n, HashFunction * hash);
    BHashMap<KeyBits, ValueBits>(int n, double loadFactor);

    uint8_t * search(uint8_t * key);

    // Extract key and value, respectively, from key-value entry to separate array
    void getKey(uint8_t (&kv)[M], uint8_t (&to)[KEY_BYTES]);
    void getKeyP(uint8_t * kv, uint8_t (&to)[KEY_BYTES]);
    void getValue(uint8_t (&kv)[M], uint8_t (&to)[VALUE_BYTES]);
    void getValueP(uint8_t * kv, uint8_t (&to)[VALUE_BYTES]);

    // Put/Insert operations
    // Insert key and value at position
    void insertAt(int pos, uint8_t (&key)[KEY_BYTES], uint8_t (&value)[VALUE_BYTES]);
    void insertAt(int pos, uint8_t * kv);
    void insertAt(int pos, uint8_t * key, uint8_t * value);
    //void put(uint8_t (&key)[KEY_BYTES], uint8_t (&value)[VALUE_BYTES]);
    void put(uint8_t * key, uint8_t * value);


    // Get/GetAt Operations
    bool getAt(int pos, uint8_t (&to)[VALUE_BYTES]);
    uint64_t getAt(int pos);
    bool get(uint8_t (&key)[KEY_BYTES], uint8_t (&to)[VALUE_BYTES] );
    uint64_t get(uint8_t (&key)[KEY_BYTES]);

    // Debug and visualization methods
    void printMap();
    static string printBit(uint8_t c) {
        string s;
        for (int i = 0; i < 8; i++) {
            char text[1];
            sprintf(text, "%d", (c & (int)(pow(2,7-i))) >> 7-i);
            s += text;
        }
        return s;
    }

    static void printArray(uint8_t * ary, int len) {
        for (int i = 0; i < len; i++) {
            cout << printBit(ary[i]) << " ";
        }
        cout << endl;
    }
};


template <int KeyBits, int ValueBits>
BHashMap<KeyBits, ValueBits>::BHashMap(int size, HashFunction *hash) {
    //BHashMap(size);
    this->hash = hash;
}

template <int KeyBits, int ValueBits>
BHashMap<KeyBits, ValueBits>::BHashMap(int n, double loadFactor) {
    this->n = n;
    this->loadFactor = loadFactor;
    this->loadThreshold = n * loadFactor;

    // use before assigning memory of hashmap
    this->hash = (HashFunction *) malloc(sizeof(SuperFastHash));
    new(this->hash) SuperFastHash;

    cout << "!byte  " << (uint32_t*)byte << endl;
    this->byte = (uint8_t *) malloc(n * M);
    cout << "bytemal " << (uint32_t*)byte << endl;

    //this->hash = new SuperFastHash();

    int remainder = KeyBits % 8;

    this->keyMask = (KeyBits % 8) == 0 ? 0xFF : 0xFF - (pow(2, (8 - remainder)) - 1);
    this->valMask = keyMask ^ 0xFF;


    uint8_t m[4] = { 0 };
    for (int i = 0; i < M-KEY_BYTES; i++) {
        m[i] = 0xff;
    }
    m[M-KEY_BYTES] |= valMask;
    this->valueMask = *((uint32_t*) &m);
    cout << "value int keyMask: " << this->valueMask << endl;

    cout << "keyMask: " << (int)keyMask << endl;
}



template <int KeyBits, int ValueBits>
bool BHashMap<KeyBits, ValueBits>::isKeySame(uint8_t *kv1, uint8_t *kv2) {
    int i = 0;
    for (; i < KEY_BYTES-1; i++) {
        if (kv1[i] != kv2[i]) {
            return false;
        }
        if ((kv1[i] & keyMask) != kv2[i] & keyMask) {
            return false;
        }
    }
    return true;
}

template <int KeyBits, int ValueBits>
bool BHashMap<KeyBits, ValueBits>::isEmpty(int pos) {

    for (int i = M-1; i > KEY_BYTES - 1; i--) {
        if (byte[(pos*M)+i] != 0x00) return(false);
    }

    return (!HAS_OVERLAP ? true : ((byte[(pos * M) + OVERLAP_POS] & valMask) == 0x00));
}



template <int KeyBits, int ValueBits>
//template<int M, int KB>
void BHashMap<KeyBits, ValueBits>::put(uint8_t * key, uint8_t * value) {
    int32_t h = ((*hash)(key, KEY_BYTES) % n);
    cout << "original hash " << h << " ----- ";
    //cout << "is_empty: " << (bool)(isEmpty(h)) << endl;
    while (!isEmpty(h)) {
        cout << psl(key,h) << " - curpsl: " << psl(byte+(h*M),h) << endl;
        if (isKeySame(key, (byte+(h*M)))) {
            cout << "update at: " << h << endl;
            insertAt(h, key, value);
            return;
        } else if (psl(key, h) > psl(byte+(h*M), h)) {
            cout << "robin at: " << h << endl;
            uint8_t newkey[KEY_BYTES];
            uint8_t newval[VALUE_BYTES];
            getKeyP(byte+(h*M), newkey);
            getValueP(byte+(h*M), newval);
            insertAt(h, key,value);
            key = (uint8_t *) newkey;
            value = (uint8_t *) newval;
        }


        h = (h+1) % n;
    }
    cout << "insert at: " << h << endl;

    insertAt(h, key, value);
    if (++load >= loadThreshold) {
        resize(this->by);
    }
}

template <int KeyBits, int ValueBits>
uint8_t *BHashMap<KeyBits, ValueBits>::search(uint8_t *key) {
    return nullptr;
}

template <int KeyBits, int ValueBits>
void  BHashMap<KeyBits, ValueBits>::getKey(uint8_t (&kv)[M], uint8_t (&to)[KEY_BYTES]) {
    for (int i = 0; i < KEY_BYTES - 1; i++) {
        to[i] = kv[i];
    }
    to[KEY_BYTES - 1] = kv[KEY_BYTES - 1] & keyMask;
}

template <int KeyBits, int ValueBits>
void  BHashMap<KeyBits, ValueBits>::getKeyP(uint8_t * kv, uint8_t (&to)[KEY_BYTES]) {
    for (int i = 0; i < KEY_BYTES - 1; i++) {
        to[i] = kv[i];
    }
    to[KEY_BYTES - 1] = kv[KEY_BYTES - 1] & keyMask;
}

template <int KeyBits, int ValueBits>
void  BHashMap<KeyBits, ValueBits>::getValue(uint8_t (&kv)[M], uint8_t (&to)[VALUE_BYTES]) {
    for (int i = KEY_BYTES; i < M; i++) {
        to[i-KEY_BYTES+OVERLAP_BYTES] = kv[i];
    }
    if (HAS_OVERLAP)
        to[0] = kv[OVERLAP_POS] & valMask;
}
template <int KeyBits, int ValueBits>
void  BHashMap<KeyBits, ValueBits>::getValueP(uint8_t * kv, uint8_t (&to)[VALUE_BYTES]) {
    for (int i = KEY_BYTES; i < M; i++) {
        to[i-KEY_BYTES+OVERLAP_BYTES] = kv[i];
    }
    if (HAS_OVERLAP)
        to[0] = kv[OVERLAP_POS] & valMask;
}


template<int KeyBits, int ValueBits>
void BHashMap<KeyBits, ValueBits>::printMap() {
    for (int i = 0; i < n; i++) {
        cout << i << ": ";
        for (int j = 0; j < M; j++) {
            cout << printBit(byte[(M*i)+j]) << " ";
        }
        cout << endl;
    }
}

template<int KeyBits, int ValueBits>
void BHashMap<KeyBits, ValueBits>::insertAt(int pos, uint8_t (&k)[KEY_BYTES], uint8_t (&v)[VALUE_BYTES]) {
    for (int i = 0; i < KEY_BYTES; i++) {
        byte[(pos*M)+i] = k[i];
    }
    if (HAS_OVERLAP) {
        byte[(pos*M)+OVERLAP_POS] |= v[0];
    }
    for (int j = OVERLAP_BYTES; j < VALUE_BYTES; j++) {
        byte[(pos*M)+KEY_BYTES - OVERLAP_BYTES + j] = v[j];
    }
}

template<int KeyBits, int ValueBits>
void BHashMap<KeyBits, ValueBits>::insertAt(int pos, uint8_t * key, uint8_t * value) {
    for (int i = 0; i < KEY_BYTES; i++) {
        byte[(pos*M)+i] = key[i];
    }
    if (HAS_OVERLAP) {
        byte[(pos*M)+OVERLAP_POS] |= value[0];
    }
    for (int j = OVERLAP_BYTES; j < VALUE_BYTES; j++) {
        byte[(pos*M)+KEY_BYTES - OVERLAP_BYTES + j] = value[j];
    }
}


template<int KeyBits, int ValueBits>
void BHashMap<KeyBits, ValueBits>::insertAt(int pos, uint8_t *kv) {
    for (int i = 0; i < M; i++)
        byte[(pos*M)+i] = kv[i];
}


template<int KeyBits, int ValueBits>
bool BHashMap<KeyBits, ValueBits>::getAt(int pos, uint8_t (&to)[VALUE_BYTES]) {
    if (isEmpty(pos)) return false;
    else
        getValue(*((uint8_t (*)[M])byte[pos*M]), to);
    return true;
}

template<int KeyBits, int ValueBits>
uint64_t BHashMap<KeyBits, ValueBits>::getAt(int pos) {
    uint8_t longA[sizeof(uint64_t)] = { 0 };

    for (int i = M - 1; i >= KEY_BYTES; i--) {
        longA[M - 1 - i] = byte[(pos*M)+i];
    }
    if (HAS_OVERLAP) {
        longA[VALUE_BYTES-1] = valMask & byte[(pos*M)+OVERLAP_POS];
    }
    return *((long *) longA);
}


template<int KeyBits, int ValueBits>
bool BHashMap<KeyBits, ValueBits>::get(uint8_t (&key)[KEY_BYTES], uint8_t (&to)[VALUE_BYTES]) {
    return false;
}

template<int KeyBits, int ValueBits>
uint64_t BHashMap<KeyBits, ValueBits>::get(uint8_t (&key)[KEY_BYTES]) {
    int32_t h = ((*hash)(key, KEY_BYTES) % n);

    while (!isEmpty(h)) {
        if (isKeySame(key, (byte+(h*M)))) {
            return getAt(h);
        }
        h = (h+1) % n;
    }
    return -1;
}

template<int KeyBits, int ValueBits>
int BHashMap<KeyBits, ValueBits>::psl(uint8_t * kv, int current) {
    uint8_t key[KEY_BYTES];
    getKeyP(kv, key);
    int32_t h = ((*hash)(key, KEY_BYTES) % n);
    return current - h;
}

template<int KeyBits, int ValueBits>
void BHashMap<KeyBits, ValueBits>::resize(double by) {
    cout << "############### resize ######################";


    int oldN = n;
    n += n * by;

    loadThreshold = loadFactor * n;


    cout << "first: " << byte << endl;
    cout << "last: " << (byte+(n*M)) << endl;
    cout << "hash: " << hash << endl;


    cout << "byte " << (uint32_t*)byte << endl;
    uint8_t * tmp = (uint8_t *) realloc(this->byte, n);
    cout << "tmp " << (uint32_t*)tmp << endl;

    if (tmp) {
        cout << "success !!!!!!!!!!!!!!!!!!!!" << endl;
        byte = tmp;
        for (int i = (oldN * M); i < (n * M); i++) {
            byte[i] = 0x00;
        }
    }
    loadThreshold = loadFactor * n;

    cout << "li " << (n*M) << endl;
    cout << "first " << (uint32_t*)byte << endl;
    cout << "last: " << (uint32_t*)(byte+(n*M)) << endl;
    cout << "hash: " << hash << endl;

    printMap();


    DLList<int> processed;
    KmerQueue<M> queue;

    int index = 0;
    while (isEmpty(index))
        index++;



    resizeInsert(index, oldN, queue, processed);
}

template<int KeyBits, int ValueBits>
void BHashMap<KeyBits, ValueBits>::resizeInsert(int index, int oldN, KmerQueue<M> queue, DLList<int> processed) {
    cout << "############################################################reinsert: " << index << endl;

    cout << "beginning "<< endl;
    printMap();

    string interrupt;

    uint8_t kv[M];
    uint8_t keyT[KEY_BYTES];
    uint8_t valueT[VALUE_BYTES];



    // in case of Robin
    uint8_t newkey[KEY_BYTES];
    uint8_t newval[VALUE_BYTES];

    processed.removeLT(index);

    if (!queue.isEmpty()) {
        cout << "poppy" << endl;
        queue.pop(kv);
    } else {
        while (isEmpty(index) || processed.contains(index))
            index++;
        cout << "index: " << index << endl;
        cout << "oldN: " << oldN << endl;
        if (index >= oldN) return;

        // Copy entry and clear slot in byte
        cout << "copy and clear slot: " << index << endl;
        for (int i = 0; i < M; i++) {
            kv[i] = byte[ (index * M) + i ] ;
            byte[ (index * M) + i ] = 0x00;
        }
    }



    // extract key and value
    getKey(kv, keyT);
    getValue(kv, valueT);

    uint8_t * key = (uint8_t *) keyT;
    uint8_t * value = (uint8_t *) valueT;

    cout << "key " << endl;
    printArray(key, KEY_BYTES);

    // get pos in hashmap
    int32_t h = ((*hash)(key, KEY_BYTES)) % n;

    cout << "before ahilw copy/poppy" << endl;
    printMap();

    while (!isEmpty(h)) {
        // if not processed slot is considered empty. queue entry up for processing
        if (!processed.contains(h) && h >= index) {
            cout << "pushy" << endl;
            queue.push(byte + (h * M) );
            break;
        }

        cout << psl(key,h) << " - curpsl: " << psl(byte+(h*M),h) << endl;

        if (psl(key, h) > psl(byte+(h*M), h)) {
            cout << "robin at: " << h << endl;
            processed.insert(h);
            getKeyP(byte + (h * M), newkey);
            getValueP(byte + (h * M), newval);
            insertAt(h, key, value);
            key = (uint8_t *) newkey;
            value = (uint8_t *) newval;
        }

        h = (h+1) % n;
    }

    cout << "after awhile " << h << endl;
    printMap();

    cout << "reinserted at: " << h << endl;

    insertAt(h, (uint8_t *)key, (uint8_t *)value);
    processed.insert(h);

    this->printMap();
    cout << "queue ";
    queue.print();
    cout << "after awhile " << h << endl;
    printMap();
    cout << "processed items ";
    processed.print();

    cout << "after processed " << h << endl;
    printMap();

    cin >> interrupt;

    cout << "address " << &interrupt << endl;

    cout << "end " << h << endl;
    printMap();
    resizeInsert(++index, oldN, queue, processed);
}

#endif //BHM_BHASHMAP_H
