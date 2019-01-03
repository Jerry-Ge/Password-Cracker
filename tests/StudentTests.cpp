#include "catch.hpp"
#include "SrcMain.h"
#include "HashTable.hpp"
#include <string>
#include <chrono>
#include <algorithm>
#include <iostream>

// Helper function declarations (don't change these)
extern bool CheckFileMD5(const std::string& fileName, const std::string& expected);
extern bool CheckTextFilesSame(const std::string& fileNameA, 
	const std::string& fileNameB);

// Helper to generate hashes that always are the same number
template <size_t num = 0>
struct AlwaysNumHash
{
	size_t operator()(const std::string& str) const
	{
		return num;
	}
};

// Test cases for HashTable implementation
TEST_CASE("Hash Table tests", "[student]")
{
	SECTION("Basic insertion")
	{
		HashTable<int> table(20);
		// Do insertion
		REQUIRE(table.Insert("January", 1));
		REQUIRE(table.Size() == 1);
		REQUIRE(table.Capacity() == 20);

		// Make sure data is at expected location
		auto data = table.Data();
		size_t idx = std::hash<std::string>()("January");
		idx %= table.Capacity();
		REQUIRE(data[idx].first == "January");
		REQUIRE(data[idx].second == 1);
		for (size_t i = 0; i < table.Capacity(); i++)
		{
			if (i != idx)
			{
				REQUIRE(data[i].first.empty());
			}
		}
	}
    
    
	SECTION("Trying to insert at full capacity")
	{
		std::vector<std::pair<std::string, int>> elements = {
			{"January", 1},
			{"February", 2},
			{"March", 3},
		};

		HashTable<int> table(3);
		for (size_t i = 0; i < elements.size(); i++)
		{
			REQUIRE(table.Insert(elements[i].first, elements[i].second));
		}
		
		REQUIRE(table.Size() == 3);

		REQUIRE(table.Insert("April", 4) == false);
		
		REQUIRE(table.Size() == 3);
		REQUIRE(table.Capacity() == 3);
	}
    
	SECTION("Insertion with collisions")
	{
		std::vector<std::pair<std::string, int>> elements = {
			{"January", 1},
			{"February", 2},
			{"March", 3},
		};

		HashTable<int, AlwaysNumHash<0>> table(3);
		for (size_t i = 0; i < elements.size(); i++)
		{
			REQUIRE(table.Insert(elements[i].first, elements[i].second));
		}

		// Since everything hashed to zero, data should be sequential
		auto data = table.Data();
		for (size_t i = 0; i < elements.size(); i++)
		{
			REQUIRE(data[i].first == elements[i].first);
			REQUIRE(data[i].second == elements[i].second);
		}
	}
    
	SECTION("Insertion with wrapping")
	{
		std::vector<std::pair<std::string, int>> elements = {
			{"January", 1},
			{"February", 2},
			{"March", 3},
		};

		HashTable<int, AlwaysNumHash<2>> table(3);
		for (size_t i = 0; i < elements.size(); i++)
		{
			REQUIRE(table.Insert(elements[i].first, elements[i].second));
		}

		// Will be messed up ordering, since we first insert at 2, then 0, then 1
		auto data = table.Data();
		REQUIRE(data[0].first == "February");
		REQUIRE(data[1].first == "March");
		REQUIRE(data[2].first == "January");
	}
    
    
	SECTION("Basic find")
	{
		std::vector<std::pair<std::string, int>> elements = {
			{"January", 1},
			{"February", 2},
			{"March", 3},
		};

		HashTable<int> table(50);
		for (size_t i = 0; i < elements.size(); i++)
		{
			REQUIRE(table.Insert(elements[i].first, elements[i].second));
		}

		auto val = table.Find("February");
        //std::cout << val << std::endl;
        //std::cout << *val << std::endl;
		REQUIRE(*val == 2);
	}
   
	SECTION("Find when element not there")
	{
		std::vector<std::pair<std::string, int>> elements = {
			{"January", 1},
			{"February", 2},
			{"March", 3},
		};

		HashTable<int> table(50);
		for (size_t i = 0; i < elements.size(); i++)
		{
			REQUIRE(table.Insert(elements[i].first, elements[i].second));
		}

		auto val = table.Find("April");
		REQUIRE(val == nullptr);
	}
    
	SECTION("Find with collisions")
	{
		std::vector<std::pair<std::string, int>> elements = {
			{"January", 1},
			{"February", 2},
			{"March", 3},
		};

		HashTable<int, AlwaysNumHash<0>> table(3);
		for (size_t i = 0; i < elements.size(); i++)
		{
			REQUIRE(table.Insert(elements[i].first, elements[i].second));
		}

		auto val = table.Find("March");
		REQUIRE(*val == 3);
	}

	SECTION("Find with collisions and not there")
	{
		std::vector<std::pair<std::string, int>> elements = {
			{"January", 1},
			{"February", 2},
			{"March", 3},
		};

		HashTable<int, AlwaysNumHash<0>> table(3);
		for (size_t i = 0; i < elements.size(); i++)
		{
			REQUIRE(table.Insert(elements[i].first, elements[i].second));
		}

		auto val = table.Find("May");
		REQUIRE(val == nullptr);
	}
    
	SECTION("ForEach")
	{
		std::vector<std::pair<std::string, int>> elements = {
			{"January", 1},
			{"February", 2},
			{"March", 3},
		};

		HashTable<int> table(100);
		for (size_t i = 0; i < elements.size(); i++)
		{
			REQUIRE(table.Insert(elements[i].first, elements[i].second));
		}

		int count = 0;
		int janCount = 0;
		int febCount = 0;
		int marCount = 0;

		table.ForEach([&](const std::string& key, int& value) {
			count++;
			if (key == "January")
			{
				janCount++;
				REQUIRE(value == 1);
			}
			else if (key == "February")
			{
				febCount++;
				REQUIRE(value == 2);
			}
			else if (key == "March")
			{
				marCount++;
				REQUIRE(value == 3);
			}
		});

		REQUIRE(count == 3);
		REQUIRE(janCount == 1);
		REQUIRE(febCount == 1);
		REQUIRE(marCount == 1);
	}

	SECTION("ForEach - value changes persist")
	{
		std::vector<std::pair<std::string, int>> elements = {
			{"January", 1},
			{"February", 2},
			{"March", 3},
		};

		HashTable<int> table(100);
		for (size_t i = 0; i < elements.size(); i++)
		{
			REQUIRE(table.Insert(elements[i].first, elements[i].second));
		}

		table.ForEach([&](const std::string& key, int& value) {
			if (key == "January")
			{
				value = 1337;
			}
		});

		auto v = table.Find("January");
		REQUIRE(*v == 1337);
	}
    
	// Rule of Five Tests
	// NOTE: You should only uncomment these once you
	// think you've implemented them correctly (otherwise might crash)
    SECTION("Destructor deletes and sets data to nullptr")
    {
        HashTable<int>* table = new HashTable<int>(50);
        table->~HashTable();
        REQUIRE(table->Size() == 0);
        REQUIRE(table->Capacity() == 0);
        REQUIRE(table->Data() == nullptr);
        delete table;
    }

	//SECTION("Copy constructor deep copies")
    {
        std::vector<std::pair<std::string, int>> elements = {
            {"January", 1}, {"February", 2}, {"March", 3},
            {"April", 4}, {"May", 5}, {"June", 6},
            {"July", 7}, {"August", 8}, {"September", 9},
            {"October", 10}, {"November", 11}, {"December", 12},
        };

        HashTable<int> table(20);
        for (size_t i = 0; i < elements.size(); i++)
        {
            REQUIRE(table.Insert(elements[i].first, elements[i].second));
        }
        
        HashTable<int> tableCopy(table);
        // When copied, should be different memory addresses
        REQUIRE(table.Data() != tableCopy.Data());
        REQUIRE(table.Capacity() == tableCopy.Capacity());
        REQUIRE(table.Size() == tableCopy.Size());

        auto data = table.Data();
        auto dataCopy = tableCopy.Data();
        for (size_t i = 0; i < 20; i++)
        {
            REQUIRE(data[i].first == dataCopy[i].first);
            REQUIRE(data[i].second == dataCopy[i].second);
        }
    }

    SECTION("Assignment operator deep copies")
    {
        std::vector<std::pair<std::string, int>> elements = {
            {"January", 1}, {"February", 2}, {"March", 3},
            {"April", 4}, {"May", 5}, {"June", 6},
            {"July", 7}, {"August", 8}, {"September", 9},
            {"October", 10}, {"November", 11}, {"December", 12},
        };

        HashTable<int> table(20);
        for (size_t i = 0; i < elements.size(); i++)
        {
            REQUIRE(table.Insert(elements[i].first, elements[i].second));
        }

        HashTable<int> tableCopy(50);
        tableCopy = table;
        // When copied, should be different memory addresses
        REQUIRE(table.Data() != tableCopy.Data());
        REQUIRE(table.Capacity() == tableCopy.Capacity());
        REQUIRE(table.Size() == tableCopy.Size());

        auto data = table.Data();
        auto dataCopy = tableCopy.Data();
        for (size_t i = 0; i < 20; i++)
        {
            REQUIRE(data[i].first == dataCopy[i].first);
            REQUIRE(data[i].second == dataCopy[i].second);
        }
    }

    SECTION("Move constructor implementation")
    {
        std::vector<std::pair<std::string, int>> elements = {
            {"January", 1}, {"February", 2}, {"March", 3},
            {"April", 4}, {"May", 5}, {"June", 6},
            {"July", 7}, {"August", 8}, {"September", 9},
            {"October", 10}, {"November", 11}, {"December", 12},
        };

        HashTable<int> table(20);
        for (size_t i = 0; i < elements.size(); i++)
        {
            REQUIRE(table.Insert(elements[i].first, elements[i].second));
        }

        auto oldData = table.Data();
        size_t oldCapacity = table.Capacity();
        size_t oldSize = table.Size();
        
        HashTable<int> tableMoved(std::move(table));
        // When moved, should be same address as old one
        REQUIRE(oldData == tableMoved.Data());
        REQUIRE(oldCapacity == tableMoved.Capacity());
        REQUIRE(oldSize == tableMoved.Size());

        // Make sure old one is set to 0/null/etc
        REQUIRE(table.Data() == nullptr);
        REQUIRE(table.Capacity() == 0);
        REQUIRE(table.Size() == 0);

        auto dataMoved = tableMoved.Data();
        for (size_t i = 0; i < 20; i++)
        {
            REQUIRE(oldData[i].first == dataMoved[i].first);
            REQUIRE(oldData[i].second == dataMoved[i].second);
        }
    }

    SECTION("Move assignment implementation")
    {
        std::vector<std::pair<std::string, int>> elements = {
            {"January", 1}, {"February", 2}, {"March", 3},
            {"April", 4}, {"May", 5}, {"June", 6},
            {"July", 7}, {"August", 8}, {"September", 9},
            {"October", 10}, {"November", 11}, {"December", 12},
        };

        HashTable<int> table(20);
        for (size_t i = 0; i < elements.size(); i++)
        {
            REQUIRE(table.Insert(elements[i].first, elements[i].second));
        }

        auto oldData = table.Data();
        size_t oldCapacity = table.Capacity();
        size_t oldSize = table.Size();

        HashTable<int> tableMoved(50);
        tableMoved = std::move(table);
        // When moved, should be same address as old one
        REQUIRE(oldData == tableMoved.Data());
        REQUIRE(oldCapacity == tableMoved.Capacity());
        REQUIRE(oldSize == tableMoved.Size());

        // Make sure old one is set to 0/null/etc
        REQUIRE(table.Data() == nullptr);
        REQUIRE(table.Capacity() == 0);
        REQUIRE(table.Size() == 0);

        auto dataMoved = tableMoved.Data();
        for (size_t i = 0; i < 20; i++)
        {
            REQUIRE(oldData[i].first == dataMoved[i].first);
            REQUIRE(oldData[i].second == dataMoved[i].second);
        }
    }
}

