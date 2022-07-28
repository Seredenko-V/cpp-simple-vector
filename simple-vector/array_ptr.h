#pragma once

#include <cstdlib>
#include <cassert>
#include <utility>
#include <algorithm>

template <typename Type>
class ArrayPtr {
public:
    // Инициализирует ArrayPtr нулевым указателем
    ArrayPtr() = default;

    // Создаёт в куче массив из size элементов типа Type.
    // Если size == 0, поле raw_ptr_ должно быть равно nullptr
    explicit ArrayPtr(size_t size) {
        if (size != 0) {
            Type* arr = new Type[size];
            raw_ptr_ = arr;
        }
    }

    // Конструктор из сырого указателя, хранящего адрес массива в куче либо nullptr
    explicit ArrayPtr(Type* raw_ptr) noexcept
        : raw_ptr_(raw_ptr) {
    }

    // Запрещаем копирование
    ArrayPtr(const ArrayPtr&) = delete;

    ~ArrayPtr() {
        delete[] raw_ptr_;
        raw_ptr_ = nullptr;
    }

    // Запрещаем присваивание
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    // Конструктор перемещения
    ArrayPtr(ArrayPtr&& rvalue) noexcept {
        raw_ptr_ = std::move(rvalue.raw_ptr_);
        /* когда rvalue выходит за пределы области видимости, вызывается деструктор rvalue, и rvalue.raw_ptr_
        удаляется. Если в этот момент rvalue.raw_ptr_ всё еще указывает на тот же объект, что и raw_ptr_, тогда
        raw_ptr_ останется висячим указателем. Когда объект, содержащий raw_ptr_, в конечном итоге будет
        использован(или уничтожен), мы получим неопределенное поведение. */
        rvalue.raw_ptr_ = nullptr;
    }

    // Перемещающий оператор присваивания
    ArrayPtr& operator=(ArrayPtr&& rvalue) noexcept {
        if (this->raw_ptr_ == rvalue.raw_ptr_) {
            return *this;
        }
        this->swap(rvalue);
        return *this;
    }

    // Прекращает владением массивом в памяти, возвращает значение адреса массива
    // После вызова метода указатель на массив должен обнулиться
    [[nodiscard]] Type* Release() noexcept {
        Type* ptr_arr = raw_ptr_;
        raw_ptr_ = nullptr;
        return ptr_arr;
    }

    // Возвращает ссылку на элемент массива с индексом index
    Type& operator[](size_t index) noexcept {
        return *(raw_ptr_ + index);
    }

    // Возвращает константную ссылку на элемент массива с индексом index
    const Type& operator[](size_t index) const noexcept {
        return *(raw_ptr_ + index);
    }

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const {
        return this->raw_ptr_ == nullptr ? false : true;
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept {
        return raw_ptr_;
    }

    // Обменивается значениям указателя на массив с объектом other
    void swap(ArrayPtr& other) noexcept {
        std::swap(this->raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};