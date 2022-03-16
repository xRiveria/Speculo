#include <cstddef>
#include <cstring>
#include <utility>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include <string>

// General reserved space of up to 4x.

#ifndef SPECULO_VECTOR
#define SPECULO_VECTOR

#define SPECULO_VECTOR_MAX_SIZE 1000000000

namespace Speculo
{
    template <typename T>
    class Vector
    {
    public:
        typedef T                                                   value_type;
        typedef T&                                                  reference;
        typedef const T&                                            const_reference;
        typedef T*                                                  pointer;
        typedef const T*                                            const_pointer;
        typedef T*                                                  iterator;
        typedef const T*                                            const_iterator;
        typedef std::reverse_iterator<iterator>                     reverse_iterator;
        typedef std::reverse_iterator<const_iterator>               const_reverse_iterator;
        typedef ptrdiff_t                                           difference_type;
        typedef unsigned int                                        size_type;

        // Construct/Copy/Destroy
        Vector() noexcept;
        explicit Vector(size_type count);
        Vector(size_type count, const T& value);
        template <typename InputIt> Vector(InputIt first, InputIt last);
        Vector(std::initializer_list<T>);
        Vector(const Vector<T>&);
        Vector(Vector<T>&&) noexcept;
        ~Vector();

        Vector<T>& operator=(const Vector<T>&);
        Vector<T>& operator=(Vector<T>&&);
        Vector<T>& operator=(std::initializer_list<T>);

        void assign(size_type, const T& value);
        template <typename InputIt> void assign(InputIt, InputIt);
        void assign(std::initializer_list<T>);

        // Iterators
        iterator begin() noexcept;
        iterator end() noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;
        reverse_iterator rbegin() noexcept;
        reverse_iterator rend() noexcept;
        const_reverse_iterator crbegin() const noexcept;
        const_reverse_iterator crend() const noexcept;

        // Capacity
        bool empty() const noexcept;
        size_type size() const noexcept;
        size_type max_size() const noexcept;
        size_type capacity() const noexcept;
        void resize(size_type);
        void resize(size_type, const T&);
        void reserve(size_type);
        void shrink_to_fit();

        // Element Access
        reference operator [](size_type);
        const_reference operator [](size_type) const;
        reference at(size_type);
        const_reference at(size_type) const;
        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;

        // Data Access
        T* data() noexcept;
        const T* data() const noexcept;

        // Modifiers
        template <typename ...Args> void emplace_back(Args&& ...args);
        void push_back(const T&);
        void push_back(T&&);
        void pop_back();

        template <typename ...Args> iterator emplace(const_iterator, Args&&...);
        iterator insert(const_iterator, const T&);
        iterator insert(const_iterator, T&&);
        iterator insert(const_iterator, size_type, const T&);
        template <typename InputIt> iterator insert(const_iterator, InputIt, InputIt);
        iterator insert(const_iterator, std::initializer_list<T>);
        iterator erase(const_iterator);
        iterator erase(const_iterator, const_iterator);
        void swap(Vector<T>&);
        void clear() noexcept;

        bool operator == (const Vector<T>&) const;
        bool operator != (const Vector<T>&) const;
        bool operator < (const Vector<T>&) const;
        bool operator <= (const Vector<T>&) const;
        bool operator > (const Vector<T>&) const;
        bool operator >= (const Vector<T>&) const;

        friend void Print(const Vector<T>&, std::string);

    private:
        size_type m_Reserved_Size = 4;
        size_type m_Vector_Size = 0;
        T* m_Array;

        inline void reallocate();
    };

    template <typename T>
    Vector<T>::Vector() noexcept
    {
        m_Array = new T[m_Reserved_Size];
    }

    template <typename T>
    Vector<T>::Vector(typename Vector<T>::size_type count)
    {
        size_type i;
        m_Reserved_Size = count << 2;  
        m_Array = new T[m_Reserved_Size];
 
        for (i = 0; i < m_Reserved_Size; ++i)
        {
            m_Array[i] = T();
        }

        m_Vector_Size = count;
    }

