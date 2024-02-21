using namespace std;

class accumulator {
public:
   accumulator (const accumulator& ht );       // constructor for a copy
   accumulator(const unsigned long NumKeys);          // constructor of hashtable 
   ~accumulator();                           // destructor
   void Print () const;       
   void Insert (const string Key, const float Data); 
   int GetData (const string docId); 
   void GetUsage (int &Used, int &Collisions, int &Lookups) const;
   void InsertAdd(const string docId, const float Data);
   protected:
   struct StringIntPair // the datatype stored in the hashtable`
   {
      string key;
      string docid;
      float data;
   };
   unsigned long Find (const string docid); // the index of the ddr in the hashtable
private:
   StringIntPair *hashtable;        // the hashtable array itself
   unsigned long size;              // the hashtable size
   unsigned long used;
   unsigned long collisions;
   unsigned long lookups;
};
