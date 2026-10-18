# Геометрия террас: замеры и предложение оптимизации

Дата: 2026-10-16. Конфигурация: seed 1337, мир 2048 м, island_size 1200,64 м,
resolution 1024, ячейка 2 м, восемь надводных и восемь подводных террас.
Исходное требование аудита: **контуры сохранять строго без изменений**, включая
дальний вид. Позднее пользователь разрешил управляемое упрощение дальнего
контура отдельным слайдером 0–50%, по умолчанию 0%. Ближайший LOD остаётся точным.
Количество LOD включает ближайший: для N=5 и P=50% ряд равен
0 / 12,5 / 25 / 37,5 / 50%. Ниже приведены замеры и оценки именно точного режима.

После аудита пользователь выбрал проектировать Continuous LOD сразу вместе
с точным упрощением и отдельным грубым LOD для shadowmap. Актуальный проект:
[Точное упрощение террас и Continuous LOD](superpowers/specs/2026-10-16-terrain-continuous-lod-design.md).
Приведённая ниже рекомендация отложить решение о CLOD сохранена как вывод
первоначального аудита; выбор пользователя её заменяет.

## Что измерено

Воспроизводимый запуск из корня репозитория:

```powershell
./gbScripts/profile-terrain-geometry.ps1
```

Это отдельный CPU-профайлер, собранный MSVC с `/O2`. Он вызывает настоящие
генераторы и render cache, но не меняет уровень, настройки или приложение.
Числа обозначают треугольники в геометрии до отсечения. Это не GPU timings,
не сумма всех проходов и не измерение FPS. Варианты без травы нужны для
раздельного учёта геометрии; саму траву профайлер также генерирует отдельно.

| Сценарий | Треугольники | Горизонтальные | Уступы | Вершины |
|---|---:|---:|---:|---:|
| Исходный остров | 20 160 | 9 920 | 10 240 | 60 480 |
| Начальный остров после разбиения и группировки редактора | 55 126 | 38 280 | 16 846 | 165 378 |
| Весь остров в сеточном представлении редактора, без изменения данных высот | 621 302 | 521 802 | 99 500 | 518 560 |
| Геометрия в кэше после одного штампа Terrain диаметром 32 м в (12, 2), target 7 | 57 179 | 40 169 | 17 010 | 166 057 |
| Все чанки в сеточном представлении после этого штампа | 621 630 | 521 966 | 99 664 | 519 052 |

**621 тысяча — сценарий полной перестройки сеточным генератором, а не стартовый
кадр и не результат одного штампа.** Локальное редактирование заменяет только
затронутые чанки. При дальнейшей обработке острова доля плотной сетки растёт.
Таблица кэша после штампа не включает отдельные стыкующие юбки;
профайлер дополнительно выводит фактически публикуемые Terrain-группы.

Отдельно начальная трава: **2 795 414 треугольников, 5 590 828 вершин, 94 исходных
пакета**. Её нельзя приписывать сетке площадок. До группировки это около 186,6 МиБ
GPU-буферов (32 байта на вершину и 16-битные индексы), без дополнительных копий
и служебных структур. Поэтому оптимизация только террас не обещает такой же
процент ускорения всего кадра.

## Откуда берётся лишняя геометрия

1. `terrain_mesh_builder.cpp::build` строит 320 отсчётов каждого контура.
   Площадка-кольцо содержит два треугольника на сегмент, стена — ещё два;
   верхняя площадка использует веер. Итого `(4 * 16 - 1) * 320 = 20 160`.
   Здесь плоские поверхности уже сравнительно экономны; контурные сегменты
   нельзя просто выбрасывать при требовании точного сохранения силуэта.
2. `TerrainEditorRenderCache::initialize` обрезает исходные треугольники по сетке
   чанков 16 × 16 ячеек (32 × 32 м). Большие кольцевые треугольники пересекают
   несколько чанков. Обрезка и повторная триангуляция увеличивают число до 55 126.
   `append_triangle` записывает три независимые вершины на каждый треугольник.
3. `build_editor_chunk` после редактирования обрабатывает каждую ячейку как два
   треугольника, обрезая их по порогам террас. Даже совершенно ровный чанк имеет
   **512 треугольников и 289 вершин**. Удаления внутренних рёбер площадок нет.
   У сеточного варианта около **84% треугольников горизонтальные**.
4. `merge` объединяет чанки в группы 4 × 4 (128 × 128 м), но сохраняет
   исходную топологию. Группировка уменьшает число draw calls, не количество
   треугольников. Для текущего острова получается 67 непустых Terrain-групп.
5. Общий счётчик кадра включает повторные отрисовки в разных проходах.
   Нельзя сравнивать его напрямую с числом уникальных треугольников острова.

