#include <iostream>
#include "Log.h"
#include <stdio.h>
#include <cmath>
#include "BHashMap.h"
#include "DLList.h"
#include "KmerQueue.h"

string uint8toBit(uint8_t c) {
    string s;
    for (int i = 0; i < 8; i++) {
        char text[1];
        sprintf(text, "%d", (c & (int)(pow(2,7-i))) >> 7-i);
        s += text;
    }
    return s;
}

void testMap () {
    Log("hallo");;

    BHashMap<23,17> map (10, 0.7);


// 11011110 00000011 1111101|0
    uint8_t key1[map.KEY_BYTES] = { 0xDE, 0x03, 0xFA};
    // 0000000|1 11111110 01110110
    uint8_t val1[map.VALUE_BYTES] = { 0x01, 0xFE, 0x76};

    // 10011010 00010010 0100001|0
    uint8_t key2[map.KEY_BYTES] = { 0x9A, 0x12, 0x42};
    // 0000000|0 10101110 00100011
    uint8_t val2[map.VALUE_BYTES] = { 0x00, 0xAE, 0x23};

    map.insertAt(3, (uint8_t *)key1, (uint8_t *)val1);
    map.insertAt(6, (uint8_t *)key2, (uint8_t *)val2);
    map.printMap();

    BHashMap<16,8> map2 (10, 0.7);

    // 10001010 10011011
    uint8_t key3[map2.KEY_BYTES] = { 0x8A, 0x9B};
    // 10101100
    uint8_t val3[map2.VALUE_BYTES] = { 0xAC };

    // 10011010 00010011
    uint8_t key4[map2.KEY_BYTES] = { 0x9A, 0x13 };
    // 01010011
    uint8_t val4[map2.VALUE_BYTES] = { 0x53 };

    map2.insertAt(3, (uint8_t *)key3, (uint8_t *)val3);
    map2.insertAt(6, (uint8_t *)key4, (uint8_t *)val4);
    map2.printMap();

    BHashMap<20,4> map3 (10, 0.7);

    // 01010011 01001011 0111
    uint8_t key5[map3.KEY_BYTES] = { 0x53, 0x4B, 0x70};
    // 00001111
    uint8_t val5[map3.VALUE_BYTES] = { 0x0F };

    // 10011010 00010011 0011
    uint8_t key6[map3.KEY_BYTES] = { 0x9A, 0x13, 0x20 };
    // 00001010
    uint8_t val6[map3.VALUE_BYTES] = { 0x0A };


    map3.insertAt(3, (uint8_t *)key5, (uint8_t *)val5);
    map3.insertAt(6, (uint8_t *)key6, (uint8_t *)val6);
    map3.printMap();
    map3.insertAt(3, (uint8_t *)key6,(uint8_t *) val6);
    map3.printMap();

    map.printMap();
    uint8_t returnVal[map.VALUE_BYTES];
    if (map.getAt(3, returnVal)) {
        cout << 3 << ": ";
        map.printArray(returnVal, map.VALUE_BYTES);
    }
    if (map.getAt(4, returnVal)) {
        cout << 3 << ": ";
        map.printArray(returnVal, map.VALUE_BYTES);
    }

    cout << "3: " << map.getAt(3) << endl;
    cout << "6: " << map.getAt(6) << endl;
    cout << "0: " << map.getAt(0) << endl;
}

void testMap2(int size) {
    BHashMap<23,17> map (10, 0.7);

    // 11011110 00000011 1111101|1 11111110 01110110
    uint8_t key1[map.KEY_BYTES] = { 0xDE, 0x03, 0xFA};
    uint8_t val1[map.VALUE_BYTES] = { 0x01, 0xFE, 0x76};

    // 10011010 00010010 0100001|0 10101110 00100011
    uint8_t key2[map.KEY_BYTES] = { 0x9A, 0x12, 0x42};
    uint8_t val2[map.VALUE_BYTES] = { 0x00, 0xAE, 0x23};


    uint8_t key3[map.KEY_BYTES] = { 0x9A, 0x12, 0xD8};
    uint8_t val3[map.VALUE_BYTES] = { 0x01, 0xFE, 0x3C};

    uint8_t key4[map.KEY_BYTES] = { 0xFF, 0xC2, 0x22};
    uint8_t val4[map.VALUE_BYTES] = { 0x00, 0xF0, 0x1A};

    uint8_t key5[map.KEY_BYTES] = { 0xFD, 0xC2, 0xD2};
    uint8_t val5[map.VALUE_BYTES] = { 0x00, 0xA0, 0x1A};


    uint8_t key6[map.KEY_BYTES] = { 0xAC, 0xDF, 0x22};


    map.printMap();

    cout << "put operation key1" << endl;
    map.put((uint8_t *)key1, (uint8_t *)val1);
    map.printMap();

    cout << "put operation key2" << endl;
    map.put((uint8_t *)key2, (uint8_t *)val2);
    map.printMap();

    cout << "put operation key3" << endl;
    map.put((uint8_t *)key3, (uint8_t *)val3);
    map.printMap();

    cout << "put operation key4" << endl;
    map.put((uint8_t *)key4, (uint8_t *)val4);
    map.printMap();

    cout << "put operation key4 val1" << endl;
    map.put((uint8_t *)key4, (uint8_t *)val1);
    map.printMap();

    cout << "put operation key5" << endl;
    map.put((uint8_t *)key5, (uint8_t *)val5);
    map.printMap();

    long value4 = map.get(key4);
    long value5 = map.get(key5);
    long value6 = map.get(key6);

    cout << "get key4: " << value4 << endl;
    cout << "get key5: " << value5 << endl;
    cout << "get key6: " << value6 << endl;
}

