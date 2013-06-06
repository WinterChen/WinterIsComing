




#include "winterHash.h"

#define MAXRATE 2

void
WinterHashTable::TableInit(){
    

}


//when insert
/* 1)  is the key exists in the hash table?
             if yes return  HASH_EXISTS
             else goto 2)
     2) is it rehashing?
             if yes, we should insert the key into the mHashTable[1]
             else goto 3)
     3) insert the key and value into the mHashTable[0]



*/
int 
WinterHashTable::Insert(void *key, void *value){
	if(Find(key) != NULL)
		return HASH_EXISTS;
	if(IsRehashing() == HASH_OK)
		return InsertToTable(key, value, 1);
	return InsertToTable(key, value,0);
	
}


template <class T1, class T2>
int 
WinterHashTable::Insert(T1 key, T2 value){
       return Insert((void*) key, (void*)value);
	
}

unsigned long 
WinterHashTable::GetIndex(void *key, unsigned long sizeMask){
	unsigned long index;
	if(mType && mType->mHashFunction)
		index = mType->mHashFunction(key);
	else
		index = (unsigned long) key;
	return index & sizeMask;
}

int 
WinterHashTable::InsertToTable(void *key, void *value, int tableIndex){
	if(tableIndex == 1){//is rehashing
	       //add to table
	       unsigned long index = GetIndex(key,mHashTable[tableIndex].mSizeMask);
		if(index < 0 || index > mSize)
			return INSERT_ERR;
		HashEntry *p = new HashEntry(key, value);
		p->mNext = mHashTable[tableIndex].mTable[index];
		mHashTable[tableIndex].mTable[index] = p;
		mHashTable[tableIndex].mUsedSize++;
		ReHashStep(1);
		return HASH_OK;
			
	}
	if(tableIndex == 0){//not rehashing
	       //add to table
	       if(NeedReHash()){
		   	ReHash();
                     return InsertToTable(key, value, mPrimaryTable);
		}
		else{
			unsigned long index = GetIndex(key, mHashTable[tableIndex].mSizeMask);
		       if(index < 0 || index > mSize)
			       return INSERT_ERR;
		       HashEntry *p = new HashEntry(key, value);
		       p->mNext = mHashTable[tableIndex].mTable[index];
		       mHashTable[tableIndex].mTable[index] = p;
			mHashTable[tableIndex].mUsedSize++;
		       return HASH_OK;
		}		
	}
	return HASH_UNKNOWN;
}



int 
WinterHashTable::ReHashStep(int step){
	//is rehashing?
	if(!IsRehashing()) return HASH_ERR;
	
	
	while(step--){
		//if rehash finished
		if(mHashTable[0].mUsedSize == 0){
		//set mHashTable[1] to mHashTable[0]
			delete mHashTable[0].mTable;
			mHashTable[0].mTable = mHashTable[1].mTable;
			mHashTable[0].mSize =mHashTable[1].mSize;
			mHashTable[0].mUsedSize = mHashTable[1].mUsedSize;
			mHashTable[0].mSizeMask = mHashTable[1].mSizeMask;
			//mHashTable[0].mRehashIndex = 0;
			delete mHashTable[1].mTable;
			mHashTable[1].mTable = NULL;
			mHashTable[1].mSize =0;
			mHashTable[1].mUsedSize = 0;
			mHashTable[1].mSizeMask = 0;
			mIsReHashing = 0;
			mPrimaryTable =0;
			mRehashIndex = 0;
			return HASH_OK;
		}

		//if it is rehashing, move data from mHashTable[0] to mHashTable[1]
		while(mHashTable[0].mTable[mRehashIndex] == NULL)
			mRehashIndex++;
		HashEntry *p = mHashTable[0].mTable[mRehashIndex];
		while(p){
			HashEntry *q = p;
			p = p->mNext;
			unsigned long index = GetIndex(p->mKey,mHashTable[1].mSizeMask);
		       if(index < 0 || index > mSize)
				return INSERT_ERR;
			//HashEntry *q = HashEntry(key, value);
		       q->mNext = mHashTable[1].mTable[index];
		       mHashTable[1].mTable[index] = q;
			mHashTable[1].mUsedSize++;
			mHashTable[0].mUsedSize--;
		}
		
	}
	
}


bool 
WinterHashTable::NeedReHash(){
        if(mHashTable[0].mSize <= 0)
			return 1;
        else if(mHashTable[0].mUsedSize/mHashTable[0].mSize >= MAXRATE)
			return 1;
	 else 
	 	return 0;	
}

