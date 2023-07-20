// Muhammad Asad Tariq (21L-5266) --- BCS-3F
// Data Structures --- Assignment 1

#include <iostream>
#include <string>
using namespace std;

class Block;
class Program;

template <class T>
class List				// singly linked list with dummy nodes (without iterators)
{
private:
	
	class Node
	{
	public:

		T* data;	// data kept as pointer for easier shifting of memory blocks from pool to program and vice versa via the memory management system

		Node* next;

		Node(T* _data = NULL, Node* _next = NULL)
		{
			data = _data;
			next = _next;
		}

		~Node()
		{
			if (data)
				delete data;
		}
	};

	Node* head;
	Node* tail;

	int size;

public:

	List()
	{
		tail = new Node();					// dummy tail node
		head = new Node(NULL, tail);		// dummy head node

		size = 0;
	}

	~List()
	{
		for (Node* temp = head; temp; temp = head)		// even if the list is empty, destructor is still needed to delete the two dummy nodes, so no !isEmpty() check
			{
				head = head->next;
				delete temp;
			}											// head = 0 by the end of the loop, no need to set tail = NULL and size = 0 as the list is about to be destroyed
	}

	bool isEmpty()
	{
		return size == 0;	// an alternative could be (head->next == tail) but since we are keeping the size of the list, it is simpler
	}

	void print()		// for displaying elements of the list (other than the dummy head and tail nodes)
	{
		if (!isEmpty())
			for (Node* temp = head->next; temp != tail; temp = temp->next)
				cout << *(temp->data) << endl;		// operator overloading for the Block and Program class makes this work
		else
			cout << "\t\t\tEMPTY - NONE FOUND!\n";
	}

	void addToHead(T* val)
	{
		head->next = new Node(val, head->next);		// since dummy nodes are being used, no need for special treatment of boundary conditions

		size++;
	}

	void addToTail(T* val)
	{
		tail->data = val;				// old dummy tail node changed into an actual node

		tail->next = new Node();		// new dummy node
		tail = tail->next;				// tail shifted to the new dummy node

		// this strategy was used to keep the time complexity of this function O(1) as the usual implementation would make it O(n)
		
		size++;
	}

	bool deleteFromHead()
	{
		if (!isEmpty())
		{
			Node* temp = head->next;
			head->next = temp->next;
			delete temp;				// this is the deletion of the first actual node, not the dummy head node

			size--;

			return true;				// function has been made bool so that if deletion does not take place due to the list being empty, the caller is informed
		}
		else
			return false;
	}

	bool deleteFromTail()
	{
		if (!isEmpty())
		{
			Node* temp = head;
			while (temp->next->next != tail)	// since this is a singly-linked list, we need to traverse the entire array to reach the node just behind the tail
				temp = temp->next;				// hence this function has time complexity O(n) rather than O(1) like the others defined above
			delete temp->next;					// this is deletion of the last actual node, not the dummy tail node
			temp->next = tail;

			size--;

			return true;
		}
		else
			return false;
	}



	void swapNodes(Node* node1, Node* node2)	// this actually swaps just the data of the nodes, not the nodes themselves as that is unnecessary here
	{											// this function is used during the sorting of the list
		T* temp = node1->data;
		node1->data = node2->data;
		node2->data = temp;
	}

	void deleteNode(Node* ptr)	// to delete a specific node (using a strategy similar to addToTail() to keep it O(1)
	{
		if (ptr->next != tail)	// if the node is not the last (i.e. the one just before the dummy tail node)
		{
			if (ptr->data)
				delete ptr->data;			// deallocate its data to prevent memory leak
			ptr->data = ptr->next->data;	// data of the next node copied into it

			Node* temp = ptr->next;
			ptr->next = temp->next;			// the next node skipped from the continuity of the list

			temp->data = NULL;				// to prevent ptr->data from becoming a dangling pointer upon deletion of temp
			delete temp;					// the next node deleted

											// NOTE: this strategy (taking data of the next node and then deleting it) was used to keep this operation O(1) 
		}
		else                                // if the node is the last node, a slight modification is made to the above strategy (the last node is converted into the dummy tail)
		{									// and the old dummy tail is deleted
			delete tail;
			if (ptr->data)
				delete ptr->data;
			ptr->next = NULL;				
			tail = ptr;
		}

		size--;
	}

