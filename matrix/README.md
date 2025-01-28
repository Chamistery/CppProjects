# Matrix RU

В рамках данной задачи вам предстоит реализовать класс `Matrix<size_t N, size_t M, typename T = int64_t>` - класс матрицы с
заданными размерами и типом элементов на этапе компиляции.

Операции, которые должны поддерживаться над матрицей:
* Конструктор по умолчанию заполняет матрицу `T()`.
* Конструктор от `std::vector<std::vector<T>>`, заполняющий матрицу элементами вектора. Гарантируется, что размеры вектора будут совпадать с размерами в шаблонах.
* Конструктор от `const T& elem`. Заполняет всю матрицу `elem`. 
* Сложение, вычитание, операторы +=, -=. Сложение и вычитание матриц несоответствующих размеров не должно компилироваться.
Гарантируется, что типы в матрицах в тестах будут поддерживать соответствующие операции.
* Умножение на элемент типа T (гарантируется, что оператор * определен для T)
* Умножение двух матриц. Попытка перемножить матрицы несоответствующих размеров должна приводить к ошибке компиляции.
* Метод `Transposed()`, возвращающий транспонированную матрицу.
* Метод `Trace()` - вычислить след матрицы. Вычисление следа от неквадратной матрицы не должно компилироваться. Подумайте, как это реализовать!
* Оператор `(i, j)`, возвращающий элемент матрицы в _i_-й строке и в _j_-м столбце. Необходимо уметь менять значение для неконстантных матриц.
* Оператор проверки на равенство.

Гарантируется, что в ходе вычисления все элементы лежат в диапазоне типа `T`.

### Примечания
* В данной задаче разрешено использовать `std::vector<T>`.

# ENG

In this task, you need to implement the class `Matrix<size_t N, size_t M, typename T = int64_t>` - a matrix class with fixed dimensions and element type defined at compile time.

Operations supported by the matrix:
* Default constructor initializes the matrix with `T()`.
* Constructor from `std::vector<std::vector<T>>`, filling the matrix with the elements of the vector. It is guaranteed that the vector dimensions match the template dimensions.
* Constructor from `const T& elem`. Fills the entire matrix with `elem`.
* Addition, subtraction, operators `+=`, `-=`. Adding and subtracting matrices of mismatched dimensions should not compile. It is guaranteed that the types in the matrices will support the corresponding operations in tests.
* Multiplication by an element of type `T` (it is guaranteed that the `*` operator is defined for `T`).
* Multiplication of two matrices. Attempting to multiply matrices of mismatched dimensions should result in a compilation error.
* Method `Transposed()`, which returns the transposed matrix.
* Method `Trace()` - calculates the trace of the matrix. Calculating the trace of a non-square matrix should not compile. Think about how to implement this!
* Operator `(i, j)` returns the element at the _i_-th row and _j_-th column of the matrix. It should allow modifying the value for non-constant matrices.
* Equality operator for comparing matrices.

It is guaranteed that during calculations, all elements remain within the range of type `T`.

### Notes
* In this task, you are allowed to use `std::vector<T>`.
