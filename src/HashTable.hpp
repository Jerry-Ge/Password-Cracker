#pragma once
#include <string>
#include <utility>
#include <functional>
#include <iostream>
template <typename ValueType, 
	typename HashFuncType = std::hash<std::string>>
class HashTable
{
public:
	// Constructs the hash table with the given capacity (size of underlying array)
	HashTable(size_t capacity)
		:mTableData(new std::pair<std::string, ValueType>[capacity])
		,mCapacity(capacity)
		,mSize(0)
	{ }

	// TODO: Implement "Rule of five" functions
    
    //1. destructor
    ~HashTable() {
        delete [] mTableData;
        mTableData = nullptr;
        mCapacity = 0;
        mSize = 0;
    }

    //2. copy constructor
    //need the deep copy
    HashTable(const HashTable& other)
        :mCapacity(other.mCapacity),
         mSize(other.mSize)
    {
        mTableData = new std::pair<std::string, ValueType>[mCapacity];
        for (size_t i = 0; i < mCapacity; ++i) {
            mTableData[i] = other.mTableData[i];
        }
    }

    //3. assignment operator
    HashTable& operator=(const HashTable& other)
    {
        delete [] mTableData;
        mCapacity = other.mCapacity;
        mSize = other.mSize;
        mTableData = new std::pair<std::string, ValueType>[mCapacity];

        //do a deep copy for actual data
        for (size_t i = 0; i < mCapacity; ++i)
        {
            mTableData[i] = other.mTableData[i];
        }

        return (*this);
    }

    //4. move constructor
    HashTable(HashTable&& other)
    : mTableData(std::move(other.mTableData)),
      mCapacity(std::move(other.mCapacity)),
      mSize(std::move(other.mSize))
    {
        other.mTableData = nullptr;
        other.mCapacity = 0;
        other.mSize = 0;
    }

    //5. move assignment operator
    HashTable& operator=(HashTable&& other)
    {
        delete [] mTableData; //clear existing data
        mTableData = std::move(other.mTableData);
        mCapacity = std::move(other.mCapacity);
        mSize = std::move(other.mSize);
        other.mTableData = nullptr;
        other.mCapacity = 0;
        other.mSize = 0;
        return (*this);
    }
    
	// Returns overall capacity of hash table (size of underlying array)
	size_t Capacity() const
	{
		return mCapacity;
	}

	// Returns the number of elements currently inserted in the hash table
	size_t Size() const
	{
		return mSize;
	}

	// Tries to insert (key, value) into hash table
	// Returns true if insert succeeds, false if not
	bool Insert(const std::string& key, const ValueType& value)
	{
        // TODO: Implement
        if (mSize == mCapacity) //already full
        {
            return false;
        }

        size_t initIndex = (mHashFunc(key)) % mCapacity; //get the initial insertion key
        
        for (size_t i = initIndex; i < mCapacity; ++i)
        {
            if (mTableData[i].first.empty()) //empty slot
            {
                mTableData[i] = std::make_pair(key, value);
                mSize++;
                
                return true;
            }
        }

        //wrap back to zero, check until the init index
        for (size_t i = 0; i < initIndex; ++i)
        {
            if (mTableData[i].first.empty())
            {
                mTableData[i] = std::make_pair(key, value);
                mSize++;
                return true;
            }
        }
        
        
        //no empty slots
		return false;
	}

	// Returns pointer to value for given key, or nullptr
	// if not in hash table
	ValueType* Find(const std::string& key) const
	{
        // TODO: Implement
        size_t initIndex = (mHashFunc(key)) % mCapacity;

        for (size_t i = initIndex; i < mCapacity; ++i) {
            if (mTableData[i].first.empty())
            {
                return nullptr;
            }
            if (mTableData[i].first == key) {
                ValueType* found =  &(mTableData[i].second);
                return found;
            }
        }

        //wrap back from 0;
        for (size_t i = 0; i < initIndex; ++i) {
            if (mTableData[i].first.empty())
            {
                return nullptr;
            }
            if (mTableData[i].first == key) {
               
                ValueType* found = &(mTableData[i].second);
                return found;
            }
        }

        
        //found out nothing, return NULL
		return nullptr;
	}

	// Executes provided lambda expression on each element inserted into the table
	void ForEach(std::function<void(const std::string& key, ValueType& value)> func)
	{
		// TODO: Implement
        for (size_t i = 0; i < mCapacity; ++i) {
            if (mTableData[i].first.empty())
            {
                continue;
            }
            func(mTableData[i].first, mTableData[i].second); //apply the function to all elements
        }
	}

	// Gets a const pointer to underlying data (for testing)
	const std::pair<std::string, ValueType>* Data() const
	{
		return mTableData;
	}
private:
	// NOTE: you should not need any more member data!
	
	// Underlying hash table array
	std::pair<std::string, ValueType>* mTableData;
	// Allocated capacity of hash table array
	size_t mCapacity;
	// Number of elements in HashTable
	size_t mSize;
	// Hash function
	HashFuncType mHashFunc;
};