## Уже существующие механизмы

- Основной проход террас **уже имеет AABB/frustum-culling**:
  `shape_3d::setup_components()` задаёт `e_box`, `ces_render_system` проверяет его
  по frustum камеры. Базовое значение `e_none` в компоненте здесь переопределено.
- CPU-редактирование и подготовка пакетов уже выполняются через `std::async`;
  загрузка GPU-буферов остаётся в игровом потоке. Смена LOD не должна запускать
  полный rebuild или навигацию.
- Тени имеют отдельные прокси и собственную проверку видимости относительно
  матрицы света. Внекамерный объект может отбрасывать видимую тень.
- Маска суши строится в проекции карты взаимодействия с водой, а не в проекции
  камеры. Её нельзя отсекать по frustum основной камеры: это изменит береговую пену.
- `terrain_parts()` используется для `BuildingSurfaceSupport` и навигации.
  Эти данные должны оставаться независимыми от камеры и экранного LOD.
- Текстура террас в `shader_island_terrain.frag::terrain_texture` привязана к
  мировым координатам и идентификатору террасы. Для её детализации не нужна
  плотная геометрическая сетка. При этом нормали стен и идентификаторы уступов
  сохранять обязательно.

## Измеримый потенциал без изменения контура

### Повторно использовать одинаковые вершины

Профайлер сравнивает точные биты всех атрибутов, а не только позиции.
Для начальных Terrain-групп 165 378 записей содержат лишь **58 132 различных
вершины внутри своих пакетов**. Индексирование одинаковых атрибутов может убрать
около **64,85% записей вершин**; число треугольников само по себе не изменится.
GPU-payload при прежних индексах сократился бы примерно с 5,36 до 2,09 МиБ.
Швы нормалей, материалов и outline ID объединять нельзя.

### Убрать внутренние вершины плоских областей

Для ровного чанка 16 × 16 ячеек:

- Сейчас: 512 треугольников.
- С сохранением всех 64 граничных вершин: 62 треугольника, **−87,89%**.
- Два треугольника возможны только там, где коллинеарные точки внешнего
  периметра согласованно удалены и у соседей. Просто оставить T-junctions нельзя.

Профайлер дополнительно считает внутренние вершины областей с постоянными
высотой и всеми интерполируемыми атрибутами. Топологическая оценка при удалении
таких вершин, с сохранением границ этих областей:

| Представление | Сейчас | Оценка после удаления внутренних вершин |
|---|---:|---:|
| Начальные Terrain-группы | 55 126 | 43 194 |
| Полностью сеточный остров, каждый чанк отдельно | 621 302 | 268 966 |
| Полностью сеточный остров, внутри текущих render-групп | 621 302 | 232 526 |

Это **оценка потенциала, не результат работающего упрощателя**. Она не учитывает
возможные отказы безопасной триангуляции и консервативную фиксацию дополнительных
стыков. Она не означает, что можно сократить стартовые террасы на 62%:
такой потенциал относится к плотному сеточному варианту.

## Рекомендуемый первый этап: точное упрощение render mesh

1. Оптимизировать только Terrain-пакеты после группировки: точное индексирование
   одинаковых вершин и повторная триангуляция плоских областей.
2. Зафиксировать каждое ребро реального контура, отверстий, границ материалов,
   нормалей, внешнего периметра пакета и стыков с сохранёнными чанками.
   Стены не выпрямлять и их нормали не усреднять.
3. Удалять только внутренние вершины заведомо плоских областей с совпадающими
   атрибутами. Для неоднозначной или некорректной топологии оставлять исходные
   треугольники. Никакого приближения высот или координат контура.
4. Оставить исходные CPU-чанки для логики, генерации травы, навигации и проверки
   площади зданий. Смена триангуляции не должна менять случайное распределение
   травы. Маска берега и тени могут использовать тот же геометрически
   эквивалентный оптимизированный render mesh.
5. Выполнять оптимизацию в существующем CPU worker только для изменённых
   Terrain-групп. Не перерабатывать всю сцену при движении камеры или кисти.

Предполагаемое место интеграции: `terrain_editor_render_cache.cpp` и небольшой
приложенческий CPU-модуль упрощения в `gbDemo`, с добавлением в Windows/Xcode
targets. Общий renderer и сохранённый формат уровня менять не требуется.

## Нужен ли Continuous LOD

