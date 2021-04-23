//
// Created by ivan on 18.3.21.
//

#ifndef RELOAD_LIST_H
#define RELOAD_LIST_H

#include "precompiled.h"

#include <cassert>
#include "Common.h"
#include "ReloadLib/sys/Heap.h"

/*
================================================================================
List<T>::NewInternal

DESCRIPTION:
Internal method for creating a new list to the given number of elements, and
if needed zeroing it out.

RETURNS:
Pointer to the list.
================================================================================
*/
template<typename T>
inline T * NewArray(int newSize, bool zeroBuffer) {
    T * ptr;

    if (zeroBuffer) {
        ptr = static_cast<T *>(Mem::ClearedAlloc(sizeof(T) * newSize));
    }
    else {
        ptr = static_cast<T *>(Mem::Alloc(sizeof(T) * newSize));
    }

    for (int i = 0; i < newSize; i++) {
        new (&ptr[i]) T;
    }

    return ptr;
}

/*
================================================================================
List<T>::DeleteInternal

DESCRIPTION:
Internal method that calls the destructors on all the elements in the list,
and frees up the allocated memory.
================================================================================
*/
template<typename T>
inline void DeleteArray(T *list, int size) {
    for (int i = 0; i < size; i++) {
        list[i].~T();
    }
    Mem::Free(list);
}


/*
================================================================================
List<T>::ResizeInternal

DESCRIPTION:
Internal method for resizing list to the given number of elements, and if needed
zeroing it out.
================================================================================
*/
template<typename T>
inline T * ResizeArray(T * oldList, int oldSize, int newSize, bool zeroBuffer) {
    T * newList = nullptr;

    if (newSize > 0) {
        newList = NewArray<T>(newSize, zeroBuffer);
        int overlap = std::min(oldSize, newSize);

        for (int i = 0; i < overlap; i++) {
            newList[i] = oldList[i];
        }
    }

    DeleteArray<T>(oldList, oldSize);
    return newList;
}

template<typename T>
class List {
public:
    typedef int		cmp_t(const T *, const T * );
    typedef T	    new_t();

    explicit        List(int newGranularity = 16);
                    List(const List &other);
                    ~List();

    List<T> &       operator=(const List<T> &other);
    const T  &      operator[](int index) const;
    T &             operator[](int index);

    T *             Data();                                                     // Gets the pointer to the underlying array.
    const T *       Data() const;                                               // Gets the pointer to the underlying array.

    T &             Alloc();                                                    // Allocates new element at the end. Returns reference to the element.
    int             Add(T const & obj);                                         // Adds element to the end. Returns the index of the element.
    int             AddUnique(T const & obj);                                   // Adds unique element to the end. Returns the index of the element.
    int             AddRange(const List & other);                               // Appends another list to the end. Returns the appended list's first element index.
    int             Insert(T const & obj, int index = 0);                       // Inserts element at the given index.
    bool			RemoveIndex(int index);				    			        // Removes the element at the given index.
    bool			RemoveIndexFast(int index);                                 // Removes the element at the given index and places the last element into its spot - DOES NOT PRESERVE LIST ORDER.
    bool			Remove(T const & obj);              						// Removes the given element.
//    void            Sort();                                                   // Sorts the list.
//    void            Swap(List & other);                                       // Swaps the contents of the list.
    void            Clear();                                                    // Clears the list.
    void            DeleteContents(bool clear = true);                          // Deletes the contents of the list.

    void			Condense();											        // Resizes list to exactly the number of elements it contains or frees up memory if empty.
    void			Resize(int newSize);								        // Resizes list to the given number of elements
    void			Resize(int newSize, int newGranularity);			        // resizes list and sets new m_granularity
    void			SetSize(int newCount);							            // Set number of elements in list and resize to exactly this number if needed
    void			AssureSize(int newSize);							        // Assure list has given number of elements, but leave them uninitialized
    void			AssureSize(int newSize, const T &initValue);	            // Assure list has given number of elements and initialize any new elements
    void			AssureSizeAlloc(int newSize, new_t *allocator);	            // Assure the pointer list has the given number of elements and allocate any new elements.
    void			SetGranularity( int newGranularity );				        // Sets new m_granularity

