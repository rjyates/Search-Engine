#include "List.h"
#include <cmath>
// #include "invertedFile.h"

using namespace std;

class globalHt {
public:
   globalHt (const globalHt& ht );       // constructor for a copy
   globalHt(const unsigned long NumKeys);          // constructor of hashtable 
   ~globalHt();                           // destructor
   void Print (const char *filename) const;  
   void PrintScreen();     
   void Insert (const string Key, int numDocs, float weight); 
   int GetNumDocs (const string Key); 
   void GetUsage (int &Used, int &Collisions, int &Lookups) const;
   void Clear();
   void WriteToDict(invertedFile& invFile, int &document_Num);
   void WriteToPost(invertedFile& invFile);
   void UpdateToken();
   unsigned long GetUnique() const;
protected:
   struct StringIntTriple // the datatype stored in the hashtable`
   {
      string key;
      int numDocs;
      List postList;
   };
   unsigned long Find (const string Key); // the index of the ddr in the hashtable
private:
   StringIntTriple *hashtable;        // the hashtable array itself
   unsigned long size;              // the hashtable size
   unsigned long used;
   unsigned long collisions;
   unsigned long lookups;
};
