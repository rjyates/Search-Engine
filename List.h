#ifndef LIST_H
#define LIST_H

#include <string> 
#include "invertedFile.h"
//-----------------------------------------------------------------
// File:  List.h
// Author:  Susan Gauch
// Purpose:  The header file for a linked list class.
//           This list is singly-linked, with a tail pointer.
//	     It also uses the TEMPLATE mechanism for the data type
//	     stored in the node.
//-----------------------------------------------------------------


class List
{
public:
// constructors and destructors
   List();                    // default constructor
   List (const List& L);      // copy constructor
   ~List();                   // destructor

// list operations
   void AddToFront (int docId, float weight);
   void AddToEnd (int docId, float weight);
   void AddSorted (int docId, float weight);
   void Delete(int docId);
   // NodePtr GetHead() const;
   // NodePtr GetNodePtrType() const;
   bool IsEmpty() const;
   void Print() const;
   std::string testPrint() const;
   // string testPrint() const;
   void Write(const char Filename[]) const;
   void WriteList(invertedFile& invFile, int& start, float& idf);
   // int GetStart();
   // friend class globalHt;
   
private:
   struct Node;
   typedef Node* NodePtr;
   // int start_tracker;
   struct Node {
    int docId;
    float weight;
    NodePtr Next;	
    };            // defined in implementation file
   void Find (int docId, NodePtr &Curr, NodePtr &Prev) const;
   NodePtr Head;              // the head of the list
   NodePtr Tail;              // the tail of the list
};

#endif
