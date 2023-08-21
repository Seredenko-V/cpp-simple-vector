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
![UML](https://raw.githubusercontent.com/Seredenko-V/cpp-simple-vector/06892519bcb94f244fbe8880da51e6f7500b60db/uml-simple-vector.svg?token=AWESMY7ZQTJZT6FTH6FZPG3E4MGBK "UML-diagram simple-vector")
