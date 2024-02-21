#include <iomanip>
#include <string>
#include <fstream>
using namespace std;

class invertedFile {
    public:
        invertedFile(); // Constructor
        ~invertedFile(); // Destructor

        void openForWrite(const string OutDir);
        void openForRead(const string OutDir);

        void closeForWrite();
        void closeForRead();

	unsigned long Find(const string key);

        void writeMapRecord(const int docId, string filename);
        void writeDictRecord(const int recordNum, string term, const int numdocs, const int start);
        void writeDictRecord(const string term, const int numdocs, const int start);
        void writePostRecord(const int docId, const float weight);

        void readMapRecord(const int recordNum, int &docId, string &filename);
        void readDictRecord(const int recordNum, string &term, int &numdocs, int &start);
        void readPostRecord(const int recordNum, int &docId, float &weight);

    private:
        ifstream imapFile;
        ifstream idictFile;
        ifstream ipostFile;
        ifstream iconfigFile;

        ofstream omapFile;
        ofstream odictFile;
        ofstream opostFile;
        ofstream oconfigFile;

        const int MAP_RECORD_SIZE = 21;
        const int DICT_RECORD_SIZE = 31;
        const int POST_RECORD_SIZE = 12;
        int numMapRecords;
        int numDictRecords;
        int numPostRecords;
        const string configFilename = "config.txt";
        string mapFilename = "map.txt";
        string dictFilename = "dict.txt";
        string postFilename = "post.txt";
        bool isOpen;
};