int 
WinterHashTable::ReHash(){
        //if mTable[0] is null
        if(mHashTable[0].mTable == NULL){
		mHashTable[0].mSize =  TABLE_INITIAL_SIZE;
		mPrimaryTable = 0;
		mHashTable[0].mTable = new HashEntry *[mHashTable[0].mSize];
		mHashTable[0].mSizeMask = mHashTable[0].mSize -1;
		mSize = mHashTable[0].mSize;
		return HASH_OK;
	}
	if(mHashTable[1].mTable != NULL)
		return HASH_ERR;
	mHashTable[1].mSize = ReSize(mHashTable[0].mSize);
	mPrimaryTable = 1;
	mHashTable[1].mTable = new HashEntry *[mHashTable[1].mSize];
	mHashTable[1].mSizeMask = mHashTable[1].mSize -1;
	mIsReHashing = 1;
	mSize = mHashTable[1].mSize;
	return HASH_OK;
	
	
}

//every time, the size of hash expand to double
unsigned long 
WinterHashTable::ReSize(unsigned long size){
	unsigned long i = TABLE_INITIAL_SIZE;
       if (size >= LONG_MAX) return LONG_MAX;
       while(1) {
           if (i >= size)
               return i;
           i *= 2;
    }
}

/* key is unique */
HashEntry * 
WinterHashTable::Find(void *key){
       int tableIndex = 0;
	
	unsigned long index;

	//if is rehashing
	if(IsRehashing())
		ReHashStep();
	//first find in mHashTable[0]
	if(mHashTable[0].mSizeMask == 0)
		return NULL;
	index = GetIndex(key, mHashTable[0].mSizeMask);	
	HashEntry *p = mHashTable[0].mTable[index];
	HashEntry *q;
	while(p != NULL){
		q = p;
		p = p->mNext;
		if(q->mKey == key)
			return q;
	}
	//if not in mHashTable[0]
	//    if is rehashing, find in mHashTable[1]
	if(IsRehashing()){
		index = GetIndex(key, mHashTable[1].mSizeMask);
		p = mHashTable[1].mTable[index];
		while(p != NULL){
			q = p;
			p = p->mNext;
			if(q->mKey == key)
				return q;
		}	
	}
	//do not find in hashTable
	return NULL;
}


template <class T3>
HashEntry * 
WinterHashTable::Find(T3 key){
       return Find((void*) key);
	
}


bool 
WinterHashTable::KeyCompareF(const void *key1, const void *key2){
	if(mType && mType->mKeyCompare)
		return mType->mKeyCompare(NULL, key1, key2);
	else 
		return (unsigned int) key1 == (unsigned int) key2;
}

bool
WinterHashTable::Delete(void* key){
	 int tableIndex = 0;
	
	 unsigned long index;
        HashEntry *prev;
	//if is rehashing
	if(IsRehashing())
		ReHashStep();
	//first find in mHashTable[0]
	if(mHashTable[0].mSizeMask == 0)
		return 0;
	index = GetIndex(key, mHashTable[0].mSizeMask);	
	HashEntry *p = mHashTable[0].mTable[index];
	prev = p;
	//HashEntry *q;
	//first find the key in mHashTable[0]
	while(p != NULL){
		if(KeyCompareF(p->mKey, key)){//here is compare function
			if(p == mHashTable[0].mTable[index]){
				mHashTable[0].mTable[index] = p->mNext;
				delete p;
				return 1;
			}
			prev->mNext = p->mNext; 
			delete p;
			return 1;
		}
		prev = p;
		p=p->mNext;
	}
	//if not in mHashTable[0]
	//    if is rehashing, find in mHashTable[1]
	if(IsRehashing()){
		index = GetIndex(key, mHashTable[1].mSizeMask);
		p = mHashTable[1].mTable[index];
		prev = p;
		while(p != NULL){
			if(KeyCompareF(p->mKey, key)){//here is compare function
				if(p == mHashTable[1].mTable[index]){
					mHashTable[1].mTable[index] = p->mNext;
					delete p;
					return 1;
				}
				prev->mNext = p->mNext; 
				delete p;
				return 1;
			}
			prev = p;
			p=p->mNext;
		}
	}
	//do not find in hashTable
	return 0;
}

bool
WinterHashTable::Update(void *key, void *value){
	HashEntry *res = Find(key);
	if(!res)
		return 0;
	res->mValue = value;
	return 1;
}

bool 
WinterHashTable::IsEmpty(){
	if(mSize == 0)
		return 1;
	if(mHashTable[0].mUsedSize == 0 && mHashTable[1].mUsedSize == 0)
		return 1;
	return 0;
}



bool
WinterHashTable::IsRehashing(){
	return mIsReHashing;
}






/********test program*********/

int main(){
	WinterHashTable testHashTable(NULL, NULL);
	testHashTable.Insert(50, 60);
	if(testHashTable.Find(50)){
		cout << "exists!" << endl;
	}
       int res = testHashTable.Insert(50, 60);
	cout << res << endl;
	testHashTable.Insert("50", "60");
	HashEntry *p = testHashTable.Find("50");
	if(p){
		cout << "exists! value is " << p->mValue << endl;
	}
}














