//
// Created by ivan on 18.3.21.
//

#ifndef RELOAD_LIST_H
#define RELOAD_LIST_H

#include <assert.h>
#include "../../Common.h"
#include "../sys/Heap.h"

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
inline T * NewArray(int newCount, bool zeroBuffer) {
    T * ptr;

    if (zeroBuffer) {
//        ptr = (T *)Mem::ClearedAlloc((int)(sizeof(T) * newCount));
        ptr = (T *)calloc(0, (sizeof(T) * newCount));
    }
    else {
//        ptr = (T *)Mem::Alloc((int)(sizeof(T) * newCount));
        ptr = (T *)malloc((sizeof(T) * newCount));
    }

    for (int i = 0; i < newCount; i++) {
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
        ((T *)list)[i].~T();
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
        for ( int i = 0; i < overlap; i++ ) {
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

                    List(int newGranularity = 16);
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
    int             Insert(T const & obj, int idx = 0);                         // Inserts element at the given index.
    bool			RemoveIndex(int index);				    			        // Removes the element at the given index.
    bool			RemoveIndexFast(int index);                                 // Removes the element at the given index and places the last element into its spot - DOES NOT PRESERVE LIST ORDER.
    bool			Remove(T const & obj);              						// Removes the given element.
//    void            Sort();                                                     // Sorts the list.
//    void            Swap(List & other);                                         // Swaps the contents of the list.
    void            Clear();                                                    // Clears the list.
    void            DeleteContents(bool clear = true);                          // Deletes the contents of the list.

    void			Condense();											        // Resizes list to exactly the number of elements it contains or frees up memory if empty.
    void			Resize(int newSize);								        // Resizes list to the given number of elements
    void			Resize(int newSize, int newGranularity);			        // resizes list and sets new granularity
    void			SetCount(int newCount);							            // Set number of elements in list and resize to exactly this number if needed
    void			AssureSize(int newSize);							        // Assure list has given number of elements, but leave them uninitialized
    void			AssureSize(int newSize, const T &initValue);	            // Assure list has given number of elements and initialize any new elements
    void			AssureSizeAlloc(int newSize, new_t *allocator);	            // Assure the pointer list has the given number of elements and allocate any new elements.
    void			SetGranularity( int newGranularity );				        // Sets new granularity

    int				IndexOf(T const & obj) const;   				            // Finds and returns the index for the given element reference.
    int				IndexOf(T const * objPtr) const;	    				    // Finds and returns the index for the given element pointer.
    T *             Find(T const & obj) const;                                  // Finds and returns a pointer to the given element reference.
    int				FindNull() const;									        // Finds and returns the index for the first NULL pointer in the list.
    int             Count() const;                                              // Gets the number of elements in the list.
    int				Granularity() const;								        // Gets the current granularity.
    int             AllocatedCount() const;                                     // Gets the number of elements currently allocated for.
    size_t          Size() const;                                               // Gets the total size of list in bytes.
    size_t          MemoryUsed() const;                                         // Gets the total memory used by the list.
    size_t          Allocated() const;                                          // Gets the number of allocated space in the list.

private:
    int				count;
    int				size;
    int				granularity;
    T * 		    list;
};


/*
================================================================================
List<T>::List

DESCRIPTION:
List's constructor with granularity as parameter. Default granularity is set to
16.
================================================================================
*/
template<typename T>
inline List<T>::List(int newGranularity) {
    assert(newGranularity > 0);

    list        = nullptr;
    granularity = newGranularity;
    Clear();
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
    list    = nullptr;
    *this   = other;
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
    return list;
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
    return list;
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
    if(list) {
        DeleteArray<T>(list, size);
    }

    list    = nullptr;
    count   = 0;
    size    = 0;
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
    for(int i = 0; i < count; i++) {
        delete list[i];
        list[i] = nullptr;
    }

    if (clear) {
        Clear();
    } else {
        memset(list, 0, size * sizeof(T));
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
    return size * sizeof(T);
}

/*
================================================================================
List<T>::Size

RETURNS:
Total size of list in bytes.

NOTES:
Doesn't take into account additional memory allocated by T
================================================================================
*/
template<typename T>
inline size_t List<T>::Size() const {
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
    return count * sizeof(*list);
}

/*
================================================================================
List<T>::Count

RETURNS:
The number of elements currently contained in the list.

NOTE:
This is NOT an indication of the memory allocated.
================================================================================
*/
template<typename T>
inline int List<T>::Count() const {
    return count;
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
    return size;
}

/*
================================================================================
List<T>::SetCount

DESCRIPTION:
Sets the number of elements and resizes the list to match.
================================================================================
*/
template<typename T>
inline void List<T>::SetCount(int newCount) {
    assert(newCount >= 0);

    if (newCount > size) {
        Resize(newCount);
    }

    count = newCount;
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

    if (list) {
        int newSize;
        granularity = newGranularity;

        newSize = count + granularity - 1;
        newSize -= newSize % granularity;
        if ( newSize != size ) {
            Resize(newSize);
        }
    }
}

/*
================================================================================
List<T>::Granularity

DESCRIPTION:
Get the current granularity.
================================================================================
*/
template<typename T>
inline int List<T>::Granularity() const {
    return granularity;
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
    if (list) {
        if (count) {
            Resize(count);
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
intact. Contents are copied using their = operator so that data is correnctly
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

    if (newSize == size) {
        return;
    }

    list = ResizeArray<T>(list, size, newSize, false);
    size = newSize;

    if (size < count) {
        count = size;
    }
}

/*
================================================================================
List<T>::Resize

DESCRIPTION:
Allocates memory for the amount of elements requested while keeping the contents
intact. Contents are copied using their = operator so that data is correnctly
instantiated.
================================================================================
*/
template<typename T>
inline void List<T>::Resize(int newSize, int newGranularity) {
    assert(newSize >= 0);
    assert(newGranularity > 0);

    granularity = newGranularity;

    if (newSize <= 0) {
        Clear();
        return;
    }

    list = ResizeArray<T>(list, size, newSize, false);
    size = newSize;

    if (size < count) {
        count = size;
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

    if (newSize > size) {
        if (granularity == 0) {	// this is a hack to fix our memset classes
            granularity = 16;
        }

        newSize += granularity - 1;
        newSize -= newSize % granularity;
        Resize(newSize);
    }

    count = newCount;
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

    if (newSize > size) {

        if (granularity == 0) {	// this is a hack to fix our memset classes
            granularity = 16;
        }

        newSize += granularity - 1;
        newSize -= newSize % granularity;
        count = size;
        Resize(newSize);

        for (int i = count; i < newSize; i++) {
            list[i] = initValue;
        }
    }

    count = newCount;
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

    if (newSize > size) {

        if (granularity == 0) {	// this is a hack to fix our memset classes
            granularity = 16;
        }

        newSize += granularity - 1;
        newSize -= newSize % granularity;
        count = size;
        Resize(newSize);

        for (int i = count; i < newSize; i++) {
            list[i] = (*allocator)();
        }
    }

    count = newCount;
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

    count		= other.count;
    size		= other.size;
    granularity	= other.granularity;

    if (size) {
        list = NewArray<T>(size, false);
        for(int i = 0; i < count; i++ ) {
            list[i] = other.list[i];
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
    assert(index < count);

    return list[index];
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
    assert(index < count);

    return list[index];
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
    if (!list) {
        Resize(granularity);
    }

    if (count == size) {
        Resize(size + granularity);
    }

    return list[count++];
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
    if (!list) {
        Resize(granularity);
    }

    if (count == size) {
        int newSize;

        if (granularity == 0) {	// this is a hack to fix our memset classes
            granularity = 16;
        }
        newSize = size + granularity;
        Resize(newSize - newSize % granularity);
    }

    list[count] = obj;
    count++;

    return count - 1;
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
    if (!list) {
        Resize(granularity);
    }

    if (count == size) {
        int newSize;

        if (granularity == 0) {	// this is a hack to fix our memset classes
            granularity = 16;
        }
        newSize = size + granularity;
        Resize(newSize - newSize % granularity);
    }

    if (index < 0) {
        index = 0;
    }
    else if (index > count) {
        index = count;
    }
    for (int i = count; i > index; --i) {
        list[i] = list[i-1];
    }
    count++;
    list[index] = obj;
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
    if (!list) {
        if (granularity == 0) {	// this is a hack to fix our memset classes
            granularity = 16;
        }
        Resize(granularity);
    }

    int n = other.Count();
    for (int i = 0; i < n; i++) {
        Add(other[i]);
    }

    return Count();
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
    for(int i = 0; i < count; i++) {
        if (list[i] == obj) {
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
    int index = objPtr - list;

    assert(index >= 0);
    assert(index < count);

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
        return &list[i];
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
    for(int i = 0; i < count; i++ ) {
        if (list[i] == nullptr) {
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
    assert(list != nullptr);
    assert(index >= 0);
    assert(index < count);

    if (index < 0 || index >= count) {
        return false;
    }

    count--;
    for(int i = index; i < count; i++) {
        list[i] = list[i + 1];
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
    if (index < 0 || index >= count)
    {
        return false;
    }

    count--;
    if (index != count)
    {
        list[index] = list[count];
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
