#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
//#include <locale.h>
#include <windows.h>

// === Глобальные переменные ===
int* array = NULL;          // Указатель на массив
int size = 0;               // Текущий размер массива
long swaps = 0;             // Количество перестановок
double time_taken = 0.0;    // Время работы в секундах

// === Прототипы функций ===
void show_menu();
void generate_array();
void load_from_file();
void save_to_file();
void sort_and_display();
void clear_array();
void print_array(int arr[], int n, const char* title);
int read_array_from_csv(const char* filename, int** arr, int* n);  // ИСПРАВЛЕНО: добавлен 3-й параметр
int write_array_to_csv(const char* filename, int arr[], int n);
long bubble_sort(int arr[], int n);
void wait_for_enter();

// === Основная функция ===
int main() {
    SetConsoleOutputCP(65001);
    //setlocale(LC_ALL, "Russian");
    srand((unsigned int)time(NULL));

    int choice;

    do {
        show_menu();
        printf("Ваш выбор: ");
        scanf("%d", &choice);
        getchar(); // Очистка буфера

        switch (choice) {
        case 1:
            generate_array();
            break;
        case 2:
            load_from_file();
            break;
        case 3:
            save_to_file();
            break;
        case 4:
            sort_and_display();
            break;
        case 5:
            clear_array();
            break;
        case 0:
            printf("\nВыход из программы. До свидания!\n");
            clear_array();
            break;
        default:
            printf("\nОшибка: Неверный выбор. Попробуйте снова.\n");
            wait_for_enter();
        }
    } while (choice != 0);

    return 0;
}

// === Отображение главного меню ===
void show_menu() {
    system("clear"); // Для Linux/Mac. Для Windows используйте "cls"
    printf("==================================================\n");
    printf("       СОРТИРОВКА ПУЗЫРЬКОМ (BUBBLE SORT)\n");
    printf("                Вариант 1\n");
    printf("==================================================\n");
    printf("  1. Сгенерировать случайный массив\n");
    printf("  2. Загрузить массив из CSV-файла\n");
    printf("  3. Сохранить отсортированный массив в CSV-файл\n");
    printf("  4. Выполнить сортировку и показать результат\n");
    printf("  5. Очистить массив\n");
    printf("  0. Выход\n");
    printf("==================================================\n");
    if (size > 0) {
        printf("Текущий массив (размер: %d)\n", size);
        print_array(array, size, "");
    }
    else {
        printf("Массив пуст.\n");
    }
    printf("==================================================\n");
}

// === Генерация случайного массива ===
void generate_array() {
    int min_val, max_val;

    printf("\n--- ГЕНЕРАЦИЯ СЛУЧАЙНОГО МАССИВА ---\n");
    printf("Введите количество элементов: ");
    scanf("%d", &size);
    if (size <= 0) {
        printf("Ошибка: Размер должен быть положительным числом.\n");
        wait_for_enter();
        return;
    }

    printf("Введите минимальное значение: ");
    scanf("%d", &min_val);
    printf("Введите максимальное значение: ");
    scanf("%d", &max_val);

    if (min_val > max_val) {
        printf("Ошибка: Минимальное значение не может быть больше максимального.\n");
        wait_for_enter();
        return;
    }

    // Освобождаем старый массив
    if (array) {
        free(array);
        array = NULL;
    }

    array = (int*)malloc(size * sizeof(int));
    if (!array) {
        printf("Ошибка: Не удалось выделить память.\n");
        wait_for_enter();
        return;
    }

    for (int i = 0; i < size; i++) {
        array[i] = (rand() % (max_val - min_val + 1)) + min_val;
    }

    printf("\nМассив успешно сгенерирован!\n");
    print_array(array, size, "Сгенерированный массив");
    wait_for_enter();
}

// === Загрузка из CSV-файла ===
void load_from_file() {


    char filename[256];
    printf("\n--- ЗАГРУЗКА ИЗ CSV-ФАЙЛА ---\n");
    printf("Введите имя файла (например, input.csv): ");
    scanf("%255s", filename);

    int* new_array = NULL;
    int new_size = 0;

    // ИСПРАВЛЕНО: передаем 3 аргумента
    if (read_array_from_csv(filename, &new_array, &new_size)) {
        // Освобождаем старый массив
        if (array) {
            free(array);
            array = NULL;
        }
        array = new_array;
        size = new_size;
        printf("\nМассив успешно загружен из файла '%s'!\n", filename);
        print_array(array, size, "Загруженный массив");
    }
    else {
        printf("\nОшибка: Не удалось прочитать файл '%s'.\n", filename);
        printf("Проверьте, что файл существует и содержит корректные данные.\n");
    }
    wait_for_enter();
}

