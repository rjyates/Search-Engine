#include "invertedFile.h"
#include <iostream>
using namespace std;

// Constructor - initialize all num records to -1.
invertedFile::invertedFile() {
    numMapRecords = -1;
    numDictRecords = -1;
    numPostRecords = -1;
}

// Destructor - Do nothing.
invertedFile::~invertedFile() {
    // Do nothing
}

// Open functions
void invertedFile::openForWrite(const string OutDir) {
    // Set num records map to 0.
    // configFilename = OutDir + "/" + configFilename;
    mapFilename = OutDir + "/" + mapFilename;
    dictFilename = OutDir + "/" + dictFilename;
    postFilename = OutDir + "/" + postFilename;

    numMapRecords = 0;
    numDictRecords = 0;
    numPostRecords = 0;

    // open files for writing.
    omapFile.open(mapFilename);
    odictFile.open(dictFilename);
    opostFile.open(postFilename);

    // Set file open boolean to true.
    isOpen = true;
}

void invertedFile::openForRead(const string OutDir) {
    // reads the number of records variables for the map, dict, and post files
    mapFilename = OutDir + "/" + mapFilename;
    dictFilename = OutDir + "/" + dictFilename;
    postFilename = OutDir + "/" + postFilename;

    iconfigFile.open(configFilename);
    iconfigFile >> numMapRecords >> numDictRecords >> numPostRecords;
    iconfigFile.close();
    // open files for writing.
    imapFile.open(mapFilename);
    idictFile.open(dictFilename);
    ipostFile.open(postFilename);

    // Set file open boolean to true.
    isOpen = true;
}

// Close functions
void invertedFile::closeForWrite() {
    /* mapFilename = OutDir + "/" + mapFilename;
    dictFilename = OutDir + "/" + dictFilename;
    postFilename = OutDir + "/" + postFilename; */
    // writes the number of records variables for the map, dict, and post files
    oconfigFile.open(configFilename);
    oconfigFile << numMapRecords << " " << numDictRecords << " " << numPostRecords;
    oconfigFile.close();

    // closes the dict, post, and map files
    omapFile.close();
    odictFile.close();
    opostFile.close();

    // set num records to -1 for dict post and map.
    numMapRecords = -1;
    numDictRecords = -1;
    numPostRecords = -1;

    // Set file open boolean to false.
    isOpen = false;
}

void invertedFile::closeForRead() {
    // closes the dict, post, and map files
    imapFile.close();
    idictFile.close();
    ipostFile.close();

    // set num records to -1 for dict post and map.
    numMapRecords = -1;
    numDictRecords = -1;
    numPostRecords = -1;

    // Set file open boolean to false.
    isOpen = false;
}


unsigned long invertedFile::Find (const string Key)
{
  unsigned long Sum = 0;
  unsigned long Index;
  string term = "";
  int numdocs = 0;
  int start = 0;
  //unsigned long location = Find(Key);

   // add all the characters of the key together
   for (long unsigned i=0; i < Key.length(); i++)
      Sum = (Sum * 19) + Key[i];  // Mult sum by 19, add byte value of char

  Index = Sum % numDictRecords;

   // Check to see if word is in that location
   // If not there, do linear probing until word found
   // or empty location found.
   readDictRecord(Index, term, numdocs, start);
   while ((term != Key) && (term != ""))
   {
        Index = (Index + 1) % numDictRecords;
        readDictRecord(Index, term, numdocs, start);
   }

   if(term != ""){
      return Index;
      cout << "Index is " << Index << endl;
   }
}


/*  -----------------------------------------------------------------------------------------------------------------------------------------------------
                                                        WRITE FUNCTIONS
    -----------------------------------------------------------------------------------------------------------------------------------------------------
*/

// Write to Map File
void invertedFile::writeMapRecord(const int docId, const string filename) {
    if (isOpen) {
        omapFile << setw(4) // 4 characters for docId
                << docId
                << " " // 1 character for space
                << setw(15)
                << filename.substr(0, min((int)filename.length(), 15)) // 15 characters for filename, truncate the rest.
                << endl; // 1 character for newline.
        
        numMapRecords++;
    } else {
        cout << "ERROR: File is not open." << endl;
    }
}

void invertedFile::writeDictRecord(const string term, const int numdocs, const int start) {
    if (isOpen) {
        odictFile << setw(20) // 20 characters term
                << term.substr(0, min((int)term.length(), 20))
                << " " // 1 character space
                << setw(4) // 4 character numdocs
                << numdocs
                << " " // 1 character space
                << setw(4) // 4 character start
                << start
                << endl; // 1 character newline
        numDictRecords++;
    } else {
        cout << "ERROR: File is not open." << endl;
    }
}

// Write to Post File
void invertedFile::writePostRecord(int docId, float weight) {
    if (isOpen) {
        opostFile << setw(4) // 4 characters for docId
                << docId
                << " " // 1 character for space
                << setw(4) // 4 characters for weight
		<< setprecision(4)
		<< fixed
                << weight
                << endl;
        
        numPostRecords++;
    } else {
        cout << "ERROR: File is not open." << endl;
    }
}

/*  -----------------------------------------------------------------------------------------------------------------------------------------------------
                                                        READ FUNCTIONS
    -----------------------------------------------------------------------------------------------------------------------------------------------------
*/

// Read a Map Record
void invertedFile::readMapRecord(const int recordNum, int &docId, string &filename) {
    if (isOpen) {
        if (recordNum >= 0 && recordNum <= numMapRecords) {
            imapFile.seekg(recordNum * MAP_RECORD_SIZE, ios::beg);
            imapFile >> docId >> filename;
        } else {
            docId = -1;
            filename = "";
        }
    } else {
        cout << "ERROR: File is not open." << endl;
    }
}

// Read a Dict Record
void invertedFile::readDictRecord(const int recordNum, string &term, int &numdocs, int &start) {
    if (isOpen) {
        if (recordNum >= 0 && recordNum <= numDictRecords) {
            idictFile.seekg(0, ios::beg);
            idictFile.seekg(recordNum * DICT_RECORD_SIZE, ios::beg);
            idictFile >> term >> numdocs >> start;
        } else {
            term = "empty";
            numdocs = -1;
            start = -1;
        }
    } else {
        cout << "ERROR: File is not open." << endl;
    }
}

// Read a Post Record
void invertedFile::readPostRecord(const int recordNum, int &docId, float &weight) {
    if (isOpen) {
        if (recordNum >= 0 && recordNum <= numPostRecords) {
            ipostFile.seekg(recordNum * POST_RECORD_SIZE, ios::beg);
            ipostFile >> docId >> weight;
        } else {
            docId = -1;
            weight = -1;
        }
    } else {
        cout << "ERROR: File is not open." << endl;
    }
}