    int				IndexOf(T const & obj) const;   				            // Finds and returns the index for the given element reference.
    int				IndexOf(T const * objPtr) const;	    				    // Finds and returns the index for the given element pointer.
    T *             Find(T const & obj) const;                                  // Finds and returns a pointer to the given element reference.
    [[nodiscard]] int	FindNull() const;									    // Finds and returns the index for the first NULL pointer in the list.
    [[nodiscard]] int   Size() const;                                           // Gets the number of elements in the list.
    [[nodiscard]] int	Granularity() const;								    // Gets the current m_granularity.
    [[nodiscard]] int   AllocatedCount() const;                                 // Gets the number of elements currently allocated for.
    [[nodiscard]] size_t SizeInBytes() const;                                   // Gets the total size of list in bytes.
    [[nodiscard]] size_t MemoryUsed() const;                                    // Gets the total memory used by the list.
    [[nodiscard]] size_t Allocated() const;                                     // Gets the number of allocated space in the list.

private:
    int 		    m_size;
    int 			m_capacity;
    int 			m_granularity;
    T * 		    m_list;
};


/*
================================================================================
List<T>::List

DESCRIPTION:
List's constructor with m_granularity as parameter. Default m_granularity is set to
16.
================================================================================
*/
template<typename T>
inline List<T>::List(int newGranularity) {
    assert(newGranularity > 0);

    m_size = 0;
    m_capacity = 0;
    m_granularity = newGranularity;
    m_list = nullptr;
}


/*
================================================================================
List<T>::List

DESCRIPTION:
The list's copy constructor.
================================================================================
*/
template<typename T>
inline List<T>::List(const List &other) {
    m_list = nullptr;
    *this = other;
}

/*
================================================================================
List<T>::~List

DESCRIPTION:
The list's destructor.
================================================================================
*/
template<typename T>
inline List<T>::~List() {
    Clear();
}

/*
================================================================================
List<T>::Data

DESCRIPTION:
Gets the pointer to the underlying array.
================================================================================
*/
template<typename T>
const inline T * List<T>::Data() const {
    return m_list;
}

/*
================================================================================
List<T>::Data

DESCRIPTION:
Gets the pointer to the underlying array.
================================================================================
*/
template<typename T>
inline T * List<T>::Data() {
    return m_list;
}

/*
================================================================================
List<T>::Clear

DESCRIPTION:
Frees up the memory allocated by the list.
================================================================================
*/
template<typename T>
inline void List<T>::Clear() {
    if(m_list) {
        DeleteArray<T>(m_list, m_capacity);
    }

    m_list = nullptr;
    m_size = 0;
    m_capacity  = 0;
}

/*
================================================================================
List<T>::DeleteContents

DESCRIPTION:
Calls the destructor of all elements in the list.  Conditionally frees up memory
used by the list.

NOTE:
This only works on lists containing pointers to objects and will cause a
compiler error if called with non-pointers. Since the list was not responsible
for allocating the object, it has no information on whether the object still
exists or not, so care must be taken to ensure that the pointers are still valid
when this function is called.  Function will set all pointers in the list to
`nullptr`.
================================================================================
*/
template<typename T>
inline void List<T>::DeleteContents(bool clear) {
    for(int i = 0; i < m_size; i++) {
        delete m_list[i];
        m_list[i] = nullptr;
    }

    if (clear) {
        Clear();
    } else {
        memset(m_list, 0, m_capacity * sizeof(T));
    }
}

/*
================================================================================
List<T>::Allocated

RETURNS:
Total memory allocated for the list in bytes.

NOTES:
Doesn't take into account additional memory allocated by T
================================================================================
*/
template<typename T>
inline size_t List<T>::Allocated() const {
    return m_capacity * sizeof(T);
}

/*
================================================================================
List<T>::SizeInBytes

RETURNS:
Total size of list in bytes.

NOTES:
Doesn't take into account additional memory allocated by T
================================================================================
*/
template<typename T>
inline size_t List<T>::SizeInBytes() const {
    return sizeof(List<T>) + Allocated();
}

/*
================================================================================
List<T>::MemoryUsed

RETURNS:
The total memory used by the list.
================================================================================
*/
template<typename T>
inline size_t List<T>::MemoryUsed() const {
    return m_size * sizeof(*m_list);
}

