/*
这是一个关于函数指针做成员变量的使用示例
*/

#include <stddef.h>
#include <iostream>
using namespace std;
typedef    unsigned int (*hashFunction)(const void *key);
typedef    void *(*keyDup)(void *privdata, const void *key);
typedef    void *(*valDup)(void *privdata, const void *obj);
typedef    int (*keyCompare)(void *privdata, const void *key1, const void *key2);
typedef    void (*keyDestructor)(void *privdata, void *key);
typedef    void (*valDestructor)(void *privdata, void *obj);

class HashType {
public:
    HashType(hashFunction f1, keyDup f2, valDup f3, keyCompare f4, keyDestructor f5, valDestructor f6){
        mHashFunction = f1;
        mKeyDup = f2;
        mValDup = f3;
        mKeyCompare = f4;
        mKeyDestructor = f5;
        mValDestructor = f6;
    }
    hashFunction mHashFunction;
    keyDup mKeyDup;
    valDup mValDup;
    keyCompare mKeyCompare;
    keyDestructor mKeyDestructor;
    valDestructor mValDestructor;
} ;

unsigned int f1(const void *key){
    cout << "key is " << (unsigned int)key << endl;
    return  (unsigned int)key;
}


int main(){
    HashType test(f1, NULL, NULL, NULL, NULL, NULL);
    test.mHashFunction((void*)1314);
}