	void append(List<T>* list)//		// to add a list at the end of another list
	{
		if (!list->isEmpty())
		{
			tail->next = list->head->next->next;	// converting tail into first actual node of other list, with the second being its next
			tail->data = list->head->next->data;

			list->head->next->data = NULL;			// deleting the dummy head and the now useless first node of that list
			delete list->head->next;
			delete list->head;

			tail = list->tail;						// shifting tail to the new tail, i.e. the tail of the list that has just been apended
			list->head = NULL;						// to prevent the new nodes from being deleted upon the later destruction of the now defunct list
			size += list->size;
		}

		delete list;	// deleting the now useless list object as all its nodes have been taken
	}


	// Functions For Use With Only List<Block>:

	void mergeBlocks()	// to traverse the pool and merge all adjacent blocks of memory
	{
		if (!isEmpty())
		{
			Node* temp = head->next;
			while (temp->next != tail)
			{
				if (temp->data->isAdjacent(temp->next->data))
				{
					temp->data->mergeBlock(temp->next->data);

					Node* tmp2 = temp->next;
					temp->next = temp->next->next;
					delete tmp2;						// the latter is deleted as its data has been merged into the former

					size--;
				}
				else
					temp = temp->next;
			}
		}
	}

	void sortList()		// sorting the list via common bubble sort algorithm, can be used generally but requires overloading the > operator, which has only been done
	{					// for class Block in this assignment as it was only needed there
		bool done = false;
		Node* temptail = tail;

		for (int i = 1; i < size && !done; i++)		// no. of passes
		{
			done = true;							// assume sorted
			for (Node* temp = head->next; temp != temptail; temp = temp->next)
			{
				if (*(temp->data) > temp->next->data)	// check if swap required
				{
					done = false;		// if swap required, assumption false

					/*T* tempdata = temp->data;
					temp->data = temp->next->data;
					temp->next->data = tempdata;*/	// alternate implementation, without swap function

					swapNodes(temp, temp->next);		// nodes swapped
				}

				if (temp->next->next == temptail)
					temptail = temp->next;
			}
		}

	}

	Block* getBlock(int size, bool strategy)		// find block from pool based upon strategy
	{
		Block* fit = NULL;
		Node* temp = head->next;

		int fitFactor = -1;
		while (temp != tail && !fit)		// this loop is for finding first fit, whether exact or not
		{
			fitFactor = temp->data->checkFitness(size);
			if (fitFactor == 0)									// if an exact fit
			{
				fit = temp->data;
				temp->data = NULL;
				deleteNode(temp);
				fitFactor = 0;
				break;
			}
			if (fitFactor != -1)								// if not an exact fit, but still a fit
				fit = temp->data;
			temp = temp->next;
		}

		if (strategy && fitFactor != 0)		// if BEST-FIT strategy is to be used and exact fit not found yet, then we traverse the entire list
		{									// otherwise this part is skipped and the first-fit block found above is used
			int tempFactor = 0;
			while (temp != tail)
			{
				tempFactor = temp->data->checkFitness(size);
				if (tempFactor == 0)		// if an exact fit, it is naturally the best, so no need to traverse further
				{
					fit = temp->data;
					temp->data = NULL;
					deleteNode(temp);
					fitFactor = 0;
					break;
				}
				if (tempFactor != -1 && tempFactor < fitFactor)	// if not an exact fit, we keep traversing to find the one that fits better than all others
				{
					fit = temp->data;
					fitFactor = tempFactor;
				}
				temp = temp->next;
			}
		}

		if (fit != NULL && fitFactor != 0)	// if fitting lock found but not an exact fit, we break away required piece from it
				fit = fit->breakBlock(size);

		return fit;
	}

	// Functions For Use With Only List<Program>:

	Program* findProgram(string ID)		// to find and return pointer to program with given ID, or NULL in case program not found
	{
		Program* prog = NULL;

		for (Node* temp = head->next; temp != tail && !prog; temp = temp->next)
			if (*(temp->data) == ID)		// == operator overloaded for class program to compare using IDs
				prog = temp->data;

		return prog;
	}

	Program* removeProgram(string ID)	// remove prog from list of progs (but not yet delete it)
	{
		Program* prog = NULL;

		Node* temp = head;
		while (temp->next != tail && !prog)
		{
			if (*(temp->next->data) == ID)
			{
				prog = temp->next->data;
				temp->next->data = NULL;
				
				Node* tmp2 = temp->next;
				temp->next = temp->next->next;
				delete tmp2;						// the node of that program is deleted from the list of programs, but the program itself has not been deleted yet
													// as we still need it to return memory to the pool from the MMS
				size--;
			}
			temp = temp->next;
		}

		return prog;
	}
};



class Block
{
private:

	int start_byte_ID;
	int total_bytes;

public:

	Block(int _start_byte_ID, int _total_bytes)
	{
		start_byte_ID = _start_byte_ID;
		total_bytes = _total_bytes;
	}

