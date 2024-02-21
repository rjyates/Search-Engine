#include <assert.h>
#include <iostream>
#include <fstream>

#include "globalHt.h"

using namespace std;
/*-------------------------- Constructors/Destructors ----------------------*/

/* : globalHt
 * Author: seg
 * Parameters:  ht: the hashtable to copy
 * Purpose:     copy a hashtable 
 *              NOTE:  this is crucial to include since it is invoked
 *              by call-by-value parameter passing
 * Returns:     nothing
*/
globalHt::globalHt( const globalHt &ht )
{
   size = ht.size;                    // set the size of the array
   used = ht.used;                    // set the size of the array
   if((hashtable = new StringIntTriple[size]) == NULL)
       cout << "Out of memory at globalHt::globalHt(const globalHt)" << endl;
   assert( hashtable != 0 );

   for (unsigned long i=0; i < size; i++)     // make a _copy_ of the array elements
   {
      hashtable[i].key = ht.hashtable[i].key;
      hashtable[i].numDocs = 0;
      // hashtable[i].postList;
   }
   
}
           
/* Name:  globalHt
 * Author: seg
 * Parameters:  none
 * Purpose:     allocate a hashtable for an expected number of keys
 *              initializes all values to null (0)
 * Returns:     pointer to the created documentHt or 0 if out of memory
*/
globalHt::globalHt(const unsigned long NumKeys)
{
   // allocate space for the table, init to null key
   size = NumKeys * 3;   // we want the hash table to be 2/3 empty
   used = 0;
   collisions = 0;
   lookups = 0;
   if((hashtable = new StringIntTriple[size]) == NULL)
      cout << "Out of memory at globalHt::globalHt(unsigned long)" << endl;
   assert( hashtable != 0 );

   // initialize the hashtable
   for (unsigned long i=0; i < size; i++)
   {
      hashtable[i].key = "";
      hashtable[i].numDocs = 0;
   }
}


/* Name:  ~globalHt
 * Author: seg
 * Parameters:  none
 * Purpose:     deallocate a hash table
 * Returns:     nothing
*/
globalHt::~globalHt()
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
void globalHt::Print(const char *filename) const
{
   ofstream fpout(filename); 

   // Print out the non-zero contents of the hashtable
   for ( unsigned long i=0; i < size; i++ )
   {  
      if ( !(hashtable[i].key == ""))
          fpout << hashtable[i].key << " "
                << hashtable[i].numDocs << endl;
   }
   fpout.close();
   cout << "Collisions: " << collisions << ", Used: " << used
        <<  ", Lookups: " << lookups << endl;
}


void globalHt::PrintScreen() {
   // cout << "Printing now" << endl;
   for(unsigned long i=0; i < size; i++) {
      if(!(hashtable[i].key == "")) {
         cout << hashtable[i].key << " " << hashtable[i].numDocs << endl << hashtable[i].postList.testPrint() << endl;
      }
   }
   /* cout << "Collisions: " << collisions << ", Used: " << used
        <<  ", Lookups: " << lookups << endl; */
}

void globalHt::WriteToDict(invertedFile& invFile, int &document_Num) {
   int start = 0;
   for(unsigned long i = 0; i < size; i++) {
         invFile.writeDictRecord(hashtable[i].key, hashtable[i].numDocs, start);
	//cout << "testnum" << document_Num << " / " << hashtable[i].numDocs << endl;
	 if(hashtable[i].numDocs != 0){
          float idf = 1 + log(document_Num/hashtable[i].numDocs);
	  hashtable[i].postList.WriteList(invFile, start, idf);
	 }
	 else{
	  float idf = 0;
	  hashtable[i].postList.WriteList(invFile, start, idf);
	 }
         // start++;
   }
}

/* void globalHt::WriteToPost(invertedFile& invFile) {
   
} */


void globalHt::Insert(const string Key, int docId, float weight) {
   unsigned long Index;
   if(used >= size) {
      cerr << "The hashtable is full; cannot insert.\n";
   } else {
      Index = Find(Key);
      if (hashtable[Index].key == "") {
         hashtable[Index].key = Key;
         // int docId = numDocs;
         hashtable[Index].numDocs = 1;
         hashtable[Index].postList.AddToEnd(docId, weight);
	 //cout << "weight" << fixed << setprecision(4) << weight << endl;
         used++;
      } else {
         // int docId = numDocs;
         hashtable[Index].numDocs++;
         hashtable[Index].postList.AddToEnd(docId, weight);
	 //cout << "weight" << weight << endl;
      }
   }
}

/* Name: GetData
 * Author: sgauch
 * Parameters:	key: the string
 * Purpose:	return the data or -1 if Key is not found
 * Return:	return an int 
*/
int globalHt::GetNumDocs(const string Key)
{
unsigned long Index;

 lookups++; 
 Index = Find(Key);
 if (hashtable[Index].key == "")
    return -1;
 else   
    return (hashtable[Index].numDocs);
}

/* Name: GetUsage
 * Author: S. Gauch
 * Parameters:	None
 * Purpose:	return the number of collisions
 * Return:	return a char *
*/
void globalHt::GetUsage(int &Used, int &Collisions, int &Lookups) const
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
unsigned long globalHt::Find (const string Key) 
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

void globalHt::Clear() {
   // allocate space for the table, init to null key
   // size = NumKeys * 3;   // we want the hash table to be 2/3 empty
   used = 0;
   collisions = 0;
   lookups = 0;

   // initialize the hashtable
   for (unsigned long i=0; i < size; i++)
   {
      hashtable[i].key = "";
      hashtable[i].numDocs = 0;
   }
}

unsigned long globalHt::GetUnique() const {
   return used;
}
