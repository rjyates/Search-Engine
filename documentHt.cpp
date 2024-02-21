#include <assert.h>
#include <iostream>
#include <fstream>

#include "documentHt.h"
// #include "globalHt.h"

using namespace std;
/*-------------------------- Constructors/Destructors ----------------------*/

/* : documentHt
 * Author: seg
 * Parameters:  ht: the hashtable to copy
 * Purpose:     copy a hashtable 
 *              NOTE:  this is crucial to include since it is invoked
 *              by call-by-value parameter passing
 * Returns:     nothing
*/
documentHt::documentHt( const documentHt &ht )
{
   size = ht.size;                    // set the size of the array
   used = ht.used;                    // set the size of the array
   if((hashtable = new StringIntPair[size]) == NULL)
       cout << "Out of memory at documentHt::documentHt(const documentHt)" << endl;
   assert( hashtable != 0 );

   for (unsigned long i=0; i < size; i++)     // make a _copy_ of the array elements
   {
      hashtable[i].key = ht.hashtable[i].key;
      hashtable[i].data = 0;
   }
   
}
           
/* Name:  documentHt
 * Author: seg
 * Parameters:  none
 * Purpose:     allocate a hashtable for an expected number of keys
 *              initializes all values to null (0)
 * Returns:     pointer to the created documentHt or 0 if out of memory
*/
documentHt::documentHt(const unsigned long NumKeys)
{
   // allocate space for the table, init to null key
   size = NumKeys * 3;   // we want the hash table to be 2/3 empty
   used = 0;
   collisions = 0;
   lookups = 0;
   numtokens = 0;
   if((hashtable = new StringIntPair[size]) == NULL)
      cout << "Out of memory at documentHt::documentHt(unsigned long)" << endl;
   assert( hashtable != 0 );

   // initialize the hashtable
   for (unsigned long i=0; i < size; i++)
   {
      hashtable[i].key = "";
      hashtable[i].data = 0;
   }
}


/* Name:  ~documentHt
 * Author: seg
 * Parameters:  none
 * Purpose:     deallocate a hash table
 * Returns:     nothing
*/
documentHt::~documentHt()
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
void documentHt::Print(const char *filename) const
{
   ofstream fpout(filename); 

   // Print out the non-zero contents of the hashtable
   for ( unsigned long i=0; i < size; i++ )
   {  
      if ( !(hashtable[i].key == ""))
          fpout << hashtable[i].key << " "
                << hashtable[i].data << endl;
   }
   fpout.close();
   cout << "Collisions: " << collisions << ", Used: " << used
        <<  ", Lookups: " << lookups << endl;
}

void documentHt::PrintScreen() {
   for(unsigned long i=0; i < size; i++) {
      if(!(hashtable[i].key == "")) {
         cout << hashtable[i].key << " " << hashtable[i].data << endl;
      }
   }
   cout << "Collisions: " << collisions << ", Used: " << used
        <<  ", Lookups: " << lookups << endl;
}

/* Name: Insert
 * Author: sgauch
 * Parameter:
 * 		key : The target of context words to be stored
 * 		frequency: Total frequency count
 * Purpose: 	insert or add a word with its frequency count in hashtable
 * Return:	nothing
*/
void documentHt::Insert (const string Key)
{
unsigned long Index;
numtokens++;
 if (used >= size) {
   cerr << "The hashtable is full; cannot insert.\n";
   // return false;
 } else {
   Index = Find(Key);

   // If not already in the table, insert it
   if (hashtable[Index].key == "") {
      hashtable[Index].key = Key;
      hashtable[Index].data = 1;
      // hashtable[Index].docId = docId;
      used++; 
   } else {
      hashtable[Index].data++;
   }
      // else do nothing
   }
}

/* void documentHt::InsertIntoGlobal(globalHt& globalHt) {
   for(unsigned long i = 0; i < size; i++) {
      if(!hashtable[i].key == "") {
         globalHt.Insert(hashtable[i].key, hashtable[i].data);
      }
   }
} */

/* Name: GetData
 * Author: sgauch
 * Parameters:	key: the string
 * Purpose:	return the data or -1 if Key is not found
 * Return:	return an int 
*/
int documentHt::GetData(const string Key)
{
unsigned long Index;

 lookups++; 
 Index = Find(Key);
 if (hashtable[Index].key == "")
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
void documentHt::GetUsage(int &Used, int &Collisions, int &Lookups) const
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
unsigned long documentHt::Find (const string Key) 
{
unsigned long Sum = 0;
unsigned long Index;

   // add all the characters of the key together
   for (long unsigned i=0; i < Key.length(); i++)
      Sum = (Sum * 19) + Key[i];  // Mult sum by 19, add byte value of char
  
   Index = Sum % size;

   // Check to see if word is in that location
   // If not there, do linear probing until word found
   // or empty location found.
   while (((hashtable[Index].key) != Key) &&
          ((hashtable[Index].key) != "") ) 
   {
      Index = (Index+1) % size;
      collisions++;
   }
   
   return Index;
}

void documentHt::Clear() {
   // allocate space for the table, init to null key
   // size = NumKeys * 3;   // we want the hash table to be 2/3 empty
   used = 0;
   collisions = 0;
   lookups = 0;
   numtokens = 0;

   // initialize the hashtable
   for (unsigned long i=0; i < size; i++)
   {
      hashtable[i].key = "";
      hashtable[i].data = 0;
   }
}

void documentHt::InsertIntoGlobal(globalHt& globalHt, int docId) {
   for(unsigned long i=0; i < size; i++) {
      if(!(hashtable[i].key == "")) {
        //cout << "inglob" << hashtable[i].data << " / " << numtokens << endl; 
         // The global hashtable class itself handles numDocs
	 float wt = (float)hashtable[i].data/(float)numtokens;
 	//cout << "wt " << fixed << setprecision(4) << wt << endl;
         globalHt.Insert(hashtable[i].key, docId, wt);
      }
   }
}

unsigned long documentHt::GetUnique() const {
   return used;
}

/* unsigned long documentHt::GetSize() const {
   return size;
} */
