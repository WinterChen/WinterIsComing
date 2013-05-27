/*
this is a example for using function point as class member

compile in linux: g++ -o functionPoint functionPoint.cc
*/

#include <stddef.h> //head file of NULL
#include <iostream>
using namespace std;

//function point
typedef    unsigned int (*HashFunction)(const void *key);
typedef    void *(*KeyDup)(void *privdata, const void *key);
typedef    void *(*ValDup)(void *privdata, const void *obj);
typedef    int (*KeyCompare)(void *privdata, const void *key1, const void *key2);
typedef    void (*KeyDestructor)(void *privdata, void *key);
typedef    void (*ValDestructor)(void *privdata, void *obj);



class HashType {
public:
    HashType(HashFunction f1, KeyDup f2, ValDup f3, KeyCompare f4, KeyDestructor f5, ValDestructor f6){
        mHashFunction = f1;
        mKeyDup = f2;
        mValDup = f3;
        mKeyCompare = f4;
        mKeyDestructor = f5;
        mValDestructor = f6;
    }
    HashFunction mHashFunction;
    KeyDup mKeyDup;
    ValDup mValDup;
    KeyCompare mKeyCompare;
    KeyDestructor mKeyDestructor;
    ValDestructor mValDestructor;
} ;

unsigned int f1(const void *key){
    cout << "key is " << (unsigned int)key << endl;
    return  (unsigned int)key;
}


int main(){
    HashType test(f1, NULL, NULL, NULL, NULL, NULL);
    test.mHashFunction((void*)1314);
}
