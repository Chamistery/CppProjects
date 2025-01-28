# Задача "Лист" RU

В этой задаче вам необходимо реализовать шаблонный класс List\<T, Allocator\> аналог std::list из stl.

Реализация должна быть основана на нодах (класс Node) которые содержат указатели на лево и на право.

Ваш List должен уметь в:

* Базовую функциональность + итераторы
* Поддержку аллокаторов
* Быть exception-safety

## Codestyle

Все методы snake_case для соответствия stl'ному list'у

## Методы:

### Using'и

* value_type
* allocator_type
* iterator (должен удовлетворять https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator)

### Конструкторы

Ваш класс должен реализовывать следующие конструкторы (что они делают можно посмотреть на cppref):

* List()
* List(size_t count, const T& value = T(), const Allocator& alloc = Allocator())
* explicit List(size_t count, const Allocator& alloc = Allocator())
* List(const list& other); // не забывайте про select_on_container_copy_construction
* List(std::initializer_list\<T\> init, const Allocator& alloc = Allocator())

### Деструктор

Ну, он должен быть :)

### Iterators (с поддержкой константных и обратных)

* begin()
* end()
* rbegin()
* rend()
* cbegin()
* cend()

### operator=

* List& operator=(const List& other)

***Note*** Важное уточнение по поводу оператора присваивания: вы должны пользоваться propagate_on_container_copy_assignment. Это будет проверяться в тестах. Как это работает написано на cppref https://en.cppreference.com/w/cpp/container/list/operator%3D

### element access methods

* T& front()
* const T& front() const
* T& back()
* const T& back() const


### Capacity

* bool empty()
* size_t size()

### Modifiers

* push_back(front)(const T&)
* pop_back(front)();

## Различные требования:

### Поддержка аллокаторов

* Вы должны работать с памятью только через аллокатор
* Вы должны конструировать и разрушать объекты только через аллокатор
* Вы должны поддержать propagate_on_container_copy в соответствующих методах
* Вы должны использовать rebind для аллоцирования и конструирования внутреннего класса Node

### Exception-safety

Общая концепция: если где-то выскочит исключение ваш контейнер должен вернуться в оригинальное состояние и пробросить исключение наверх.

Т.е. ваш класс должен быть готов к исключениям в копировании объекта T и в конструкторах объекта T

# LIST ENG

In this task, you need to implement a templated class `List<T, Allocator>` similar to `std::list` from the STL.

The implementation must be based on nodes (class `Node`) that contain pointers to the left and right.

Your `List` must support:

* Basic functionality + iterators
* Allocator support
* Exception safety

## Codestyle

All methods must use `snake_case` to match the STL `list`.

## Methods

### `using` Aliases

* `value_type`
* `allocator_type`
* `iterator` (must satisfy [BidirectionalIterator](https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator))

### Constructors

Your class must implement the following constructors (refer to cppreference for their behavior):

* `List()`
* `List(size_t count, const T& value = T(), const Allocator& alloc = Allocator())`
* `explicit List(size_t count, const Allocator& alloc = Allocator())`
* `List(const List& other)` // remember to use `select_on_container_copy_construction`
* `List(std::initializer_list<T> init, const Allocator& alloc = Allocator())`

### Destructor

It should, of course, be implemented :)

### Iterators (with support for constant and reverse iterators)

* `begin()`
* `end()`
* `rbegin()`
* `rend()`
* `cbegin()`
* `cend()`

### operator=

* `List& operator=(const List& other)`

***Note:*** An important clarification about the assignment operator: you must use `propagate_on_container_copy_assignment`. This will be tested. Refer to [cppreference](https://en.cppreference.com/w/cpp/container/list/operator%3D) for details.

### Element Access Methods

* `T& front()`
* `const T& front() const`
* `T& back()`
* `const T& back() const`

### Capacity

* `bool empty()`
* `size_t size()`

### Modifiers

* `push_back(front)(const T&)`
* `pop_back(front)();`

## Various Requirements

### Allocator Support

* You must work with memory exclusively through the allocator.
* You must construct and destroy objects only through the allocator.
* You must support `propagate_on_container_copy` in the appropriate methods.
* You must use `rebind` for allocating and constructing the internal `Node` class.

### Exception Safety

General concept: if an exception occurs, your container must revert to its original state and propagate the exception to the caller.

This means your class must handle exceptions that occur during copying or constructing objects of type `T`.