Исходный [CDLOD Филипа Стругара](https://github.com/fstrugar/CDLOD) строит
quadtree регулярных сеток над heightmap и плавно переводит вершины между
уровнями детализации. Это хороший исходный принцип, но не готовая замена
геометрии наших раздельных площадок и вертикальных стен.

При строгом сохранении контуров нельзя позволять morphing сдвигать кромки,
срезать изгибы или превращать отвесный уступ в склон. Кроме того, **плоскую
площадку можно рисовать редкой сеткой и вблизи**: плотность текстуры остаётся
прежней. После точного упрощения переключать её между плотным и редким LOD
может быть просто нечего.

Поэтому рекомендуется сначала реализовать и измерить первый этап, затем
решать о второй системе по оставшейся геометрии и CPU/GPU-времени. Если она
потребуется, допустим следующий ограниченный вариант:

- Quadtree над пространственными группами с AABB и оценкой ошибки поверхности.
- Плоские внутренние области допускают укрупнение с нулевой геометрической
  ошибкой; узлы, пересекающие контуры, сохраняют исходные рёбра и стены.
- Соседние выбранные узлы обязаны иметь согласованные рёбра, без щелей,
  T-junctions и скрывающих их дополнительных юбок.
- Изменение уровня детализации влияет только на представление для рендера,
  не на TerrainData, траву, здания, лестницы или маршруты персонажей.
- Видимость основного изображения, теней и карты берега определяется
  отдельно. Нельзя выключать сущность целиком только потому, что она за камерой.
- Камера не должна провоцировать перестроение CPU-геометрии или навигации.

Этот второй этап требует отдельной интеграции и измерения. Обычное снижение
resolution, уменьшение числа отсчётов контуров или общий decimation всего
острова нарушают выбранное требование и не предлагаются.

## Дополнение: отдельный LOD для shadowmap

Пожелание пользователя: в проходах генерации shadowmap использовать самый
грубый доступный LOD независимо от детализации основного изображения.

- Для `ws.sun.near` и `ws.sun.far` выбирать отдельный меш объекта для отбрасывания
  тени. Его назначать существующим shadow-прокси, не менять меш видимой сущности.
- Для террас даже самый грубый вариант сохраняет исходные контурные сегменты,
  вертикальные стены и высоты; упрощается внутреннее разбиение площадок.
  Это совместимо с подтверждённым требованием неизменных контуров.
- Если у объекта ещё нет отдельного LOD, использовать его имеющийся меш.
  Не заменять здание или персонажа произвольной заглушкой. Сохранить анимацию,
  skinning и используемое shadow-шейдером отсечение прозрачных частей.
- Матрицы света, depth bias и фильтрацию shadowmap не менять. Проверку видимости
  выполнять по границам выбранного shadow-меша в проекции света; внекамерные
  объекты по-прежнему могут отбрасывать видимые тени.
- Хранить подготовленный shadow-меш вместе с поколением render-кэша. Обновлять
  после изменения соответствующей области, без пересборки при движении камеры.
  Замену GPU-ресурсов и их отложенное освобождение выполнять существующим путём.
- Проверить оба прохода отдельно: число отправленных треугольников, совпадение
  силуэтов теней, стыки террас и низкое солнце, при котором ошибки особенно заметны.

Сейчас `ces_sun_shadow_system::on_feed_end` передаёт один и тот же исходный
меш обоим shadow-прокси. Отдельный выбор LOD пока не реализован; это добавленное
требование к предстоящей оптимизации.

## Проверка будущей реализации

- CPU: площади и покрытия площадок, высоты, ориентирование треугольников,
  все граничные сегменты, отверстия, соединённость, ID и нормали стен,
  разные материалы, лимит 16-битных индексов, несколько seed и редактируемые формы.
- Независимость логики: совпадают grass roots и параметры травы, размещение
  зданий и лестниц, навигация, undo/redo, сохранение/загрузка.
- Кадры до/после при одинаковых камере, времени суток, времени волн и ветра:
  общий вид, близкая кромка, скользящий угол, берег, здания, тени и пена.
  Совпадение формы не следует выдавать за побитовую идентичность GPU-кадра без
  проверки: другая триангуляция может проявить погрешности интерполяции.
- F8: сравнить каждый проход и фактически отправленные треугольники.
  Отдельно измерить CPU worker/upload и GPU-время, а не обещать FPS из числа полигонов.
- Проверить Vulkan; OpenGL и Metal требуют собственной сборки и запуска.

Текущее состояние: анализ и воспроизводимый профайлер готовы. Runtime-геометрия,
шейдеры, LOD и поведение приложения в рамках этого аудита не изменялись.


## Implemented optimization and verification

The exact optimizer operates only on render Terrain batches. It indexes identical
attributes and removes flat interior vertices; gradients, walls, outline IDs and
every boundary segment remain fixed. Logical source meshes, grass generation,
building/stair support and resident navigation remain unchanged.

Measured CPU audit (triangles, excluding grass):

| Case | Before | Exact optimized | Reduction |
|---|---:|---:|---:|
| Starter render batches | 55,126 | 43,314 | 21.4% |
| Full canonical editor island | 621,302 | 232,594 | 62.6% |
| Canonical island after a stamp | 621,630 | 232,950 | 62.5% |
| Changed Terrain upload region | 6,459 | 3,843 | 40.5% |

The CLOD forest is rooted at existing 128m render groups and 16-bit parts,
subdivided to 16-cell leaves. Root/child variants retain identical artificial seam
segments, so spatial depth differences require no skirts. Contour transitions
keep heights fixed and reject inverted top faces, swept-contour intersections
and contacts with other contours. Protected building/stair/tree footprints limit
reduction. Selection uses projected error and cost hysteresis. Morph intervals
are ordered even when errors are equal, so intermediate levels cannot be skipped.

Native Vulkan measurements on the fixed starter camera: P=0 selected 43,276
triangles from 43,314 exact root inputs; UI P=48 selected 27,447. Selection took
approximately 0.24-0.32ms. At rest the upload count remained 67; after the P=48
generation the GPU cache held 18MiB. These are Terrain statistics, not total scene
counts or an FPS guarantee; grass still accounts for millions of triangles.
The fixed-time P=0 image comparison differed on 1,531 of 372,000 inspected scene
pixels, with 227 exceeding 16/255 and a mean RGB difference of 0.049/255. The diff
shows sparse subpixel shading/rasterization changes, without silhouette movement.

GPU cache has a 64MiB soft limit; current main/water/shadow buffers remain pinned.
Child uploads are lazy (at most one new child per frame). Old meshes use the
existing delayed-retirement path. Unchanged group signatures reuse CPU variants
and GPU buffers. Terrain/load/settings revisions guard worker results and upload
queue entries. Changing settings does not rebuild navigation. Shadow maps select
independently by light-projection error; shoreline masks always use exact roots.

CPU checks cover flat boundaries, holes, gradients, supports, 16-bit packing,
annular contours, protected points, intermediate triangle orientation, contour
contacts, CLOD thresholds, culling, versions, layout and JSON compatibility.
Native frames/logs are in the ignored
`gbDemo/output/island-check/terrain-lod-work` verification workspace.

The normal Debug command encountered an occupied gb_demo.pdb (C1041). Debug
compiled using isolated compiler/linker PDB and executable paths; Release built
normally. OpenGL and Metal/Apple runtime remain unverified.
The unrelated resident behavior runner currently fails its `stops>100` assertion
on the enlarged starter island; preceding crowd and surface/path safety assertions
pass. No input source of that runner was changed by this renderer optimization.


Final integration run: `check-terrain-lod-demo.ps1 -Exercise` verified N=1,
N=8, N=5/P=50, dragging beyond the track, F8 Save, sculpt and Undo/Redo.
A subsequent fresh process loaded the saved N=5/P=50 and selected 28,622 Terrain
triangles (43,276 at P=0). Both exited normally with clean Vulkan validation.
A 63,200-pixel distant-ocean region was bit-identical between P=0/P=50 frames.
The smoke runner restores GraphicsSettings byte-for-byte; the shipped defaults
remain N=5/P=0. Native terrain-file-dialog Load and Apple backends were not
exercised; terrain roundtrip/load invalidation was checked in the CPU runners.

Local Terrain/footprint invalidation republishes only intersecting render groups,
with unchanged group generations and buffers reused. Global N/P changes have a
separate render-only invalidation flag. A focused four-group regression checks
one published group for local LOD dirtiness and all four for settings dirtiness,
with zero logical chunk remeshes in both cases.

Implementation decisions: keep the approved dirty checkout in place and preserve
its baseline/log workspace; root the forest at existing upload groups rather than
building one world-wide GPU mesh; preserve identical seams rather than requiring
spatial neighbor balancing; pool only selected render entities. The first two
retain the existing workflow/upload boundaries (with less cross-group reduction);
the latter two avoid skirts and thousands of hidden ECS receivers. No engine-core
changes, branch operations, commits or publishing were needed for this feature.

The fresh read-only review found and prompted fixes for skipped equal-error LOD
transitions, contour-to-hole contact, and toolbar overlap at 1024x768. Its final
formal report was unavailable because the reviewer hit a usage limit; the author
completed source and runtime verification. No second reviewer was dispatched.

The final contour acceptance check also evaluates the quadratic signed area of
every affected top triangle across the entire simultaneous morph, including its
interior extremum. A dedicated regression rejects a morph with valid endpoint
triangles but an inverted midpoint. Final Release/isolated Debug builds and the
full Vulkan Exercise run passed after this safeguard was added.
