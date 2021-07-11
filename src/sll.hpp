template <typename T>
class SinglyLinkedList
{
public:
	struct Node
	{
		T data;
		Node *next;
	};

	Node *head;

public:
	SinglyLinkedList();
	void insert(Node *prev, Node *node);
	void remove(Node *prev, Node *node);
};

template<typename T>
SinglyLinkedList<T>::SinglyLinkedList() {}

template<typename T>
void SinglyLinkedList<T>::insert(Node *prev, Node *node) 
{
	if (prev == nullptr) // insert in the begining of list
	{
		// Is the first node
		if (head != nullptr) 
		{
			node->next = head;
		} 
		else 
		{
			node->next = nullptr;
		}
		head = node;
	}
	else 
	{
		if (prev->next == nullptr)
		{
			prev->next = node;
			node->next = nullptr;
			// Is the last node
		}
		else 
		{
			// Is a middle node
			node->next = prev->next;
			prev->next = node;
		}
	}
}

template<typename T>
void SinglyLinkedList<T>::remove(Node *prev, Node *node) 
{
  if (prev == nullptr) 
	{
    // Is the first node
    if (node->next == nullptr) 
		{
      // List only has one element
      head = nullptr;
    } 
		else 
		{
      // List has more elements
      head = node->next;
    }
  } 
	else 
	{
    prev->next = node->next;
  }
}

