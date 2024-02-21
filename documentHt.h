#include "globalHt.h"

using namespace std;

class documentHt {
public:
   documentHt(const unsigned long NumKeys);  // constructor of hashtable 
   documentHt (const documentHt& ht );       // constructor for a copy
   ~documentHt();                            // destructor
   void Print (const char *filename) const;  
   void PrintScreen();     
   void Insert (const string Key); 
   int GetData (const string Key); 
   void GetUsage (int &Used, int &Collisions, int &Lookups) const;
   void Clear();
   void InsertIntoGlobal(globalHt& globalHt, int docId);
   // unsigned long GetSize() const;
   // void InsertIntoGlobal(globalHt& globalHt);
   unsigned long GetUnique() const;
protected:
   struct StringIntPair // the datatype stored in the hashtable`
   {
      string key;
      int data;
      // int docId;
   };
   unsigned long Find (const string Key); // the index of the ddr in the hashtable
private:
   StringIntPair *hashtable;        // the hashtable array itself
   unsigned long size;              // the hashtable size
   unsigned long used;
   unsigned long collisions;
   unsigned long lookups;
   int numtokens;
};