// Provided file tests
// (Comment when needed)
TEST_CASE("File tests", "[student]")
{
    SECTION("Basic dictionary")
    {
        const char* argv[] = {
            "tests/tests",
            "dictionary",
            "40",
            "input/d2.txt",
            "input/pass-basicdict.txt"
        };
        ProcessCommandArgs(4, argv);
        bool result = CheckTextFilesSame("solved.txt", "expected/basicdict-solved.txt");
        REQUIRE(result);
    }

    SECTION("Nouns dictionary")
    {
        const char* argv[] = {
            "tests/tests",
            "dictionary",
            "100",
            "input/nouns.txt",
            "input/pass-nounsdict.txt"
        };
        ProcessCommandArgs(4, argv);
        bool result = CheckTextFilesSame("solved.txt", "expected/nounsdict-solved.txt");
        REQUIRE(result);
    }

    SECTION("Small phrases cracking")
    {
        const char* argv[] = {
            "tests/tests",
            "phrases",
            "100",
            "input/nouns-small.txt",
            "input/pass-phrasessmall.txt"
        };
        ProcessCommandArgs(4, argv);
        bool result = CheckTextFilesSame("solved.txt", "expected/phrasessmall-solved.txt");
        REQUIRE(result);
    }

    SECTION("Large phrases cracking")
    {
        const char* argv[] = {
            "tests/tests",
            "phrases",
            "100000",
            "input/nouns.txt",
            "input/pass-phrasesbig.txt"
        };
        ProcessCommandArgs(4, argv);
        bool result = CheckTextFilesSame("solved.txt", "expected/phrasesbig-solved.txt");
        REQUIRE(result);
    }

    SECTION("Large phrases cracking timing test")
    {
        const char* argv[] = {
            "tests/tests",
            "phrases",
            "100000",
            "input/nouns.txt",
            "input/pass-phrasesbig.txt"
        };

        auto start = std::chrono::high_resolution_clock::now();
        ProcessCommandArgs(4, argv);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        float seconds = duration / 1000000000.0f;

        bool result = CheckTextFilesSame("solved.txt", "expected/phrasesbig-solved.txt");
        REQUIRE(result);

        WARN("****Large phrases timed test took: " << seconds << "s****");
        REQUIRE(seconds < 3.0f);
    }
}
