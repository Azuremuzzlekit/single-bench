// bench.h

/*
 * by Azuremuzzlekit
 * мой GitHub: https://github.com/Azuremuzzlekit
 * 
 * ЛИЦЕНЗИЯ: MIT
 * 
 * bench.h - библиотека для создания бенчмарков повышенной точности блоков кода
 */

#ifndef BENCH_H
#define BENCH_H

#include <time.h>
#include <stdint.h>
#include <stdio.h>

/*
 * Макрос для измерения времени выполнения блока кода в наносекундах.
 * Использует CLOCK_MONOTONIC_RAW для максимальной точности.
 * 
 * Параметры:
 *   name - название теста (для вывода)
 *   code - измеряемый блок кода (в фигурных скобках)
 *   iterations - количество итераций для усреднения
 * 
 * Особенности:
 * - Избегает переупорядочения инструкций через memory barrier
 * - Вычисляет минимальное/максимальное/среднее время
 * - Нулевые накладные расходы за пределами измеряемой зоны
 */
#define BENCH(name, code, iterations) do { \
    struct timespec _bench_start, _bench_end; \
    uint64_t _bench_min = UINT64_MAX, _bench_max = 0, _bench_total = 0; \
    \
    /* Основной цикл измерений */ \
    for (int _bench_i = 0; _bench_i < iterations; _bench_i++) { \
        /* Memory barrier и получение времени ДО выполнения кода */ \
        asm volatile ("" ::: "memory"); \
        clock_gettime(CLOCK_MONOTONIC_RAW, &_bench_start); \
        \
        /* Измеряемый блок кода */ \
        { code; } \
        \
        /* Memory barrier и получение времени ПОСЛЕ выполнения кода */ \
        asm volatile ("" ::: "memory"); \
        clock_gettime(CLOCK_MONOTONIC_RAW, &_bench_end); \
        \
        /* Вычисления времени выполнения */ \
        uint64_t _bench_ns = ((_bench_end.tv_sec - _bench_start.tv_sec) * 1000000000ULL) \
                            + (_bench_end.tv_nsec - _bench_start.tv_nsec); \
        \
        /* Обновление статистики */ \
        _bench_total += _bench_ns; \
        _bench_min = _bench_ns < _bench_min ? _bench_ns : _bench_min; \
        _bench_max = _bench_ns > _bench_max ? _bench_ns : _bench_max; \
    } \
    \
    /* Вывод результатов */ \
    printf("[%s]\nAvg     %7.2fns\nMin     %6luns\nMax     %6luns\nRuns     %d\n\n", \
           name, \
           (double)_bench_total / iterations, \
           _bench_min, \
           _bench_max, \
           iterations); \
} while(0)

/*
 * BENCH_RDTSC - версия с использованием счётчика циклов процессора (Time Stamp Counter).
 * Дает максимально точные результаты в процессорных циклах, но зависит от частоты CPU.
 * 
 * Особенности:
 * - Использует RDTSCP вместо RDTSC для сериализации конвейера
 * - Измеряет непосредственно циклы процессора
 * - Не зависит от системных часов
 * - Требует калибровки на системах с динамической частотой
 */
#define BENCH_RDTSC(name, code, iterations) do { \
    uint64_t _bench_start, _bench_end; \
    uint64_t _bench_min = UINT64_MAX, _bench_max = 0, _bench_total = 0; \
    \
    for (int _bench_i = 0; _bench_i < iterations; _bench_i++) { \
        /* Чтение TSC с сериализацией (RDTSCP + регистры) */ \
        asm volatile ( \
            "RDTSCP\n" /* Читаем счётчик и процессорный ID */ \
            "shl $32, %%rdx\n" /* Сдвигаем верхние 32 бита */ \
            "or %%rdx, %%rax\n" /* Объединяем в 64-битное значение */ \
            "mov %%rax, %0" \
            : "=m" (_bench_start) \
            : \
            : "rax", "rdx", "rcx" \
        ); \
        \
        /* Барьер для изоляции измеряемого кода */ \
        asm volatile ("" ::: "memory"); \
        { code; } \
        asm volatile ("" ::: "memory"); \
        \
        /* Повторное чтение TSC */ \
        asm volatile ( \
            "RDTSCP\n" \
            "shl $32, %%rdx\n" \
            "or %%rdx, %%rax\n" \
            "mov %%rax, %0" \
            : "=m" (_bench_end) \
            : \
            : "rax", "rdx", "rcx" \
        ); \
        \
        /* Расчёт циклов */ \
        uint64_t _bench_cycles = _bench_end - _bench_start; \
        _bench_total += _bench_cycles; \
        _bench_min = _bench_cycles < _bench_min ? _bench_cycles : _bench_min; \
        _bench_max = _bench_cycles > _bench_max ? _bench_cycles : _bench_max; \
    } \
    \
    printf("[%s]\nAvg     %7.2f cycles\nMin     %6lu\nMax     %6lu\nRuns     %d\n\n", \
           name, \
           (double)_bench_total / iterations, \
           _bench_min, \
           _bench_max, \
           iterations); \
} while(0)

#endif // BENCH_H
