# world.hpp - Data-Oriented Design Toolkit 

![C++17](https://img.shields.io/badge/C++-17-blue?style=for-the-badge&logo=c%2B%2B)
![header-only](https://img.shields.io/badge/header--only-yellow?style=for-the-badge)

---

## 📖 Description / Описание

**EN:** A single-header C++17 library for Data-Oriented Design (DOD), based on compile-time component lists. It provides maximum performance through tuple-based storage, direct memory access, and zero run-time overhead. Perfect for scenarios where the data layout is known in advance.

**RU:** Однозаголовочная C++17 библиотека для Data-Oriented Design (DOD), основанная на списке компонентов, известном во время компиляции. Обеспечивает максимальную производительность благодаря хранению в `std::tuple`, прямому доступу к памяти и отсутствию накладных расходов во время выполнения. Идеальна, когда структура данных фиксирована заранее.

---

## 🎯 The World Concept / Концепция World

**EN:** In traditional OOP, we describe an object class and create multiple instances. In DOD, we think differently: we have a **world** that contains many objects of the same type. Their properties are stored not in scattered objects, but in dense arrays-storages.

**RU:** В традиционном ООП мы описываем класс объекта и создаем множество экземпляров. В DOD мы мыслим иначе: у нас есть **мир (World)**, который содержит множество однотипных объектов. Их свойства хранятся не в разрозненных объектах, а в плотных массивах-хранилищах (Storages).

```cpp
// EN: Instead of this / RU: Вместо этого:
std::vector<Ball> balls;  // Array of objects / Массив объектов

// EN: We define the world with a fixed component list
// RU: Определяем мир с фиксированным списком компонентов
dod::World<Position, Velocity, Health> gameWorld(1000);
```

## 📦 Features / Особенности

**EN:**

- Single-header – just one file world.hpp

- Zero-cost abstractions – direct memory access via pointers, no virtual calls, no map lookups

- Type-safe – component types are checked at compile time

- SIMD-friendly – data stored in dense arrays (struct-of-arrays)

- Minimalism – only what's needed for DOD, no dynamic string-based storage creation

**RU:**

- Single-header – всего один файл world.hpp

- Zero-cost абстракции – прямой доступ к памяти через указатели, без виртуальных вызовов и поисков по map

- Type-safe – типы компонентов проверяются на этапе компиляции

- SIMD-friendly – данные хранятся в плотных массивах (struct-of-arrays)

- Минимализм – только то, что нужно для DOD, без динамического создания хранилищ по строкам

## ⚠️ Warning / Предупреждение
**EN:** ⚡ This approach is not a silver bullet, but in specific cases it can provide significant performance gains. 

**RU:** ⚡ Данный подход не является панацеей, но в отдельных случаях может давать значительный прирост производительности.
