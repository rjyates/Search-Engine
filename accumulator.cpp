#include <assert.h>
#include <iostream>
#include <fstream>
#include "queryList.h"
#include "accumulator.h"
queryList qlist;

using namespace std;
/*-------------------------- Constructors/Destructors ----------------------*/

/* : HashTable
 * Author: seg
 * Parameters:  ht: the hashtable to copy
 * Purpose:     copy a hashtable 
 *              NOTE:  this is crucial to include since it is invoked
 *              by call-by-value parameter passing
 * Returns:     nothing
*/
accumulator::accumulator( const accumulator &ht )
{
   size = ht.size;                    // set the size of the array
   used = ht.used;                    // set the size of the array
   if((hashtable = new StringIntPair[size]) == NULL)
       cout << "Out of memory at HashTable::HashTable(const HashTable)" << endl;
   assert( hashtable != 0 );

   for (unsigned long i=0; i < size; i++)     // make a _copy_ of the array elements
   {
      hashtable[i].key = ht.hashtable[i].key;
      hashtable[i].data = ht.hashtable[i].data;
      hashtable[i].docid = ht.hashtable[i].docid;
   }
   
}
           
/* Name:  HashTable
 * Author: seg
 * Parameters:  none
 * Purpose:     allocate a hashtable for an expected number of keys
 *              initializes all values to null (0)
 * Returns:     pointer to the created HashTable or 0 if out of memory
*/
accumulator::accumulator(const unsigned long NumKeys)
{
   // allocate space for the table, init to null key
   size = NumKeys * 3;   // we want the hash table to be 2/3 empty
   used = 0;
   collisions = 0;
   lookups = 0;
   if((hashtable = new StringIntPair[size]) == NULL)
      cout << "Out of memory at HashTable::HashTable(unsigned long)" << endl;
   assert( hashtable != 0 );

   // initialize the hashtable
   for (unsigned long i=0; i < size; i++)
   {
      hashtable[i].key = "";
      hashtable[i].data = 0;
      hashtable[i].docid = "";
   }
}


/* Name:  ~HashTable
 * Author: seg
 * Parameters:  none
 * Purpose:     deallocate a hash table
 * Returns:     nothing
*/
accumulator::~accumulator()
{
   delete [] hashtable;
}

/*-------------------------- Accessors ------------------------------------*/


/* Name:  Print
 * Author: seg
 * Parameters:  none
 * Purpose:     print the contents of the hash table
 *              currently, only prints non-null entries
 * Returns:     nothing
*/
void accumulator::Print() const
{
   //ofstream fpout(filename); 

   // Print out the non-zero contents of the hashtable
   for ( unsigned long i=0; i < size; i++ )
   {  
      if ( !(hashtable[i].docid == ""))
         // cout << hashtable[i].docid << " " << hashtable[i].data << endl;
      	  //int doci = stoi(hashtable[i].docid);
      	 // qlist.AddSorted(doci, hashtable[i].data);
	 try
	{
    	int doci = stoi(hashtable[i].docid);
	qlist.AddSorted(doci, hashtable[i].data);
	} 
	catch (const std::invalid_argument& e)
	{
    	cout << endl;
	}
	catch (const std::out_of_range& e)
	{
    	cout << endl;
	}
   }
   qlist.Print();
   
   //fpout.close();
   //cout << "Collisions: " << collisions << ", Used: " << used
     //   <<  ", Lookups: " << lookups << endl;
}

/* Name: Insert
 * Author: sgauch
 * Parameter:
 * 		key : The target of context words to be stored
 * 		frequency: Total frequency count
 * Purpose: 	insert or add a word with its frequency count in hashtable
 * Return:	nothing
*/
void accumulator::Insert (const string Key, const float Data)
{
//unsigned long Index;

// if (used >= size)
//    cerr << "The hashtable is full; cannot insert.\n";
// else
// {
    //Index = Find(Key);

    // If not already in the table, insert it
//    if (hashtable[Index].key == "")
//    {
//       hashtable[Index].key = Key;
//       hashtable[Index].data = Data;
//       used++;
  //  }
    // else do nothing
// }
}

void accumulator::InsertAdd (const string docId, const float Data)
{
unsigned long Index;

 if (used >= size)
    cerr << "The hashtable is full; cannot insert.\n";
 else
 {
    Index = Find(docId);

    // If not already in the table, insert it
    if (hashtable[Index].docid == "" )
    {
       hashtable[Index].docid = docId;
       hashtable[Index].data = hashtable[Index].data + Data;
       used++;
    }
    // else do nothing
 }
}


/* Name: GetData
 * Author: sgauch
 * Parameters:	key: the string
 * Purpose:	return the data or -1 if Key is not found
 * Return:	return an int 
*/
int accumulator::GetData(const string docId)
{
unsigned long Index;

 lookups++; 
 Index = Find(docId);
 if (hashtable[Index].docid == "")
    return -1;
 else   
    return (hashtable[Index].data);
}

/* Name: GetUsage
 * Author: S. Gauch
 * Parameters:	None
 * Purpose:	return the number of collisions
 * Return:	return a char *
*/
void accumulator::GetUsage(int &Used, int &Collisions, int &Lookups) const
{
   Used = used;
   Collisions = collisions; 
   Lookups = lookups;
}


/*-------------------------- Private Functions ----------------------------*/
/* Name:  Find
 * Author: seg
 * Parameters:  key: the word to be located
 * Purpose:     return the index of the word in the table, or
 *              the index of the free space in which to store the word
 * Returns:     index of the word's actual or desired location
*/
unsigned long accumulator::Find (const string docId) 
{
unsigned long Sum = 0;
unsigned long Index;

   // add all the characters of the key together
   for (long unsigned i=0; i < docId.length(); i++)
      Sum = (Sum * 19) + docId[i];  // Mult sum by 19, add byte value of char
  
   Index = Sum % size;

   // Check to see if word is in that location
   // If not there, do linear probing until word found
   // or empty location found.

   while (((hashtable[Index].docid) != docId) &&
          ((hashtable[Index].docid) != "") ) 
   {
      Index = (Index+1) % size;
      collisions++;
   }
   
   return Index;
}
//unsigned long accumulator::Find (const int docId)
//{
//  unsigned long Sum = 0;
//  unsigned long Index;
//  int docid = 0;
//  float weight = 0;
  //unsigned long location = Find(Key);

   // add all the characters of the key together
   //for (long unsigned i=0; i < Key.length(); i++)
     // Sum = (Sum * 19) + Key[i];  // Mult sum by 19, add byte value of char

 // Index = 4 % size;

   // Check to see if word is in that location
   // If not there, do linear probing until word found
   // or empty location found.
   //readPostRecord(Index, docid, weight);
 //  while (hashtable[Index].docid != docId)
 //  {
   //     Index = (Index + 1) % size;
     //   readPostRecord(Index, docid, weight);
  // }

    //  return Index;
     
   
//}