// === Сохранение в CSV-файл ===
void save_to_file() {
    if (size == 0 || array == NULL) {
        printf("\nОшибка: Массив пуст. Сначала сгенерируйте или загрузите данные.\n");
        wait_for_enter();
        return;
    }

    char filename[256];
    printf("\n--- СОХРАНЕНИЕ В CSV-ФАЙЛ ---\n");
    printf("Введите имя файла для сохранения (например, output.csv): ");
    scanf("%255s", filename);

    // Создаем копию массива для сортировки
    int* sorted_copy = (int*)malloc(size * sizeof(int));
    if (!sorted_copy) {
        printf("Ошибка: Не удалось выделить память.\n");
        wait_for_enter();
        return;
    }
    memcpy(sorted_copy, array, size * sizeof(int));

    // Сортируем копию
    bubble_sort(sorted_copy, size);

    if (write_array_to_csv(filename, sorted_copy, size)) {
        printf("\nОтсортированный массив успешно сохранен в файл '%s'!\n", filename);
    }
    else {
        printf("\nОшибка: Не удалось сохранить файл '%s'.\n", filename);
    }

    free(sorted_copy);
    wait_for_enter();
}

// === Сортировка и отображение ===
void sort_and_display() {
    if (size == 0 || array == NULL) {
        printf("\nОшибка: Массив пуст. Сначала сгенерируйте или загрузите данные.\n");
        wait_for_enter();
        return;
    }

    printf("\n--- СОРТИРОВКА ПУЗЫРЬКОМ ---\n");

    // Создаем копию массива для сортировки
    int* sorted_array = (int*)malloc(size * sizeof(int));
    if (!sorted_array) {
        printf("Ошибка: Не удалось выделить память.\n");
        wait_for_enter();
        return;
    }
    memcpy(sorted_array, array, size * sizeof(int));

    // Выполняем сортировку с замером времени
    clock_t start = clock();
    swaps = bubble_sort(sorted_array, size);
    clock_t end = clock();
    time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\nИсходный массив:\n");
    print_array(array, size, "");

    printf("\nОтсортированный массив:\n");
    print_array(sorted_array, size, "");

    printf("\n=== СТАТИСТИКА ===\n");
    printf("Размер массива: %d\n", size);
    printf("Количество перестановок: %ld\n", swaps);
    printf("Время работы: %.6f секунд\n", time_taken);

    free(sorted_array);
    wait_for_enter();
}

// === Очистка массива ===
void clear_array() {
    if (array) {
        free(array);
        array = NULL;
        size = 0;
        swaps = 0;
        time_taken = 0.0;
        printf("\nМассив очищен.\n");
    }
    else {
        printf("\nМассив уже пуст.\n");
    }
    wait_for_enter();
}

// === Вспомогательные функции ===

// Вывод массива в консоль
void print_array(int arr[], int n, const char* title) {
    if (n == 0 || arr == NULL) {
        printf("  (пусто)\n");
        return;
    }

    if (strlen(title) > 0) {
        printf("%s:\n", title);
    }

    printf("  [");
    for (int i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) {
            printf(", ");
        }
        // Перенос строки для длинных массивов
        if ((i + 1) % 20 == 0 && i < n - 1) {
            printf("\n   ");
        }
    }
    printf("]\n");
}

// Чтение массива из CSV-файла
int


read_array_from_csv(const char* filename, int** arr, int* n) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return 0;
    }

    int* buffer = NULL;
    int capacity = 10;
    int count = 0;
    char line[4096];

    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return 0;
    }

    // Удаляем символы новой строки
    line[strcspn(line, "\n")] = 0;
    line[strcspn(line, "\r")] = 0;

    char* token = strtok(line, ",");
    while (token != NULL) {
        // Пропускаем пробелы
        while (*token == ' ') token++;

        // Проверяем, что это число
        int is_number = 1;
        for (int i = 0; token[i] != '\0'; i++) {
            if (!isdigit(token[i]) && token[i] != '-') {
                is_number = 0;
                break;
            }
        }

        if (!is_number) {
            free(buffer);
            fclose(file);
            return 0;
        }

        if (count >= capacity) {
            capacity *= 2;
            int* new_buffer = (int*)realloc(buffer, capacity * sizeof(int));
            if (!new_buffer) {
                free(buffer);
                fclose(file);
                return 0;
            }
            buffer = new_buffer;
        }

        buffer[count++] = atoi(token);
        token = strtok(NULL, ",");
    }

    fclose(file);

    if (count == 0) {
        free(buffer);
        return 0;
    }

    // Уменьшаем размер до фактического
    int* final_buffer = (int*)realloc(buffer, count * sizeof(int));
    if (!final_buffer && count > 0) {
        final_buffer = buffer;
    }

    *arr = final_buffer;
    *n = count;
    return 1;
}

// Запись массива в CSV-файл
int write_array_to_csv(const char* filename, int arr[], int n) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        return 0;
    }

    for (int i = 0; i < n; i++) {
        fprintf(file, "%d", arr[i]);
        if (i < n - 1) {
            fprintf(file, ",");
        }
    }
    fprintf(file, "\n");
    fclose(file);
    return 1;
}

// Ожидание нажатия Enter
void wait_for_enter() {
    printf("\nНажмите Enter для продолжения...");
    getchar();
}

// === РЕАЛИЗАЦИЯ АЛГОРИТМА СОРТИРОВКИ ПУЗЫРЬКОМ ===
long bubble_sort(int arr[], int n) {
    long swap_count = 0;
    int i, j, temp;

    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swap_count++;
            }
        }
    }

    return swap_count;
}