string toBitString(uint8_t * array, int len) {
    string res = "";
    for (int i = 0; i < len; i++) {
        res += uint8toBit(array[i]) + " ";
    }
    return res;
}

void testRobin() {
    BHashMap<25,7> map( 15 , 0.7 );

    // 11011110 00010001 10111010 10000000
    uint8_t key1[map.KEY_BYTES] = { 0xDE, 0x11, 0xBA, 0x80};

    // 11011111 11011101 00111010
    uint8_t key2[map.KEY_BYTES] = { 0xDF, 0xDD, 0x3A, 0x00};

    // 11011110 00001100 00110100 1000
    uint8_t key3[map.KEY_BYTES] = { 0xDE, 0x0C, 0x34, 0x80};

    // 00111110 10110101 01010110
    uint8_t key4[map.KEY_BYTES] = { 0x3E, 0xB5, 0x56, 0x00};

    // 11011110 10100011 10011000 10000000
    uint8_t key5[map.KEY_BYTES] = { 0xDE, 0xA3, 0x98, 0x80};

    // 11010101 11100011 11110011 0
    uint8_t key6[map.KEY_BYTES] = { 0xD5, 0xe3, 0xF3, 0x00};

    // 00001110 00100011 01101010 1000
    uint8_t key7[map.KEY_BYTES] = { 0x0E, 0x23, 0x6A, 0x80};


    uint8_t key8[map.KEY_BYTES] = { 0x4E, 0xc3, 0x77, 0x00};
    uint8_t key9[map.KEY_BYTES] = { 0x56, 0x03, 0x32, 0x80};
    uint8_t key10[map.KEY_BYTES] = { 0x5E, 0x13, 0x1A, 0x00};

    uint8_t value1[map.VALUE_BYTES] = { 0x01 };
    uint8_t value2[map.VALUE_BYTES] = { 0x02 };
    uint8_t value3[map.VALUE_BYTES] = { 0x03 };
    uint8_t value4[map.VALUE_BYTES] = { 0x04 };
    uint8_t value5[map.VALUE_BYTES] = { 0x05 };
    uint8_t value6[map.VALUE_BYTES] = { 0x06 };
    uint8_t value7[map.VALUE_BYTES] = { 0x07 };
    uint8_t value8[map.VALUE_BYTES] = { 0x08 };
    uint8_t value9[map.VALUE_BYTES] = { 0x09 };
    uint8_t value10[map.VALUE_BYTES] = { 0x0A};

    string mystring;
    cout << "key1 ";
    map.put(key1, value1);
    //map.printMap();
    //cin >> mystring;

    cout << "key2 ";
    map.put(key2, value2);
    //map.printMap();
    //cin >> mystring;

    cout << "key3 ";
    map.put(key3, value3);
    //map.printMap();
    //cin >> mystring;

    cout << "key4 ";
    map.put(key4, value4);
    //map.printMap();
    //cin >> mystring;

    cout << "key5 ";
    map.put(key5, value5);
    //map.printMap();
    //cin >> mystring;

    cout << "key6 ";
    map.put(key6, value6);
    map.printMap();
    cin >> mystring;

    cout << "key7 " << toBitString(key7 ,3) << " ";
    map.put(key7, value7);
    map.printMap();
    cin >> mystring;

    cout << "key8 " << toBitString(key8 ,3) << " ";
    map.put(key8, value8);
    map.printMap();
    cin >> mystring;

    cout << "key9 " << toBitString(key9 ,3) << " ";
    map.put(key9, value9);
    map.printMap();
    cin >> mystring;

    cout << "key10 " << toBitString(key10 ,3) << " ";
    map.put(key10, value10);
    map.printMap();
    cin >> mystring;

    map.printMap();
}

