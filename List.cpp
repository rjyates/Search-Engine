#include <fstream>
#include <iostream>

#include "List.h"

using namespace std;      

// ------------------------ constructors and destructors ----------------

//-----------------------------------------------------------------
// Function Name:  The default constructor
// Parameters:  none
// Return Value: none
// Purpose:  Initialize the list to empty
//-----------------------------------------------------------------
List::List()
{
   Head = NULL;
   Tail = NULL;
}

//-----------------------------------------------------------------
// Function Name:  The copy constructor
// Parameters:  none
// Return Value: none
// Purpose:  Initialize the list to be a copy of some other list
//-----------------------------------------------------------------
List::List(const List &Other)
{
NodePtr Curr = Other.Head;        // the current node in the original list

   Head = NULL;               // copy is initially empty
   Tail = NULL;
   
   // loop through the original list, copying nodes one at a time
   while (Curr != NULL)
   {
     AddToEnd (Curr->docId, Curr->weight);  
     Curr = Curr->Next;
   }
};     

//-----------------------------------------------------------------
// Function Name:  The destructor
// Parameters:  none
// Return Value: none
// Purpose:  Delete all the nodes in the list, freeing the space
//-----------------------------------------------------------------
List::~List()
{
NodePtr Temp;

   // loop through whole list deleting nodes
   while (Head != NULL)
   {
      Temp = Head;
      Head = Head->Next;
      delete Temp;
   }
}

// ----------------------- list operations ------------------------------

//-----------------------------------------------------------------
// Function Name:  IsEmpty
// Parameters:  none
// Return Value:  true if the list is empty, false otherwise
// Purpose:  Check whether or not the list is empty
//-----------------------------------------------------------------
bool List::IsEmpty() const
{
   return (Head == NULL);
}


//-----------------------------------------------------------------
// Function Name:  AddToFront
// Parameters:  Item - input - the info to be added
// Return Value: none
// Purpose:  Add the new entry to the front of the linked list.
//-----------------------------------------------------------------
void List::AddToFront (int docId, float weight)
{
NodePtr NewNode;

   // make the space for the new node and store the data
   NewNode = new Node;
   NewNode->docId = docId;
   NewNode->weight = weight;

   // empty list case
   if(Head==NULL) 
      Tail=NewNode;
     
   // point from this node to the previous head of the list
   NewNode->Next = Head;

   // make the head of the list point to this node
   Head = NewNode;
}


//-----------------------------------------------------------------
// Function Name:  AddToEnd
// Parameters:  Item - input - the info to be added
// Return Value: none
// Purpose:  Add the new entry to the tail of the linked list.
//-----------------------------------------------------------------
void List::AddToEnd (int docId, float weight)
{
NodePtr NewNode;

   // make the space for the new node and store the data
   NewNode = new Node;
   NewNode->docId = docId;
   NewNode->weight = weight;
   NewNode->Next=NULL;
   
   if(Tail==NULL) 
       Head=NewNode;
   else 
       Tail->Next=NewNode;
   Tail=NewNode;
}


//-----------------------------------------------------------------
// Function Name:  AddSorted
// Parameters:  Item - input - the info to be added
// Return Value: none
// Purpose:  Add the new entry to the linked list, sorted in
//           increasing order.
//-----------------------------------------------------------------
void List::AddSorted (int docId, float weight)
{
NodePtr NewNode, Curr, Prev;

   // First make the new node and store the data 
   NewNode = new Node;
   NewNode->docId = docId;
   NewNode->weight = weight;

   // Find the node prior to this one in the list 
   Find (weight, Curr, Prev);

   // If Prev is NULL, we're at the head of the list 
   if (Prev == NULL)
   {
      NewNode->Next = Head;
      Head = NewNode;
   }
   // else, insert between Prev and Curr
   else
   {
      Prev->Next = NewNode;
      NewNode->Next = Curr;
   }

   // If this is the last node in the list, move the tail pointer. 
   if (NewNode->Next == NULL)
      Tail = NewNode;
}


