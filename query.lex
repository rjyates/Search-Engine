/*---------------------------------------------------*/
/* Filename:  query.flex                              */
/* Take a query from the commandline,                */
/*                   preprocess that using flex      */
/*To compile: flex query.flex                         */
/*            g++ -o query lex.yy.c -lfl             */
/*To run:    ./query bunch_of_query_terms invertedFileDirectory  */
/*e.g.,      ./query state-of-the-art Technology! output   */
/*           outputs:  state of the art technology   */
/*Author:  Susan Gauch                               */
/*---------------------------------------------------*/
%{
#include <iostream>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "invertedFile.h"
#include "accumulator.h"
#include "queryList.h"
extern int yylex(void);
using namespace std;
const unsigned long SIZE = 10;
invertedFile inv;
accumulator accumulate(SIZE);
queryList list;

string term = "";
int numdocs = 0;
int start = 0;
int docid = 0;
float weight = 0;

string QueryBuffer = "";

void quickTokenize(char* filteredQuery, const string input);
void downcase(char* text, const int length);
void processQuery(char* filteredQuery, string directory);

//parses arguments, then prints prints them back
// it assumes that the last argument is the inverted file
int main(int argc, char **argv){

	int start = 1;
	int end = argc;

	string input = "";

	for(int i = start; i < argc - 1; i++){
		input += argv[i];
		input += " ";
	}
	printf("input Query: %s\n", input.c_str());

    string  directory = argv[argc-1];
    cout << "directory: " << directory << endl;

	char filteredQuery[input.size()];
	quickTokenize(filteredQuery, input);

    processQuery(filteredQuery, directory);

}

%}

/*----------------------------------------------------------*/
/*Section of code which specifies lex substitution patterns */
/*----------------------------------------------------------*/

DIGIT                   [0-9]
LETTER                  [A-Za-z]
UPPERCASE               [A-Z]
LOWERCASE               [a-z]
ALPHANUM                [A-Za-z0-9]

WORD                    [A-Za-z][a-z]*
UPPERWORD               [A-Z0-9][A-Z0-9]*
HYPHENWORD              [A-Za-z]+(\-[A-Za-z]+)+
ABBREVIATION    [A-Za-z]+\.([A-Za-z]+\.)+

NUMBER                  [0-9]+(,[0-9]+)*
FLOAT                   [0-9]*\.[0-9]+
PHONENUMBER             [0-9]{3}-[0-9]{3}-[0-9]{4}
TIME                    [0-9]{1,2}(:[0-9]{2})+
VERSION                 [0-9]+\.[0-9]+(\.[0-9]+)+

URL                             ((http)|(ftp))s?:\/\/[A-Za-z0-9]+([\-\.]{1}[A-Za-z0-9]+)*\.[A-Za-z0-9]{2,}(:[0-9]{1,})?(\/[A-Za-z0-9_~\.\-]*)*
EMAIL                   [A-Za-z0-9_\-\.]+@([A-Za-z0-9_\-]+\.)+[A-Za-z0-9_\-]{2,4}

ATTRIBUTE               [ \n\t]+(([A-Za-z\-_]+)?[ \n\t]*=?[ \n\t]*((\"[^\"]*\")|([A-Za-z0-9]+)|({URL}))[ \n\t]*)+[ \n\t]*
STARTTAG                <!?[A-Za-z0-9]+{ATTRIBUTE}*[\/]?>
ENDTAG                  <[\/][A-Za-z0-9]+>

%%
{EMAIL}                         downcase(yytext, yyleng);
{URL}                           downcase(yytext, yyleng);
{PHONENUMBER}                   downcase(yytext, yyleng);
{FLOAT}                         downcase(yytext, yyleng);
{NUMBER}                        ;
{TIME}                          downcase(yytext, yyleng);
{VERSION}                       downcase(yytext, yyleng);
{STARTTAG}                      ;
{ENDTAG}                        ;
{UPPERWORD}                     downcase(yytext, yyleng);
{ABBREVIATION}                  downcase(yytext, yyleng);
{HYPHENWORD}                    downcase(yytext, yyleng);
{WORD}                          downcase(yytext, yyleng);
[\n\t ]                         ;
.                               ;
%%


#undef yywrap
int yywrap(){
	return 1;
}

//performs lex tokenizing on input stores results in a c string
void quickTokenize(char* filteredQuery, const string input){
	QueryBuffer =  "";
	yyin = tmpfile();
	fprintf(yyin, "%s", input.c_str());
	rewind(yyin);
	yylex();
	fclose (yyin);
	strcpy(filteredQuery, QueryBuffer.c_str());
	QueryBuffer =  "";
}

//downcases
void downcase(char* text, const int length){
	char line[100];
	int i = 0;
	
	for(i = 0; i < length; i++){
		if(text[i] >= 'A' && text[i] <= 'Z'){
			line[i] = text[i] - 'A' + 'a';
		}else{
			line[i] = text[i];
		}
	}
	
	line[min(i, 100)] = '\0';
	QueryBuffer.append(line);
	QueryBuffer.append(" ");
}

void processQuery (char* queryString, string directory)
{

cout << "The query is:  " << queryString << endl;
string dictFilename = directory+"/dict.txt";
string postFilename = directory+"/post.txt";
string map = directory+"/map.txt";

inv.openForRead("tinyout"); //open the inverted files

// for each token in queryString (separated by space) do
char *token = strtok (queryString, " ");   // get the first token

   while (token != NULL)
   {
      cout << endl;
      printf ("%s\n", token);   // print the next token to the screen for debugging
      
      unsigned long Index = inv.Find(token); //find key and get index of key

      inv.readDictRecord(Index, term, numdocs, start); //read index of key from dict
      cout << "dict record: " << Index << " " << term << " " << numdocs << " " << start << endl;
       
      int temp = start; 
     for(int i=0; i<numdocs; i++){ 
        inv.readPostRecord(temp, docid, weight); //read index of key from post
	cout << "post record: " << temp << " " << docid << " " << weight << endl;
	string doci = to_string(docid);
	accumulate.InsertAdd(doci, weight);
	temp = temp + 1;
     }

      token = strtok (NULL, " ");  // get the next token
      
   }
   inv.closeForRead(); //close the inverted files


	cout << endl;
   cout << "-------------------------" << endl;
  // cout << "accumulator " << endl;
   accumulate.Print();

}

