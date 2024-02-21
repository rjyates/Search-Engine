/*---------------------------------------------------*/
/* Filename:  query.lex                              */
/* Take a query from the commandline,                */
/*                   preprocess that using flex      */
/*To compile: flex query.lex                         */
/*            g++ -o query lex.yy.c -lfl             */
/*To run:    ./query bunch of query terms            */
/*e.g.,      ./query state-of-the-art Technology!    */
/*           outputs:  state of the art technology   */
/*Author:  Susan Gauch                               */
/*---------------------------------------------------*/
%{
#include <iostream>
#include <algorithm>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern int yylex(void);
using namespace std;

const int NUM_UNIQUE_TERMS = 50000;  // used to set the global hashtable size
const int NUM_RECORDS_IN_DICT = NUM_UNIQUE_TERMS * 3;
const int DICT_REC_SIZE = 28;
const int POST_REC_SIZE = 16; 
const int MAP_REC_SIZE = 11;
const int NUM_DOCS_IN_COLLECTION = 1750;  // used to set the accumulator size

string QueryBuffer = "";

struct Weight {
	int docID = 0;
	float weight = 0.0;
};

void quickTokenize(char* filteredQuery, const string input);
void downcase(char* text, const int length);
void processQuery(char* filteredQuery, string directory);
bool getDictRecord(const string token, ifstream &dictFile, int &num_docs, int &start);
void getPostRecord(ifstream &postFile, const int recNum, int &docID, float &weight);
void getMapRecord(ifstream &mapFile, const int recNum, string &filename);
void presentResults (Weight accumulator[], ifstream &mapFile);
bool sorter(Weight const &lhs, Weight const &rhs);

//parses arguments, then prints prints them back
// it assumes that the last argument is the inverted file
int main(int argc, char **argv){
//cout << "<H2>Hello Dad.</H2>";
	int start = 1;
	int end = argc;

	string input = "";

	for (int i = start; i < argc - 1; i++) {
		input += argv[i];
		input += " ";
	}

	cout << "input Query: " << input << "<br>";

    string directory;
    directory = argv[argc - 1];
    cout << "directory: " << directory << "<br>";
	char filteredQuery[input.size()];
	quickTokenize(filteredQuery, input);
	cout << "filtered Query: " << filteredQuery << "<br>";

    processQuery(filteredQuery, directory);


}

%}

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
{EMAIL}				downcase(yytext, yyleng); 
{URL}				downcase(yytext, yyleng);
{PHONENUMBER}		downcase(yytext, yyleng);
{FLOAT}				; 
{NUMBER}			; 
{TIME}				downcase(yytext, yyleng); 
{VERSION}			downcase(yytext, yyleng); 
{STARTTAG}			;
{ENDTAG}			;
{UPPERWORD}			downcase(yytext, yyleng); 
{ABBREVIATION}		downcase(yytext, yyleng); 
{HYPHENWORD}		downcase(yytext, yyleng); 
{WORD}				downcase(yytext, yyleng); 
[\n\t ]				;
.
%%

#undef yywrap
int yywrap() {
	return 1;
}

//performs lex tokenizing on input stores results in a c string
void quickTokenize(char* filteredQuery, const string input) {
	QueryBuffer =  "";
	yyin = tmpfile();
	fprintf(yyin, "%s", input.c_str());
	rewind(yyin);
	yylex();
	fclose(yyin);
	strcpy(filteredQuery, QueryBuffer.c_str());
	QueryBuffer =  "";
}

//downcases
void downcase(char* text, const int length) {
	char line[100];
	int i = 0;
	
	for (i = 0; i < length; i++) {
		if (text[i] >= 'A' && text[i] <= 'Z') {
			line[i] = text[i] - 'A' + 'a';
		} else {
			line[i] = text[i];
		}
	}
	
	line[min(i, 100)] = '\0';
	QueryBuffer.append(line);
	QueryBuffer.append(" ");
}

