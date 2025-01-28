# String RU

В этой задаче вам нужно реализовать класс динамически расширяющейся строки. 
Проблема стековых и выделяемых в куче строк заключается в том, что они имеют фиксированный размер 
и не умеют автоматически расширяться при необходимости (в отличие, например, от `str` в Python). 
Кроме того, при выделении памяти в динамической области возникает потребность в 
ручном отслеживании возможных утечек памяти, что накладывает дополнительные сложности на процесс разработки.

Эти проблемы можно обойти, используя ООП функционал C++. В частности, механизм 
инкапсуляции позволяет реализовать методы добавления элементов, при вызове которых 
строка будет автоматически расширяться, создавая иллюзию неограниченного контейнера. 
А механизмы работы с ресурсами гарантируют своевременное выделение и очищение памяти.

Здесь естественным образом возникает разделение понятий "размера" строке (size) и 
ее "вместимости" (capacity). С++ в каждый момент времени позволяет поддерживать 
массивы конечной вместимости, при этом фактический размер (количество добавленных элементов) 
может быть меньше этого значения. 
*Почему бы постоянно не поддерживать равенство между этими величинами?* - это не эффективно. 
Допустим, мы добавляем элементы по одному. Тогда при каждом обновлении будет 
происходить перевыделение исходной памяти с последующим копированием всех старых элементов в новый буфер. 
Легко видеть, что в этом случае общее число копирований будет квадратичным образом зависеть от числа добавленных элементов. 
Оказыватся, что, если при каждом переполнении массива увеличивать его вместимость 
в константное число раз, то можно добиться линейной зависимости числа операций над 
массивом от числа добавляемых элементов (https://en.wikipedia.org/wiki/Dynamic_array#Geometric_expansion_and_amortized_cost). 
В вашем решении будет проверяться корректность работы данной схемы при множителе 
равном 2 (то есть, если фактический размер изменяется так: 0 -> 1 -> 2 -> 3 -> 4 -> 5 -> ..., то 
вместимость меняется следующим образом: 0 -> 1 -> 2 -> 4 -> 4 -> 8 -> ...).

## Напутственное слово

Это первая задача, где вам не дан интерфейс класса. Не расстраивайтесь, 
он описан ниже, а примеры его использования есть в тестах.

Преподавательский состав курса понимает, что данная задача является достаточно
трудоемкой, поэтому желает вам удачи.

**Не оставляйте данную задачу на последний момент! На нее придется потратить не один вечер:(**

## Детали реализации

От вас требуется реализовать класс `String` - упрощенный аналог `std::string`. 
Класс должен поддерживать следующий функционал:
* Конструктор по умолчанию - создает пустую строку, никакой памяти не выделяется!
* Конструктор, принимающий `size` и `character` (именно в этом порядке) - создает строку длины `size`, 
заполненный символами `character`
* Конструктор от `const char*`, для того, чтобы узнать длину C-style строки, используйте функцию `strlen`
* Правило "трех":
    1. Конструктор копирования
    2. Копирующий оператор присваивания
    3. Деструктор
* Метод `Clear()` - устанавливает размер в 0, деаллокации выделенной памяти при этом НЕ происходит
* Метод `PushBack(character)` - добавляет букву `character` в конец строки
* Метод `PopBack()` - удаляет последнюю букву. В случае пустой строки должен 
ничего не делать, хотя для `std::string` это будет UB
* Метод `Resize(new_size)` - изменяет размер на `new_size`. Если вместимость 
не позволяет хранить столько символов, то выделяется новый буфер с вместимостью `new_size`.
* Метод `Resize(new_size, character)` - то же, что и `Resize(new_size)`, но в случае `new_size > size` 
заполняет недостающие элементы значением `character`.
* Метод `Reserve(new_cap)` - изменяет вместимость на `max(new_cap, текущая вместимость)` 
(если new_cap <= текущая вместимость, то делать ничего не нужно). Размер при этом не изменяется.
* Метод `ShrinkToFit()` - уменьшает `capacity` до `size` (если `capacity` > `size`)
* Метод `Swap(other)` - обменивает содержимое с другой строкой `other`. Должен работать за O(1)
* Константный и неконстантный оператор доступа по индексу []. Неконстантный должен 
позволять изменять полученный элемент (`a[1] = 5`)
* Методы `Front()` и `Back()` - доступ к первому и последнему символам (тоже по две версии).
* Метод `Empty()` - `true`, если строка пустая (размер 0)
* Метод `Size()` - возвращает размер
* Метод `Capacity()` - возвращает вместимость
* Метод `Data()` - возвращает указатель на начало массива.
* Операторы сравнения (`<`, `>`, `<=`, `>=`, `==`, `!=`), задающие лексикографический порядок
* Операторы + и += для конкатенации строк. Например, `"ab" + "oba" = "aboba"`. Операция `s += t` должна работать за `O(|t|)`!!! Иначе не дождетесь итогов тестирования и спалите тестирующие сервера
* Оператор умножения на число. Принимает строку `str` и число `n` (именно в таком порядке) и вернет строку вида `str +  ... + str` (`n` раз). 
Сложность по времени должна быть O(длины результата). Умножение числа на строку должно приводить ошибку компиляции!
  Загадка о лектора по алгоритмам - не возникло ли у вас аналогии с каким-то известным вам алгоритмом?)

* Операторы ввода из потока и вывода в поток.
* Метод `std::vector<String> Split(const String& delim = " ")` - аналог сплита в питоне.
* Метод `String Join(const std::vector<String>& strings)` - аналог джоина в питоне.

## Примечания

1. Решение должно состоять из двух файлов: `string.hpp` с объявлением класса и его методов и `string.cpp` с определением всего необходимого.

2. В этой задаче запрещается использование стандартной библиотеки C++ кроме как `std::vector` только для `Split` и `Join`. Если нужны какие-либо служебные классы или функции, реализуйте их самостоятельно.

3. Проверка устроена таким образом, что требует от вас жесткого следования принятым (выше) 
сигнатурам и именованиям сущностей (то есть никакие `MyString`, `__string_`, `push_back`, `superSolver3000` не пройдут). 
Если вы реализовали требуемый функционал не полностью или интерфейс отличается от заявленного, 
в ответ вы получите ошибку компиляции.

4. В тесты включены проверки функциональности (методы работают так как требует условие), 
проверка эффективности реализации (что значит эффективно описано выше), 
проверка корректности работы с памятью (утечки памяти, обращения к памяти 
не принадлежащей процессу будут приводить к провалу)

# ENG
# String

In this task, you need to implement a dynamically growing string class.  
The problem with stack-allocated and heap-allocated strings is that they have a fixed size  
and cannot automatically expand when needed (unlike `str` in Python, for example).  
Additionally, when allocating memory dynamically, manual tracking of potential memory leaks  
is required, adding complexity to the development process.

These problems can be avoided using C++ OOP features. In particular, encapsulation  
allows implementing methods for adding elements, making the string automatically expand,  
creating the illusion of an unlimited container.  
Furthermore, resource management mechanisms ensure timely allocation and deallocation of memory.

This naturally leads to a distinction between the **size** of a string and its **capacity**.  
C++ allows maintaining arrays with a fixed capacity while the actual size  
(number of added elements) may be smaller.  
*Why not always keep these values equal?* — Because it's inefficient.  
For example, if we add elements one by one, memory reallocation will occur each time,  
along with copying all previous elements into the new buffer.  
It is easy to see that the total number of copies will depend quadratically  
on the number of added elements.  
It turns out that if we increase the capacity **by a constant factor**  
when the array overflows, the number of operations performed on the array  
will grow **linearly** with the number of added elements  
([source](https://en.wikipedia.org/wiki/Dynamic_array#Geometric_expansion_and_amortized_cost)).  
Your solution will be tested for correctness of this scheme with a **growth factor of 2**.  
For example, if the actual size evolves as:  
`0 -> 1 -> 2 -> 3 -> 4 -> 5 -> ...`  
then the capacity should evolve as:  
`0 -> 1 -> 2 -> 4 -> 4 -> 8 -> ...`.

## A Word of Advice

This is the first task where the class interface is not provided.  
Don't worry! It is described below, and examples of its usage are available in the tests.

The course instructors understand that this task is quite labor-intensive,  
so they wish you good luck.  

**Don't leave this task for the last moment! It will take more than one evening to complete. :(**

## Implementation Details

You need to implement the `String` class, a simplified version of `std::string`.  
The class must support the following functionality:

* **Default constructor** — creates an empty string; no memory is allocated.
* **Constructor taking `size` and `character`** (in this order) —  
  creates a string of length `size` filled with the character `character`.
* **Constructor from `const char*`**  
  (to determine the length of a C-style string, use `strlen`).
* **Rule of Three**:
    1. Copy constructor.
    2. Copy assignment operator.
    3. Destructor.
* **Method `Clear()`** — sets the size to `0`, but does *not* deallocate memory.
* **Method `PushBack(character)`** — adds the character `character` to the end of the string.
* **Method `PopBack()`** — removes the last character.  
  If the string is empty, it does nothing (unlike `std::string`, where this is UB).
* **Method `Resize(new_size)`** — changes the size to `new_size`.  
  If the capacity is insufficient, a new buffer with capacity `new_size` is allocated.
* **Method `Resize(new_size, character)`** — same as `Resize(new_size)`,  
  but if `new_size > size`, the missing elements are filled with `character`.
* **Method `Reserve(new_cap)`** — changes capacity to `max(new_cap, current_capacity)`.  
  (If `new_cap <= current_capacity`, no action is taken.) The size remains unchanged.
* **Method `ShrinkToFit()`** — reduces `capacity` to `size` (if `capacity > size`).
* **Method `Swap(other)`** — swaps contents with another string `other`. Must run in **O(1)**.
* **Const and non-const index access operator `[]`**.  
  The non-const version must allow modification (`a[1] = 'x'`).
* **Methods `Front()` and `Back()`** — access the first and last character (both const and non-const versions).
* **Method `Empty()`** — returns `true` if the string is empty (`size == 0`).
* **Method `Size()`** — returns the current size.
* **Method `Capacity()`** — returns the current capacity.
* **Method `Data()`** — returns a pointer to the beginning of the internal character array.
* **Comparison operators (`<`, `>`, `<=`, `>=`, `==`, `!=`)**  
  defining **lexicographical order**.
* **Operators `+` and `+=` for string concatenation**.  
  Example: `"ab" + "oba" = "aboba"`.  
  The operation `s += t` **must run in O(|t|)**!  
  Otherwise, the test servers might overheat.
* **Multiplication operator for repeating a string**.  
  Takes a string `str` and a number `n` (in this order) and returns a string of the form:  
  `str + ... + str` (`n` times).  
  Time complexity must be **O(result length)**.  
  **Multiplying a number by a string must cause a compilation error!**  
  (*Algorithmic riddle: does this remind you of a well-known algorithm?*)
* **Stream input and output operators**.
* **Method `std::vector<String> Split(const String& delim = " ")`**  
  — equivalent to Python’s `split()`.
* **Method `String Join(const std::vector<String>& strings)`**  
  — equivalent to Python’s `join()`.

## Notes

1. The solution must consist of **two files**:  
   - `string.hpp` containing class declarations.  
   - `string.cpp` containing method definitions.

2. **You may not use the standard C++ library**,  
   except for `std::vector`, which is allowed **only for `Split` and `Join`**.  
   If you need any utility classes or functions, **implement them yourself**.

3. The testing system requires **strict compliance**  
   with the provided function signatures and naming conventions.  
   If your implementation is incomplete or the interface differs,  
   you will receive a **compilation error**.  
   (No custom names like `MyString`, `__string_`, `push_back`, or `superSolver3000`.)

4. The tests include checks for:
   - **Functionality** (methods must work as specified).
   - **Efficiency** (as explained above).
   - **Memory safety** (memory leaks or invalid accesses **will fail the tests**).