	bool isAdjacent(Block* block)		// two blocks are adjacent if one ends at the byte number from which the other begins
	{
		if (start_byte_ID + total_bytes == block->start_byte_ID)		// the end of the LHS block is found by adding the total bytes to the strating byte
			return true;
		return false;
	}

	int checkFitness(int size)		// tells whether a certain block can be used to fulfill the request for memory of a given size, as well as the level of fitness
	{
		if (size > total_bytes)		// if the size of the block is smaller than the size of memory required then it is not fit for it
			return -1;				// lack of fitness is represented by -1; on the other hand, exact fit is represented by zero
		else
			return (total_bytes - size);	// if the block is greater than or equal to the given size, we calculate how close it is tp the given size
											// the one with level of fitness closer to zero is a BETTER fit (this is used for the determination of BEST-FIT)
	}

	void mergeBlock(Block* block)		// for merging adjacent blocks
	{
		total_bytes += block->total_bytes;		// the memory of the latter block is added to the former
		block->total_bytes = 0;					// and subtracted from the latter, which will later be deleted in the caller function
	}

	Block* breakBlock(int size)		// from those blocks which are not exact fits, we break away a piece of the required size
	{
		Block* newBlock = new Block(start_byte_ID, size);	// a block of given size is separeted

		start_byte_ID += size;		// the remaining part is resized
		total_bytes -= size;

		return newBlock;
	}

	bool operator>(Block* block)	// overloaded comparison operator, used to determine order of memory blocks for the sake of sorting
	{
		if (start_byte_ID > block->start_byte_ID)	// the block consisting of higher memory addresses comes after the block consisting of lower bytes of memory
			return true;
		return false;
	}

	friend ostream& operator<<(ostream& out, const Block& block)	// overloaded stream insertion operator, used for printing details of a block
	{
		out << "\t\t\t\t\t|\tBlock:" << "\tStart Byte ID: " << block.start_byte_ID << "\tNo. of Bytes: " << block.total_bytes;
		return out;
	}
};



class Program
{
private:

	string ID;
	int size;

	List<Block>* progMemory;	// list of memory blocks allocated to the program, kept as a pointer so that memory is more easily returned to the memory management system
								// at the end when the execution of the program gets stopped from the driver function

public:

	Program(string _ID)
	{
		ID = _ID;
		size = 0;	// size will be incremented when memory will be assigned to the program, not upon its creation

		progMemory = new List<Block>;	// this list will initially be empty (i.e. only contain dummy nodes), later memory blocks will be added to it
	}

	~Program()
	{
		if (progMemory)				// made to prevent memory leakage in case the user ends the program without ending the currently running programs
			delete progMemory;
	}

	bool operator==(string _ID)
	{
		if (ID == _ID)		// used for finding whether a program with a given ID exists in the list of programs or not
			return true;
		return false;
	}

	void assignMem(Block* block, int _size)
	{
		progMemory->addToTail(block);	// memory block of a given size is added to the progMemory list;
		size += _size;					// the size of the program is incremented accordingly
	}

	List<Block>* returnMem()
	{
		List<Block>* memory = progMemory;		// the list of blocks allocated to the program is returned to the memory management system (which returns it to the pool)
		progMemory = NULL;						// this is done to prevent the blocks returned to the pool from getting deleted by the destructor of this program
		return memory;
	}

	friend ostream& operator<<(ostream& out, const Program& program)	// overloaded stream insertion operator, use for printing details of a program
	{
		out << "\t\t|\tProgram:" << "\tID: " << program.ID << "\tSize: " << program.size << endl;
		out << "\t\t\t\t\tMemory Blocks Allocated to Program:" << endl;
		program.progMemory->print();	// printing list of blocks of memory allocated to program using the print() function of the list
		return out;
	}
};



class MemoryManagementSystem
{
private:

	List<Block> pool;			// linked list of blocks of free memory
	List<Program> progs;		// linked list of programs currently in execution (each having its own list of blocks of memory assigned to it)

	const int sizeOfRAM;
	const bool strategy;		// FALSE means FIRST-FIT strategy whereas TRUE means BEST-FIT strategy

public:

	MemoryManagementSystem(int _sizeOfRAM, bool _strategy) : sizeOfRAM(_sizeOfRAM), strategy(_strategy)		// member initialiser list used to initialise const variables of class
	{
		Block* memory = new Block(0, sizeOfRAM);		// initially, pool is supposed to consist of one huge block of memory the size of the entire RAM
		pool.addToHead(memory);							// since it is the first actual node of the list, using addtoHead() or addToTail() is equivalent here
	}