    template <typename T>
    Vector<T>::Vector(typename Vector<T>::size_type count, const T& value)
    {
        size_type i;
        m_Reserved_Size = count << 2;
        m_Array = new T[m_Reserved_Size];

        for (i = 0; i < count; ++i)
        {
            m_Array[i] = value;
        }

        m_Vector_Size = count;
    }

    template <typename T>
    template <typename InputIt>
    Vector<T>::Vector(InputIt first, InputIt last)
    {
        std::cout << "Inside... \n";
        size_type i;
        size_type count = last - first;
        std::cout << "Count = " << count << std::endl;

        m_Reserved_Size = count << 2;
        m_Vector_Size = count;
        m_Array = new T[m_Reserved_Size];
        for (i = 0; i < count; ++i, ++first)
        {
            m_Array[i] = *first;
        }
    }

    template <typename T>
    Vector<T>::Vector(std::initializer_list<T> list)
    {
        m_Reserved_Size = list.size() << 2;
        m_Array = new T[m_Reserved_Size];
        for (auto& item : list)
        {
            m_Array[m_Vector_Size++] = item;
        }
    }

    template <typename T>
    Vector<T>::Vector(const Vector<T>& other)
    {
        size_type i;
        m_Reserved_Size = other.m_Reserved_Size;
        m_Array = new T[m_Reserved_Size];

        for (i = 0; i < other.m_Vector_Size; ++i)
        {
            m_Array[i] = other.m_Array[i];
        }

        m_Vector_Size = other.m_Vector_Size;
    }

    template <typename T>
    Vector<T>::Vector(Vector<T>&& other) noexcept
    {
        size_type i;
        m_Reserved_Size = other.m_Reserved_Size;
        m_Array = new T[m_Reserved_Size];

        for (i = 0; i < other.m_Vector_Size; ++i)
        {
            m_Array[i] = std::move(other.m_Array[i]);
        }

        m_Vector_Size = other.m_Vector_Size;
    }

    template <typename T>
    Vector<T>::~Vector()
    {
        delete[] m_Array;
    }

    template <typename T>
    Vector<T>& Vector<T>::operator=(const Vector<T>& other)
    {
        if (this == &other)
        {
            return *this;
        }

        size_type i;
        if (m_Reserved_Size < other.m_Reserved_Size)
        {
            m_Reserved_Size = other.m_Reserved_Size << 2;
            reallocate();
        }

        for (i = 0; i < other.m_Vector_Size; ++i)
        {
            m_Array[i] = other.m_Array[i];
        }

        m_Vector_Size = other.m_Vector_Size;
    }

    template <typename T>
    Vector<T>& Vector<T>::operator=(Vector<T>&& other)
    {
        size_type i;
        if (m_Reserved_Size < other.m_Reserved_Size)
        {
            m_Reserved_Size = other.m_Reserved_Size << 2;
            reallocate();
        }

        for (i = 0; i < other.m_Vector_Size; ++i)
        {
            m_Array[i] = std::move(other.m_Array[i]);
        }

        m_Vector_Size = other.m_Vector_Size;
    }

    template <typename T>
    Vector<T>& Vector<T>::operator=(std::initializer_list<T> list)
    {
        if (m_Reserved_Size < list.size())
        {
            m_Reserved_Size = list.size() << 2;
            reallocate();
        }

        m_Vector_Size = 0;

        for (auto& item : list)
        {
            m_Array[m_Vector_Size++] = item;
        }
    }

    template <typename T>
    void Vector<T>::assign(typename Vector<T>::size_type count, const T& value)
    {
        size_type i;

        if (count > m_Reserved_Size)
        {
            m_Reserved_Size = count << 2;
            reallocate();
        }

        for (i = 0; i < count; ++i)
        {
            m_Array[i] = value;
        }

        m_Vector_Size = count;
    }

    template <typename T>
    template <typename InputIt>
    void Vector<T>::assign(InputIt first, InputIt last)
    {
        size_type i;
        size_type count = last - first;
        if (count > m_Reserved_Size)
        {
            m_Reserved_Size = count << 2;
            reallocate();
        }

        for (i = 0; i < count; ++i, ++first)
        {
            m_Array[i] = *first;
        }

        m_Vector_Size = count;
    }