/*
================================================================================
List<T>::Size

RETURNS:
The number of elements currently contained in the list.

NOTE:
This is NOT an indication of the memory allocated.
================================================================================
*/
template<typename T>
inline int List<T>::Size() const {
    return m_size;
}

/*
================================================================================
List<T>::AllocatedCount

RETURNS:
The number of elements currently allocated for.
================================================================================
*/
template<typename T>
inline int List<T>::AllocatedCount() const {
    return m_capacity;
}

/*
================================================================================
List<T>::SetSize

DESCRIPTION:
Sets the number of elements and resizes the list to match.
================================================================================
*/
template<typename T>
inline void List<T>::SetSize(int newSize) {
    assert(newSize >= 0);

    if (newSize > m_capacity) {
        Resize(newSize);
    }

    m_size = newSize;
}

/*
================================================================================
List<T>::SetGranularity

DESCRIPTION:
Sets the base size of the array and resizes the list to match.
================================================================================
*/
template< typename T>
inline void List<T>::SetGranularity( int newGranularity ) {
    assert(newGranularity > 0);

    if (m_list) {
        int newSize;
        m_granularity = newGranularity;

        newSize = m_size + m_granularity - 1;
        newSize -= newSize % m_granularity;
        if (newSize != m_capacity ) {
            Resize(newSize);
        }
    }
}

/*
================================================================================
List<T>::Granularity

DESCRIPTION:
Get the current m_granularity.
================================================================================
*/
template<typename T>
inline int List<T>::Granularity() const {
    return m_granularity;
}

/*
================================================================================
List<T>::Condense

DESCRIPTION:
Resizes the list to exactly the number of elements it contains or frees up
memory if empty.
================================================================================
*/
template<typename T>
inline void List<T>::Condense() {
    if (m_list) {
        if (m_size) {
            Resize(m_size);
        } else {
            Clear();
        }
    }
}

/*
================================================================================
List<T>::Resize

DESCRIPTION:
Allocates memory for the amount of elements requested while keeping the contents
intact. Contents are copied using their = operator so that data is correctly
instantiated.
================================================================================
*/
template<typename T>
inline void List<T>::Resize(int newSize) {
    assert(newSize >= 0);

    if (newSize <= 0) {
        Clear();
        return;
    }

    if (newSize == m_capacity) {
        return;
    }

    m_list = ResizeArray<T>(m_list, m_capacity, newSize, false);
    m_capacity = newSize;

    if (m_capacity < m_size) {
        m_size = m_capacity;
    }
}

/*
================================================================================
List<T>::Resize

DESCRIPTION:
Allocates memory for the amount of elements requested while keeping the contents
intact. Contents are copied using their = operator so that data is correctly
instantiated.
================================================================================
*/
template<typename T>
inline void List<T>::Resize(int newSize, int newGranularity) {
    assert(newSize >= 0);
    assert(newGranularity > 0);

    m_granularity = newGranularity;

    if (newSize <= 0) {
        Clear();
        return;
    }

    m_list = ResizeArray<T>(m_list, m_capacity, newSize, false);
    m_capacity = newSize;

    if (m_capacity < m_size) {
        m_size = m_capacity;
    }
}

/*
================================================================================
List<T>::AssureSize

DESCRIPTION:
Makes sure the list has at least the given number of elements.
================================================================================
*/
template< typename T>
inline void List<T>::AssureSize(int newSize) {
    int newCount = newSize;

    if (newSize > m_capacity) {
        newSize += m_granularity - 1;
        newSize -= newSize % m_granularity;
        Resize(newSize);
    }

    m_size = newCount;
}

/*
================================================================================
List<T>::AssureSize

DESCRIPTION:
Makes sure the list has at least the given number of elements and initialize
any elements not yet initialized.
================================================================================
*/
template< typename T>
inline void List<T>::AssureSize(int newSize, const T &initValue) {
    int newCount = newSize;

    if (newSize > m_capacity) {

        if (m_granularity == 0) {	// this is a hack to fix our memset classes
            m_granularity = 16;
        }

        newSize += m_granularity - 1;
        newSize -= newSize % m_granularity;
        m_size = m_capacity;
        Resize(newSize);

        for (int i = m_size; i < newSize; i++) {
            m_list[i] = initValue;
        }
    }

    m_size = newCount;
}