//-----------------------------------------------------------------
// Function Name:  Print
// Parameters:  none
// Return Value: none
// Purpose:  Print the contents of the list to stdout.
//-----------------------------------------------------------------
void List::Print() const
{
cout<<"The present contents of the list: "<<endl;
NodePtr Temp = Head;

   // loop through whole list printing nodes
   while (Temp != NULL)
   {
      cout << "docId: " << Temp->docId << " weight: " << Temp->weight << endl;
      Temp = Temp->Next;
   }
   cout << endl;
}

string List::testPrint() const {
   string result = "The present contents of the list: \n";
   NodePtr Temp = Head;

   while (Temp != NULL) {
      result += "docId: " + std::to_string(Temp->docId) + "\n";
      result += "weight: " + std::to_string(Temp->weight) + "\n";
      Temp = Temp->Next;
   }
   // result += "\n";
   return result;
}


//-----------------------------------------------------------------
// Function Name:  Delete
// Parameters:  none
// Return Value: none
// Purpose:  Delete a node based on the 'Item' 
//-----------------------------------------------------------------
void List::Delete(int docId)
{
NodePtr Curr; 
NodePtr Prev;
	
   // Find the node prior to the one that contains Item
   Find (docId, Curr, Prev);
	
   if (Curr == NULL || Curr->docId != docId)
   { 
      cout<<"\nNo such record to be deleted!"<<endl;
   }

   // Item was found
   else 
   {
      // Reset pointer to Curr 
      if (Curr != Head)
         Prev->Next = Curr->Next;
      else
	 Head = Curr->Next;

      // If Curr was the end of the list
      if (Curr == Tail)
            Tail = Prev;

      delete Curr;
      Curr = NULL;
   }
}

//-----------------------------------------------------------------
// Function Name:  Write
// Parameters:  The file name 
// Return Value: none
// Purpose:  Write the contents back to the file
//-----------------------------------------------------------------
void List::Write(const char FileName[]) const
{
ofstream outfile(FileName,ios::out);
NodePtr Temp = Head;

   // File couldn't be opened
   if (!outfile) 
   {
      cout<<"\nOutput file cannot be opened!"<<endl;
   }
   else
   {
      // loop through whole list printing nodes
      while (Temp != NULL)
      {
         outfile << Temp->docId << endl;
         Temp = Temp->Next;
      }
      outfile.close();
   }
}

// ----------------------- private methods ------------------------------

//-----------------------------------------------------------------
// Function Name:  Find
// Parameters:  Item - input - the Item to be found
//              Curr - output - pointer to the node with Item
//              Curr - output - pointer to the node before Item
// Return Value: none
// Purpose:  Locate a node (It is at Prev->Next)
// Note:  Assumes that > is defined for T.
//-----------------------------------------------------------------
void List::Find (int docId, NodePtr &Curr, NodePtr &Prev) const
{
   Curr = Head;
   Prev = NULL;

   /* find the proper position in the list */
   while ((Curr != NULL) && (docId > Curr->docId))
   {
      Prev = Curr;
      Curr = Curr->Next;
   }
}

void List::WriteList(invertedFile& invFile, int &start, float &idf) {
   // start_tracker = 0;
   NodePtr Curr = Head;
   while(Curr != nullptr) {
      int docId = Curr->docId;
      float weight = Curr->weight;
      //cout << "list " << weight << " * " << idf << " = " << weight*idf << endl;
      invFile.writePostRecord(docId, weight*idf);
      start++;
      Curr = Curr->Next;
      // start_tracker++;
   }
}

/* int List::GetStart() {
   return start_tracker;
} */
/* NodePtr List::GetHead() const {
   return Head;
} */

/* NodePtr List::GetNodePtrType() const {
   return nullptr;
} */