	// do not need to create destructor as there is no dynamically allocated data member of this class (i.e. Memory Management System)
	// upon destruction of this object, the destructor of List class will automatically be called for both pool and progs

	bool GetMem(string ID, int size)
	{
		Block* block = pool.getBlock(size, strategy);	// we find block of required size from pool using specified strategy

		if (block)		// if such a block has been found, otherwise block = NULL and program will go to the 'else' part
		{
			Program* program = progs.findProgram(ID);		// we find program of given ID from the list of programs

			if (program == NULL)	// if a program with the given ID does not exist, we create a new one
			{
				program = new Program(ID);
				progs.addToTail(program);
			}
			
			program->assignMem(block, size);	// we assign the returned memory block of the required size to the [new or existing] program with the given ID

			return true;	// action performed successfully
		}
		else
			return false;
	}

	bool deleteProgram(string ID)
	{
		Program* program = progs.removeProgram(ID);		// we find and remove (but not yet delete) the program having given ID from the list of programs

		if (program)	// if the program is found in the list
		{
			List<Block>* retMemory = program->returnMem();		// we make it return its list of memory blocks assigned to it

			pool.append(retMemory);		// the returned list is appended to the end of the pool of free memory blocks
			pool.sortList();			// the pool is sorted (via bubble sort) so that adjacent blocks come together
			pool.mergeBlocks();			// adjacent blocks are merged to keep the pool as defragmented as possible

			delete program;//
			return true;		// action performed successfully
		}
		else
			return false;

		
		return false;
	}

	void showPool()
	{
		cout << "\t\tPool of Available Memory Blocks:\n\n";
		pool.print();
	}

	void showProgs()
	{
		cout << "\t\tList of Programs Currently Running:\n\n";
		progs.print();
	}
};



void main()		// driver function
{
	int sizeOfRAM = 0;
	bool strategy = false;

	cout << "Enter size of memory (in bytes): ";
	cin >> sizeOfRAM;

	cout << "Enter 0 for First-Fit or 1 for Best-Fit strategy: ";
	cin >> strategy;

	if (sizeOfRAM <= 0)		// sizeOfRAM has to be a positive integer
	{
		cout << "Your value for the size of RAM is invalid. The default value of 102400 will be used instead." << endl;
		sizeOfRAM = 102400;
	}

	MemoryManagementSystem MMS(sizeOfRAM, strategy);	// MMS object created with specified memory size and memory allocation strategy

	cout << endl << "----------------------------------------------------------------------------------------------------" << endl << endl;

	while (true)	// for repeated prompting for commands
	{
		cout << "\tEnter 1 to start a new program or assign more memory to an existing program.\n";
		cout << "\tEnter 2 to end a program.\n";
		cout << "\tEnter 3 to view the pool of available memory.\n";
		cout << "\tEnter 4 to view the programs currently running.\n";
		cout << "\tEnter 5 to exit this program.\n";

		int choice = 0;
		cout << "\n\tCommand: ";
		cin >> choice;
		cout << endl;

		if (choice == 1)	// Assigning memory to a new or existing program
		{
			string ID;
			int size = 0;

			cout << "\t\tEnter Program ID: ";
			cin.ignore();
			getline(cin, ID);

			cout << "\t\tEnter size of memory required: ";
			cin >> size;

			cin.clear();
			cin.ignore();//

			if (MMS.GetMem(ID, size))
				cout << "\n\tMemory Allocation to Program " << ID << " Successful!" << endl;
			else
				cout << "\n\tMemory Allocation Unsuccessful! Sufficient Memory Not Available in RAM." << endl;
		}

		else if (choice == 2)	// Stopping execution of a program and hence deallocating all memory assigned to it
		{
			string ID;

			cout << "\t\tEnter Program ID: ";
			cin.ignore();
			getline(cin, ID);

			if (MMS.deleteProgram(ID))
				cout << "\n\tProgram " << ID << " Ended Successfully!" << endl;
			else
				cout << "\n\tProgram Deletion Unsuccessful! Specified Program Does Not Exist." << endl;
		}

		else if (choice == 3)	// displaying pool of free memory
			MMS.showPool();

		else if (choice == 4)	// displaying list of programs currently in execution and the blocks of memory assigned to each
			MMS.showProgs();

		else if (choice == 5)	// exiting the interface
			break;

		else
			cout << "\tInvalid input!\n";

		cout << endl << "----------------------------------------------------------------------------------------------------" << endl << endl;
	}

	cout << "----------------------------------------------------------------------------------------------------" << endl << endl;
	cout << "The program has ended." << endl;
	system("pause");
}