void getMapRecord(ifstream &mapFile, const int recNum, string &filename)
{
	mapFile.seekg((recNum * MAP_REC_SIZE), ios::beg);	// Go to target record
    mapFile >> filename;
}

// Receives a token and returns the respective dict record
// Example: dog -> "      dog    13    89"
bool getDictRecord(const string token, ifstream &dictFile, int &num_docs, int &start) {
	unsigned long sum = 0;
    bool success = true;
    string tokenFound;
	
   	// Hash Function
   	for (long unsigned i = 0; i < token.length(); i++) {
		sum = (sum * 19) + token[i];
	}
   	unsigned long int hash_loc = sum % NUM_RECORDS_IN_DICT;

	// Linear probing on dict file to find record
	dictFile.seekg(hash_loc * DICT_REC_SIZE, ios::beg);					// Set beginning of dictFile
    dictFile >> tokenFound >> num_docs >> start;

	while (tokenFound != token && tokenFound != "empty") {
        dictFile >> tokenFound >> num_docs >> start;
		hash_loc++ % NUM_RECORDS_IN_DICT;
	}

	if (tokenFound == "empty") 
       success = false;

    return success;
}

void getPostRecord(ifstream &postFile, const int recNum, int &docID, float &weight)
{
	postFile.seekg((recNum * POST_REC_SIZE), ios::beg);	// Go to target record in post file
    postFile >> docID >> weight;
}

void processQuery (char* queryString, string directory) {

//cout << "Inside processQuery" << "<br>";
	cout << "<h4>The query is:  " << queryString << "</h4>";
    
    // open the inverted file
	string dictFilePath = directory + "/dict.txt";
	ifstream dictFile(dictFilePath);								// Create input stream

	string postFilePath = directory + "/post.txt";
	ifstream postFile(postFilePath);

	string mapFilePath = directory + "/map.txt";
	ifstream mapFile(mapFilePath);

    // declare the accumulator and other variables
	Weight accumulator[NUM_DOCS_IN_COLLECTION];
    int numPostings;
	int postingLineNum;
	int currentDocID;
	float currentWeight;

	// for each token in queryString (separated by space) do
	char *token = strtok(queryString, " ");   // get the first token

	while(token != NULL) 
    {
		// if the token is found in the Dict file, get the rest of the record
		if (getDictRecord(token, dictFile, numPostings, postingLineNum))
        {
			// fill the accumulator
			for (int i = 0; i < numPostings; i++) 
            {
                // read a posting
                getPostRecord (postFile, postingLineNum, currentDocID, currentWeight);
				postingLineNum++;													// Inc line number

                // add posting to the accumulator
				accumulator[currentDocID].docID = currentDocID;
				accumulator[currentDocID].weight += currentWeight;
		    }
        }
		token = strtok(NULL, " ");  // get the next token
	}
	
	// sort the accumulator by wt
	sort(begin(accumulator), end(accumulator), &sorter);

	// write the results to the screen
    presentResults(accumulator, mapFile);

    // close the files
    dictFile.close();
    postFile.close();
    mapFile.close();

}

// used by sort method
bool sorter(Weight const &lhs, Weight const &rhs) {
	if (lhs.weight > rhs.weight)
       return true;
    else
        return false;
}

void presentResults (Weight accumulator[], ifstream &mapFile)
{
string filename; 

    cout << "<h2>============= Top 10 Results =================</h2>";
    // loop over the accumulator, printing the results
	for (int i = 0; i < 10; i++) {
        getMapRecord(mapFile, accumulator[i].docID, filename);
		//cout << i << ": " << accumulator[i].docID << " " << accumulator[i].weight << " " << filename << endl;
		cout << "<h4>" << i << ": " << accumulator[i].docID << " " << accumulator[i].weight << " " << filename << "</h4>";
		cout << "<a href='files/" << filename << "'>" << filename << "</a>";
		
	}

}