    template <typename T>
    void Vector<T>::assign(std::initializer_list<T> list)
    {
        size_type i, count = list.size();
        if (count > m_Reserved_Size)
        {
            m_Reserved_Size = count << 2;
            reallocate();
        }

        i = 0;
        for (auto& item : list)
        {
            m_Array[i++] = item;
        }

        m_Vector_Size = count;
    }

    template <typename T>
    typename Vector<T>::iterator Vector<T>::begin() noexcept
    {
        return m_Array;
    }

    template <typename T>
    typename Vector<T>::const_iterator Vector<T>::cbegin() const noexcept
    {
        return m_Array;
    }

    template <typename T>
    typename Vector<T>::iterator Vector<T>::end() noexcept
    {
        return m_Array + m_Vector_Size;
    }

    template <typename T>
    typename Vector<T>::const_iterator Vector<T>::cend() const noexcept
    {
        return m_Array + m_Vector_Size;
    }

    template <typename T>
    typename Vector<T>::reverse_iterator Vector<T>::rbegin() noexcept
    {
        return reverse_iterator(m_Array + m_Vector_Size);
    }

    template <typename T>
    typename Vector<T>::const_reverse_iterator Vector<T>::crbegin() const noexcept
    {
        return reverse_iterator(m_Array + m_Vector_Size);
    }

    template <typename T>
    typename Vector<T>::reverse_iterator Vector<T>::rend() noexcept
    {
        return reverse_iterator(m_Array);
    }

    template <typename T>
    typename Vector<T>::const_reverse_iterator Vector<T>::crend() const noexcept
    {
        return reverse_iterator(m_Array);
    }

    template <typename T>
    inline void Vector<T>::reallocate()
    {
        T* temporaryArray = new T[m_Reserved_Size];
        memcpy(temporaryArray, m_Array, m_Vector_Size * sizeof(T));
        delete[] m_Array;
        m_Array = temporaryArray;
    }

    template <typename T>
    bool Vector<T>::empty() const noexcept
    {
        return m_Vector_Size == 0;
    }

    template <typename T>
    typename Vector<T>::size_type Vector<T>::size() const noexcept
    {
        return m_Vector_Size;
    }

    template <typename T>
    typename Vector<T>::size_type Vector<T>::max_size() const noexcept
    {
        return SPECULO_VECTOR_MAX_SIZE;
    }

    template <typename T>
    typename Vector<T>::size_type Vector<T>::capacity() const noexcept
    {
        return m_Reserved_Size;
    }

    template <typename T>
    void Vector<T>::resize(typename Vector<T>::size_type newSize)
    {
        if (newSize > m_Vector_Size)
        {
            if (newSize > m_Reserved_Size)
            {
                m_Reserved_Size = newSize;
                reallocate();
            }
        }
        else
        {
            size_type i;
            for (i = m_VectorSize; i < newSize; ++i)
            {
                m_Array[i].~T();
            }
        }

        m_Vector_Size = newSize;
    }

    template <typename T>
    void Vector<T>::resize(typename Vector<T>::size_type newSize, const T& value)
    {
        if (newSize > m_Vector_Size)
        {
            if (newSize > m_Reserved_Size)
            {
                m_Reserved_Size = newSize;
                reallocate();
            }

            size_type i;
            for (i = m_Vector_Size; i < newSize; ++i)
            {
                m_Array[i] = value;
            }
        }
        else
        {
            size_type i;
            for (i = m_Vector_Size; i < newSize; ++i)
            {
                m_Array[i].~T();
            }
        }

        m_Vector_Size = newSize;
    }

    template <typename T>
    void Vector<T>::reserve(typename Vector<T>::size_type newSize)
    {
        if (newSize > m_Reserved_Size)
        {
            m_Reserved_Size = newSize;
            realloacate();
        }
    }

    template <typename T>
    void Vector<T>::shrink_to_fit()
    {
        m_Reserved_Size = m_Vector_Size;
        reallocate();
    }

    template <typename T>
    typename Vector<T>::reference Vector<T>::operator [](typename Vector<T>::size_type index)
    {
        return m_Array[index];
    }

    template <typename T>
    typename Vector<T>::const_reference Vector<T>::operator [](typename Vector<T>::size_type index) const
    {
        return m_Array[index];
    }

