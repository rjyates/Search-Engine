%{
#include <string.h>
#include <dirent.h>
#include <iostream>
//#include <cmath>
#include "documentHt.h"
//#include "globalHt.h"
//#include "invertedFile.h"
#include "List.h"
extern int yylex(void);
using namespace std;

int globalTotalTokens;
int globalUniqueTokens;
int fileTotalTokens;
const unsigned long NUM_KEYS = 20; // 10
const unsigned long NUM_KEYS_DOC = 10; // 5
documentHt documentHt(NUM_KEYS_DOC);
globalHt globalHt(NUM_KEYS);
invertedFile invFile;
int docId = 0;
int document_Num = 0;

#undef yywrap            // safety measure in case using old flex 

//---------  INSERT ANY CODE CALLED BY THE LEX RULES HERE --------


void downcase_and_print(const string &text) {
   string result = text;
   for(char &c: result) {
      c = tolower(c);
   }
   documentHt.Insert(result);
   globalTotalTokens++;
   fileTotalTokens++;
}


// Example of a function called inside yywrap
void ProcessDocumentHashtable()
{
   /* cout << "Total tokens in document: " << fileTotalTokens << endl;
   unsigned long unique = documentHt.GetUnique();
   cout << "Total unique tokens in document: " << unique << endl;
   cout << "Document Hashtable: " << endl;
   documentHt.PrintScreen(); */
   // cout << "Should be inserting into global hastable from document hashtable" << endl;
   documentHt.InsertIntoGlobal(globalHt, docId); // Write function to write to global hashtable
   documentHt.Clear();
   //documentHt.Print("test.txt");
}

%}

/*----------------------------------------------------------*/
/*Section of code which specifies lex substitution patterns */
/*----------------------------------------------------------*/

DIGIT			[0-9]
LETTER			[A-Za-z]
UPPERCASE		[A-Z]
LOWERCASE		[a-z]
ALPHANUM		[A-Za-z0-9]

WORD			[A-Za-z][a-z]*
UPPERWORD		[A-Z0-9][A-Z0-9]*
HYPHENWORD		[A-Za-z]+(\-[A-Za-z]+)+
ABBREVIATION	[A-Za-z]+\.([A-Za-z]+\.)+

NUMBER 			[0-9]+(,[0-9]+)*
FLOAT 			[0-9]*\.[0-9]+
PHONENUMBER		[0-9]{3}-[0-9]{3}-[0-9]{4}
TIME			[0-9]{1,2}(:[0-9]{2})+
VERSION			[0-9]+\.[0-9]+(\.[0-9]+)+

URL				((http)|(ftp))s?:\/\/[A-Za-z0-9]+([\-\.]{1}[A-Za-z0-9]+)*\.[A-Za-z0-9]{2,}(:[0-9]{1,})?(\/[A-Za-z0-9_~\.\-]*)*
EMAIL 			[A-Za-z0-9_\-\.]+@([A-Za-z0-9_\-]+\.)+[A-Za-z0-9_\-]{2,4}

ATTRIBUTE 		[ \n\t]+(([A-Za-z\-_]+)?[ \n\t]*=?[ \n\t]*((\"[^\"]*\")|([A-Za-z0-9]+)|({URL}))[ \n\t]*)+[ \n\t]*
STARTTAG 		<!?[A-Za-z0-9]+{ATTRIBUTE}*[\/]?>
ENDTAG 			<[\/][A-Za-z0-9]+>

%%
{EMAIL}				downcase_and_print(yytext);
{URL}				downcase_and_print(yytext);
{PHONENUMBER}			downcase_and_print(yytext);
{FLOAT}				downcase_and_print(yytext);
{NUMBER}			;
{TIME}				downcase_and_print(yytext);
{VERSION}			downcase_and_print(yytext);
{STARTTAG}			;
{ENDTAG}			;
{UPPERWORD}			downcase_and_print(yytext);
{ABBREVIATION}			downcase_and_print(yytext);
{HYPHENWORD}			downcase_and_print(yytext);
{WORD}				downcase_and_print(yytext); 
[\n\t ]				;
.				;
%%
// global variables for the input directory pointer
// and output directory name for use in yywrap
DIR *InputDirPtr = NULL;
string InDirname;
string OutDirname;
bool OutputFileIsOpen = false;


// This is called once per file.
int yywrap()
{
struct dirent* InputDirEntryPtr;
string InFilename;
string OutFilename;


   // if there was an input file open 
   // close it to get ready for the next file 
   if (yyin != NULL)
   {
      fclose (yyin);
      yyin = NULL;
      
      //***  Just closed the file
      
      ProcessDocumentHashtable();
      docId++;
      fileTotalTokens = 0;
   }

   // if there was an output file open
   // close it to get ready for the next file 
   if (OutputFileIsOpen)
   {
      fclose (yyout);
      OutputFileIsOpen = false;
   }

   // skip over the hidden filenames that begin with dot
   do
   {
      InputDirEntryPtr = readdir (InputDirPtr);
   } while ((InputDirEntryPtr != NULL) &&
            (InputDirEntryPtr->d_name[0] == '.'));

   // if there are still files to process
   if(InputDirEntryPtr !=NULL) 
   {
      // open the next file in the list as yyin
      InFilename = InDirname + '/'+ InputDirEntryPtr->d_name;
      cout << "\nProcessing file " << InFilename << endl;
      document_Num++;


      invFile.writeMapRecord(docId, InputDirEntryPtr->d_name);
      
      
      yyin = fopen (InFilename.c_str(), "r");

      
      // if file open failed, print an error
      if(yyin == NULL) 
         perror(InFilename.c_str());
   }
   
   // if yyin is NULL, return 1, else return 0
   return (yyin==NULL);
}

int main(int argc, char **argv)
{
DIR *OutputDirPtr = NULL;


   if (argc != 3)
   {
      cerr << "Incorrect number of arguments.\n";
      return (1);
   }
      
   InDirname = argv[1];
   OutDirname = argv[2];
   InputDirPtr = opendir (InDirname.c_str());

   // open the input directory
   if (!InputDirPtr)
      cerr << "Unable to open input directory: " << InDirname << endl;
   else
   {
      // open or create the output directory
      OutputDirPtr = opendir (OutDirname.c_str());
      if (!OutputDirPtr)
      {
         string Command = "mkdir -p "+ OutDirname;
         system (Command.c_str());
      }   
      invFile.openForWrite(OutDirname);
      // initialize the input and output file pointers
      yyin = NULL;
      yyout = NULL;

      // call yywrap and yylex to process the files
      yywrap();
      yylex();
      cout << "\nTotal tokens in document collection: " << globalTotalTokens << endl;

      // close the input and output directories
      (void) closedir (InputDirPtr);
      (void) closedir (OutputDirPtr);
   }
   cout << "Done tokenizing.  Good place to write the dict and post files."  << endl;
   cout << "Final numbers of document: " << document_Num << endl;
   // Method from global hashtable that writes dict to post
   globalHt.WriteToDict(invFile, document_Num);
   invFile.closeForWrite();
}
