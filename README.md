# Простой вектор
Учебный проект в рамках [курса](https://practicum.yandex.ru/cpp/?from=catalog) Яндекс Практикума, аналог [std::vector](https://en.cppreference.com/w/cpp/container/vector).  
Его простота заключается в отсутствии взаимодействия с сырой памятью посредством использования [размещающего оператора new](https://en.cppreference.com/w/cpp/memory/new/operator_new), 
`std::uninitialized_*` и `destroy*`.  
* `ArrayPtr` является обёрткой над памятью, выделенной в куче.  
* Методы `PushBack` и `Insert` имеют перегрузки для работы с неперемещаемыми объектами.

## Системные требования
* GCC 9.0
* C++17

## UML-диаграмма
![UML](https://raw.githubusercontent.com/Seredenko-V/cpp-simple-vector/09702d933d2f5616c80858e0f400eb333f4ff5a8/uml-simple-vector.svg?token=AWESMYZCYG2DUZNHW66KZZDE4MA7Q "UML-diagram single-linked-list")