/*
================================================================================
List<T>::AssureSizeAlloc

DESCRIPTION:
Makes sure the list has at least the given number of elements and allocates any
elements using the allocator.

NOTE:
This function can only be called on lists containing pointers. Calling it
on non-pointer lists will cause a compiler error.
================================================================================
*/
template< typename T>
inline void List<T>::AssureSizeAlloc(int newSize, new_t *allocator) {
    int newCount = newSize;

    if (newSize > m_capacity) {

        if (m_granularity == 0) {	// this is a hack to fix our memset classes
            m_granularity = 16;
        }

        newSize += m_granularity - 1;
        newSize -= newSize % m_granularity;
        m_size = m_capacity;
        Resize(newSize);

        for (int i = m_size; i < newSize; i++) {
            m_list[i] = (*allocator)();
        }
    }

    m_size = newCount;
}

/*
================================================================================
List<T>::operator=

DESCRIPTION:
Copies the contents and size attributes of another list.
================================================================================
*/
template<typename T>
inline List<T> & List<T>::operator=(const List<T> &other) {
    Clear();

    m_size		= other.m_size;
    m_capacity		= other.m_capacity;
    m_granularity	= other.m_granularity;

    if (m_capacity) {
        m_list = NewArray<T>(m_capacity, false);
        for(int i = 0; i < m_size; i++ ) {
            m_list[i] = other.m_list[i];
        }
    }

    return *this;
}

/*
================================================================================
List<T>::operator[] const

DESCRIPTION:
Access operator.  Index must be within range or an assert will be issued in
debug builds. Release builds do no range checking.
================================================================================
*/
template<typename T>
inline const T & List<T>::operator[](int index) const {
    assert(index >= 0);
    assert(index < m_size);

    return m_list[index];
}

/*
================================================================================
List<T>::operator[]

DESCRIPTION:
Access operator.  Index must be within range or an assert will be issued in
debug builds. Release builds do no range checking.
================================================================================
*/
template<typename T>
inline T & List<T>::operator[](int index) {
    assert( index >= 0 );
    assert(index < m_size);

    return m_list[index];
}

/*
================================================================================
List<T>::Alloc

DESCRIPTION:
Allocates new data element at the end of the list.

RETURNS:
Reference to the new data element.
================================================================================

*/
template<typename T>
inline T & List<T>::Alloc() {
    if (!m_list) {
        Resize(m_granularity);
    }

    if (m_size == m_capacity) {
        Resize(m_capacity + m_granularity);
    }

    return m_list[m_size++];
}

/*
================================================================================
List<T>::Add

DESCRIPTION:
Increases the size of the list by one element and copies the given data into it.

RETURNS:
The index of the new element.
================================================================================
*/
template<typename T>
inline int List<T>::Add(T const & obj) {
    if (!m_list) {
        Resize(m_granularity);
    }

    if (m_size == m_capacity) {
        int newSize = m_capacity + m_granularity;
        Resize(newSize - newSize % m_granularity);
    }

    m_list[m_size] = obj;
    m_size++;

    return m_size - 1;
}


/*
================================================================================
List<T>::Insert

DESCRIPTION:
Increases the size of the list by at least one element if necessary
and inserts the supplied data into it.

RETURNS:
The index of the new element.
================================================================================
*/
template<typename T>
inline int List<T>::Insert(T const & obj, int index) {
    if (!m_list) {
        Resize(m_granularity);
    }

    if (m_size == m_capacity) {
        int newSize;

        if (m_granularity == 0) {	// this is a hack to fix our memset classes
            m_granularity = 16;
        }
        newSize = m_capacity + m_granularity;
        Resize(newSize - newSize % m_granularity);
    }

    if (index < 0) {
        index = 0;
    }
    else if (index > m_size) {
        index = m_size;
    }
    for (int i = m_size; i > index; --i) {
        m_list[i] = m_list[i - 1];
    }
    m_size++;
    m_list[index] = obj;
    return index;
}

/*
================================================================================
List<T>::AddRange

DESCRIPTION:
Appends the other list to this one

RETURNS:
The size of the new combined list
================================================================================
*/
template<typename T>
inline int List<T>::AddRange(const List<T> &other) {
    if (!m_list) {
        if (m_granularity == 0) {	// this is a hack to fix our memset classes
            m_granularity = 16;
        }
        Resize(m_granularity);
    }

    int n = other.Count();
    for (int i = 0; i < n; i++) {
        Add(other[i]);
    }

    return Size();
}