void testRobin2() {
    BHashMap<25,7> map( 10 , 0.7 );

    // 11011110 00010001 10111010 10000000
    uint8_t key1[map.KEY_BYTES] = { 0xDE, 0x11, 0xBA, 0x80};

    // 11011111 11011101 00111010
    uint8_t key2[map.KEY_BYTES] = { 0xDF, 0xDD, 0x3A, 0x00};

    // 11011110 00001100 00110100 1000
    uint8_t key3[map.KEY_BYTES] = { 0xDE, 0x0C, 0x34, 0x80};

    // 00111110 10110101 01010110
    uint8_t key4[map.KEY_BYTES] = { 0x3E, 0xB5, 0x56, 0x00};

    // 11011110 10100011 10011000 10000000
    uint8_t key5[map.KEY_BYTES] = { 0xDE, 0xA3, 0x98, 0x80};

    // 11010101 11100011 11110011 0
    uint8_t key6[map.KEY_BYTES] = { 0xD5, 0xe3, 0xF3, 0x00};

    // 00001110 00100011 01101010 1000
    uint8_t key7[map.KEY_BYTES] = { 0x0E, 0x23, 0x6A, 0x80};


    uint8_t key8[map.KEY_BYTES] = { 0x4E, 0xc3, 0x77, 0x00};
    uint8_t key9[map.KEY_BYTES] = { 0x56, 0x03, 0x32, 0x80};
    uint8_t key10[map.KEY_BYTES] = { 0x5E, 0x13, 0x1A, 0x00};

    uint8_t value1[map.VALUE_BYTES] = { 0x01 };
    uint8_t value2[map.VALUE_BYTES] = { 0x02 };
    uint8_t value3[map.VALUE_BYTES] = { 0x03 };
    uint8_t value4[map.VALUE_BYTES] = { 0x04 };
    uint8_t value5[map.VALUE_BYTES] = { 0x05 };
    uint8_t value6[map.VALUE_BYTES] = { 0x06 };
    uint8_t value7[map.VALUE_BYTES] = { 0x07 };
    uint8_t value8[map.VALUE_BYTES] = { 0x08 };
    uint8_t value9[map.VALUE_BYTES] = { 0x09 };
    uint8_t value10[map.VALUE_BYTES] = { 0x0A};

    string mystring;
    cout << "key1 ";
    map.put(key1, value1);
    map.printMap();
    cin >> mystring;

    cout << "key2 ";
    map.put(key2, value2);
    //map.printMap();
    //cin >> mystring;

    cout << "key3 ";
    map.put(key3, value3);
    //map.printMap();
    //cin >> mystring;

    cout << "key4 ";
    map.put(key4, value4);
    //map.printMap();
    //cin >> mystring;

    cout << "key5 ";
    map.put(key5, value5);
    //map.printMap();
    //cin >> mystring;

    cout << "key6 ";
    map.put(key6, value6);
    map.printMap();
    //cin >> mystring;

    cout << "key7 " << toBitString(key7 ,3) << " ";
    map.put(key7, value7);
    map.printMap();
    //cin >> mystring;

    cout << "key8 " << toBitString(key8 ,3) << " ";
    map.put(key8, value8);
    map.printMap();
    cin >> mystring;

    cout << "key9 " << toBitString(key9 ,3) << " ";
    map.put(key9, value9);
    map.printMap();
    cin >> mystring;

    cout << "key10 " << toBitString(key10 ,3) << " ";
    map.put(key10, value10);
    map.printMap();
    cin >> mystring;

    map.printMap();
}

void dllTest() {
    DLList<int> list;

    list.print();

    list.insert(4);
    list.print();

    list.insert(10);
    list.print();

    list.insert(5);
    list.print();

    list.insert(1);
    list.print();

    list.remove(4);
    list.print();

    list.remove(7);
    list.print();

    list.remove(1);
    list.print();

    list.remove(10);
    list.print();

    list.remove(5);

    list.insert(100);
    list.print();

    list.insert(110);
    list.print();

    list.insert(111);
    list.print();


    cout << "110: " << list.contains(110) << endl;
    cout << "0: " << list.contains(0) << endl;
    cout << "100: " << list.contains(100) << endl;
    cout << "120: " << list.contains(120) << endl;

    list.insert(115);
    list.print();

    list.removeLT(105);
    list.print();

    list.insert(123);
    list.insert(101);
    list.insert(104);
    list.print();

    list.removeLT(124);
    list.print();


}

void printArray(uint8_t * ary, int len) {
    for (int i = 0; i < len; i++) {
        cout << ary[i];
    }
    cout << endl;
}

void queueTest() {
    KmerQueue<3> kmerq;

    kmerq.print();

    uint8_t kmer1[3] = { 'a', 'a', 'z'};
    uint8_t kmer2[3] = { 'b', 'b', 'z'};
    uint8_t kmer3[3] = { 'c', 'c', 'z'};

    kmerq.push(kmer1);
    kmerq.print();
    kmerq.push(kmer2);
    kmerq.print();
    kmerq.push(kmer3);
    kmerq.print();

    uint8_t target[3];

    kmerq.pop(target);
    kmerq.print();
    printArray(target, 3);


    kmerq.pop(target);
    kmerq.print();
    printArray(target, 3);

    kmerq.pop(target);
    kmerq.print();
    printArray(target, 3);

}

int main() {
    int size = 10;

    //testMap2(size);

    //testRobin();
    //dllTest();
    //queueTest();

    testRobin2();

    std::cout << "Hello, World!" << std::endl;
    return 0;


}
