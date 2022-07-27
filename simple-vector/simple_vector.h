#pragma once

#include "array_ptr.h"
#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <utility>

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity_to_reserve)
        : capacity_to_reserve_(capacity_to_reserve) {
    }
    ~ReserveProxyObj() {
        capacity_to_reserve_ = 0;
    }
    size_t capacity_to_reserve_ = 0;
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size)
        : size_(size)
        , capacity_(size)
        , simple_vector_(new Type[size]{}) {
    };

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, Type&& value)
        : size_(size)
        , capacity_(size)
        , simple_vector_(size) {
        ArrayPtr<Type> tmp_vector(size);
        for (size_t i = 0; i < size; ++i) {
            tmp_vector[i] = std::move(value);
        }
        simple_vector_ = std::move(tmp_vector);
    }

    SimpleVector(size_t size, const Type& value)
        : size_(size)
        , capacity_(size)
        , simple_vector_(size) {
        ArrayPtr<Type> tmp_vector(size);
        std::fill(tmp_vector.Get(), &tmp_vector[size], value);
        simple_vector_.swap(tmp_vector);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
        : size_(init.size())
        , capacity_(init.size())
        , simple_vector_(init.size()) {
        ArrayPtr<Type> tmp_vector(size_);
        // вместо tmp_vector.Get() можно использовать &tmp_vector[0]
        std::move(init.begin(), init.end(), tmp_vector.Get());
        simple_vector_ = std::move(tmp_vector);
    }

    // Конструктор копирования
    SimpleVector(const SimpleVector& other)
        : size_(other.GetSize())
        , capacity_(other.GetCapacity())
        , simple_vector_(other.GetSize()) {
        SimpleVector tmp_vector(size_);
        std::copy(other.begin(), other.end(), tmp_vector.begin());
        simple_vector_.swap(tmp_vector.simple_vector_);
    }

    // Конструктор перемещения
    SimpleVector(SimpleVector&& rvalue)
        : size_(rvalue.GetSize())
        , capacity_(rvalue.GetCapacity()) {
        simple_vector_.swap(rvalue.simple_vector_);
        std::exchange(rvalue.capacity_, 0);
        std::exchange(rvalue.size_, 0);
    }

    // Конструктор резервирования
    SimpleVector(ReserveProxyObj reserved_vector)
        : capacity_(reserved_vector.capacity_to_reserve_)
        , simple_vector_(reserved_vector.capacity_to_reserve_) {
    }

    ~SimpleVector() = default;

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return simple_vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return simple_vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Попытка обращения к элементу за пределами вектора.");
        }
        return simple_vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Попытка обращения к элементу за пределами вектора.");
        }
        return simple_vector_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size > size_) {
            if (new_size > capacity_) {
                SimpleVector new_vector(std::max(this->capacity_ * 2, new_size));
                new_vector.size_ = new_size;
                std::move(this->begin(), this->end(), new_vector.begin());
                for (size_t i = size_; i < new_size; ++i) {
                    new_vector.simple_vector_[i] = std::move(Type());
                }
                *this = std::move(new_vector);
                this->capacity_ = std::max(this->capacity_ * 2, new_size);
            } else {
                for (size_t i = size_; i < new_size; ++i) {
                    simple_vector_[i] = std::move(Type()); // дефолтные значения после уже записанных
                }
            }
        }
        this->size_ = new_size;
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return simple_vector_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return &simple_vector_[size_];
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return cbegin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return cend();
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return simple_vector_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return &simple_vector_[size_];
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (*this == rhs) {
            return *this;
        }
        SimpleVector tmp_vector(rhs);
        this->swap(tmp_vector);
        return *this;
    }

    // Перемещающий оператор присваивания
    SimpleVector& operator=(SimpleVector&& rvalue) {
        if (this == &rvalue) {
            return *this;
        }
        this->simple_vector_.swap(rvalue.simple_vector_);
        std::swap(this->size_, rvalue.size_);
        std::exchange(rvalue.size_, 0);
        return *this;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        this->Resize(size_ + 1);
        simple_vector_[size_ - 1] = item;
    }

    void PushBack(Type&& item) {
        this->Resize(size_ + 1);
        simple_vector_[size_ - 1] = std::move(item);
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        if (capacity_ == 0) {
            this->PushBack(value);
            return this->begin();
        }
        size_t position_new_element = std::distance(this->cbegin(), pos);
        Iterator new_pos = this->begin() + position_new_element; // текущего вектора
        if (size_ == capacity_) {
            assert(size_ <= capacity_);
            ArrayPtr<Type> tmp_arr(capacity_ * 2);
            std::copy(begin(), new_pos, tmp_arr.Get());
            tmp_arr[position_new_element] = value;
            std::copy_backward(new_pos, end(), tmp_arr.Get() + size_ + 1);
            ++size_;
            capacity_ *= 2;
            simple_vector_.swap(tmp_arr);
            return &simple_vector_[position_new_element];
        } else {
            std::copy_backward(new_pos, this->end(), std::next(this->end()));
            simple_vector_[position_new_element] = value;
            ++size_;
            return &simple_vector_[position_new_element];
        }
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() && pos <= end());
        if (capacity_ == 0) {
            this->PushBack(std::move(value));
            return this->begin();
        }
        size_t position_new_element = std::distance(this->cbegin(), pos);
        Iterator new_pos = this->begin() + position_new_element; // текущего вектора
        if (size_ == capacity_) {
            assert(size_ <= capacity_);
            ArrayPtr<Type> tmp_arr(capacity_ * 2);
            std::move(begin(), new_pos, tmp_arr.Get());
            tmp_arr[position_new_element] = std::move(value);
            std::move_backward(new_pos, end(), tmp_arr.Get() + size_ + 1);
            ++size_;
            capacity_ *= 2;
            simple_vector_ = std::move(tmp_arr);
            return &simple_vector_[position_new_element];
        } else {
            std::move_backward(new_pos, this->end(), std::next(this->end()));
            simple_vector_[position_new_element] = std::move(value);
            ++size_;
            return &simple_vector_[position_new_element];
        }
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (this->IsEmpty()) {
            return;
        }
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert(pos >= begin() && pos < end());
        std::copy(std::next(pos), this->cend(), const_cast<Iterator>(pos));
        --size_;
        return const_cast<Iterator>(pos);
    }

    Iterator Erase(Iterator pos) {
        assert(pos >= begin() && pos < end());
        std::move(std::next(pos), this->end(), const_cast<Iterator>(pos));
        --size_;
        return const_cast<Iterator>(pos);
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        std::swap(this->capacity_, other.capacity_);
        std::swap(this->size_, other.size_);
        this->simple_vector_.swap(other.simple_vector_);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }
        SimpleVector tmp_vector(new_capacity);
        std::move(begin(), end(), tmp_vector.begin());
        simple_vector_ = std::move(tmp_vector.simple_vector_);
        this->capacity_ = new_capacity;
    }

private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    ArrayPtr<Type> simple_vector_;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
}

template <typename Type>
inline bool operator==(SimpleVector<Type>&& lhs, SimpleVector<Type>&& rhs) {
    return (lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}