/*
================================================================================
List<T>::AddUnique

DESCRIPTION:
Adds the data to the list if it doesn't already exist.

RETURNS:
The index of the data in the list.
================================================================================
*/
template<typename T>
inline int List<T>::AddUnique(T const & obj) {
    int index;

    index = IndexOf(obj);
    if (index < 0) {
        index = Add(obj);
    }

    return index;
}

/*
================================================================================
List<T>::IndexOf

DESCRIPTION:
Searches for the specified data in the list and returns it's index.

RETURNS:
-1 if the data is not found.
================================================================================
*/
template<typename T>
inline int List<T>::IndexOf(T const & obj) const {
    for(int i = 0; i < m_size; i++) {
        if (m_list[i] == obj) {
            return i;
        }
    }

    return -1;
}

/*
================================================================================
List<T>::IndexOf

DESCRIPTION:
Takes a pointer to an element in the list and returns the index of the element.

RETURNS:
The index of the element.

NOTE:
This is NOT a guarantee that the object is really in the list.
Function will assert in debug builds if pointer is outside the bounds of the list,
but remains silent in release builds.
================================================================================
*/
template<typename T>
inline int List<T>::IndexOf(T const *objPtr) const {
    int index = objPtr - m_list;

    assert(index >= 0);
    assert(index < m_size);

    return index;
}

/*
================================================================================
List<T>::Find

DESCRIPTION:
Searches for the specified data in the list and returns it's address.

RETURNS:
`nullptr` if the data is not found.
================================================================================
*/
template<typename T>
inline T *List<T>::Find(T const & obj) const {
    int i = IndexOf(obj);
    if (i >= 0) {
        return &m_list[i];
    }

    return nullptr;
}

/*
================================================================================
List<T>::FindNull

DESCRIPTION:
Searches for a `nullptr` pointer in the list.

RETURNS:
-1 if `nullptr` is not found.

NOTE:
This function can only be called on lists containing pointers. Calling it
on non-pointer lists will cause a compiler error.
================================================================================
*/
template<typename T>
inline int List<T>::FindNull() const {
    for(int i = 0; i < m_size; i++ ) {
        if (m_list[i] == nullptr) {
            return i;
        }
    }

    return -1;
}

/*
================================================================================
List<T>::RemoveIndex

DESCRIPTION:
Removes the element at the specified index and moves all data following the
element down to fill in the gap. The number of elements in the list is reduced
by one.

RETURNS:
False if the index is outside the bounds of the list.

NOTE: that the element is not destroyed, so any memory used by it may not be
freed until the destruction of the list.
================================================================================
*/
template<typename T>
inline bool List<T>::RemoveIndex(int index) {
    assert(m_list != nullptr);
    assert(index >= 0);
    assert(index < m_size);

    if (index < 0 || index >= m_size) {
        return false;
    }

    m_size--;
    for(int i = index; i < m_size; i++) {
        m_list[i] = m_list[i + 1];
    }

    return true;
}

/*
================================================================================
List<T>::RemoveIndexFast

DESCRIPTION:
Removes the element at the specified index and moves the last element into its
spot, rather than moving the whole array down by one. Of course, this doesn't
maintain the order of elements! The number of elements in the list is reduced
by one.

RETURNS:
False if the data is not found in the list.

NOTE:
The element is not destroyed, so any memory used by it may not be freed
until the destruction of the list.
================================================================================
*/
template<typename T>
inline bool List<T>::RemoveIndexFast(int index)
{
    if (index < 0 || index >= m_size)
    {
        return false;
    }

    m_size--;
    if (index != m_size)
    {
        m_list[index] = m_list[m_size];
    }

    return true;
}

/*
================================================================================
List<T>::Remove

DESCRIPTION:
Removes the element if it is found within the list and moves all data following
the element down to fill in the gap. The number of elements in the list is
reduced by one.

RETURNS:
False if the data is not found in the list.

NOTE:
The element is not destroyed, so any memory used by it may not be freed until
the destruction of the list.
================================================================================
*/
template<typename T>
inline bool List<T>::Remove(T const & obj)
{
    int index = FindIndex(obj);
    if (index >= 0)
    {
        return RemoveIndex(index);
    }

    return false;
}

#endif //RELOAD_LIST_H