    template <typename T>
    typename Vector<T>::reference Vector<T>::at(size_type position)
    {
        if (position < m_Vector_Size)
        {
            return m_Array[position];
        }
        else
        {
            throw std::out_of_range("Accessed position is out of range.");
        }
    }

    template <typename T>
    typename Vector<T>::const_reference Vector<T>::at(size_type position) const
    {
        if (position < m_Vector_Size)
        {
            return m_Array[position];
        }
        else
        {
            throw std::out_of_range("Accessed position is out of range.");
        }
    }

    template <typename T>
    typename Vector<T>::reference Vector<T>::front()
    {
        return m_Array[0];
    }

    template <typename T>
    typename Vector<T>::const_reference Vector<T>::front() const
    {
        return m_Array[0];
    }

    template <typename T>
    typename Vector<T>::reference Vector<T>::back()
    {
        return m_Array[m_Vector_Size - 1];
    }

    template <typename T>
    typename Vector<T>::const_reference Vector<T>::back() const
    {
        return m_Array[m_Vector_Size - 1];
    }

    template <typename T>
    T* Vector<T>::data() noexcept
    {
        return m_Array;
    }

    template <typename T>
    const T* Vector<T>::data() const noexcept
    {
        return m_Array;
    }

    template <typename T>
    template <typename ...Args>
    void Vector<T>::emplace_back(Args&& ...args)
    {
        if (m_Vector_Size == m_Reserved_Size)
        {
            m_Reserved_Size <<= 2;
            reallocate();
        }

        m_Array[m_Vector_Size] = std::move(T(std::forward<Args>(args)...));
        ++m_Vector_Size;
    }

    template <typename T>
    void Vector<T>::push_back(const T& value)
    {
        if (m_Vector_Size == m_Reserved_Size)
        {
            m_Reserved_Size <<= 2;
            reallocate();
        }

        m_Array[m_Vector_Size] = value;
        ++m_Vector_Size;
    }

    template <typename T>
    void Vector<T>::push_back(T&& value)
    {
        if (m_Vector_Size == m_Reserved_Size)
        {
            m_Reserved_Size <<= 2;
            reallocate();
        }

        m_Array[m_Vector_Size] = std::move(value);
        ++m_Vector_Size;
    }

    template <typename T>
    void Vector<T>::pop_back()
    {
        --m_Vector_Size;
        m_Array[m_Vector_Size].~T();
    }

    template <typename T>
    template <typename ...Args>
    typename Vector<T>::iterator Vector<T>::emplace(typename Vector<T>::const_iterator it, Args&& ...args)
    {
        iterator internalIterator = &m_Array[it - m_Array];
        if (m_Vector_Size == m_Reserved_Size)
        {
            m_Reserved_Size <<= 2;
            reallocate();
        }

        memmove(internalIterator + 1, internalIterator, (m_VectorSize - (it - m_Array)) * sizeof(T));
        (*internalIterator) = std::move(T(std::forward<Args>(args)...));
        ++m_Vector_Size;
        
        return internalIterator;
    }

    template <typename T>
    typename Vector<T>::iterator Vector<T>::insert(typename Vector<T>::const_iterator it, const T& value)
    {
        iterator internalIterator = &m_Array[it - m_Array];
        if (m_Vector_Size == m_Reserved_Size) // Ensure we have space for memory manipulation.
        {
            m_Reserved_Size <<= 2;
            reallocate();
        }

        memmove(internalIterator + 1, internalIterator, (m_VectorSize - (it - m_Array)) * sizeof(T));
        (*internalIterator) = value;
        ++m_Vector_Size;
        return internalIterator;
    }

    template <typename T>
    typename Vector<T>::iterator Vector<T>::insert(typename Vector<T>::const_iterator it, T&& value)
    {
        iterator internalIterator = &m_Array[it - m_Array];
        if (m_Vector_Size == m_Reserved_Size)
        {
            m_Reserved_Size <<= 2;
            reallocate();
        }

        memmove(internalIterator + 1, internalIterator, (m_Vector_Size - (it - m_Array)) * sizeof(T));
        (*internalIterator) = std::move(value);
        ++m_Vector_Size;
        return internalIterator;
    }
}


#endif