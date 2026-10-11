# Оптимизация Game Box без снижения качества

Проверка: 2026-10-11. Область работы — ранее выполненный [аудит core](CORE_PERFORMANCE_AUDIT.md), сцена острова, океан, terrain и 200 персонажей. Предыдущие пользовательские изменения сохранены. Это отчёт о выполненных оптимизациях, а не утверждение, что все 53 пункта аудита устранены.

## Что изменено

- **Vulkan:** Release больше не включает validation по умолчанию; Debug сохраняет его. `GB_VULKAN_VALIDATION=1/0` задаёт явное переопределение. Descriptor writes объединены в один вызов, alignment кешируется. Uniform memory и descriptor pools растут блоками вместо фиксированных потолков; повторное использование привязано к существующему fence изображения swapchain.
- **Динамические VBO/IBO:** HUD использует mapped coherent buffers и снимки отправленных CPU-данных отдельно для каждого fenced изображения. Несколько обновлений внутри одного кадра получают отдельные слоты, поэтому не перезаписывают данные уже записанных draw calls. Статические загрузки копируют используемый размер, staging освобождается после завершения копирования. Асинхронный streaming текстур этим не реализован.
- **Рендер:** кандидаты группируются по техникам с сохранением порядка и fallback при изменении материалов внутри callback. Заимствованные контейнеры заменяют лишние копии; точная normal matrix кешируется по версии transform; fullscreen quad переиспользуется. Кеш нативных pipelines учитывает состояние и поколения shader/pass; освобождается перед уничтожением device. В измеренной сцене создано 26 нативных pipelines.
- **ECS и анимации:** повторно используется только математически идентичная pose с проверкой ревизий исходных кадров. Crossfade/слои и callbacks сохраняют прежний порядок и частоту. Убраны копии обходов костей, повторные lookup масок, утечки массивов матриц и лишние обновления неподвижных transforms. Массовые изменения ECS queries сохраняют next-tick поведение и порядок, включая большие очереди.
- **Навигация:** общие immutable reachable regions и scratch BFS устраняют дублирование памяти и аллокаций. Порядок соседей, выбранные пути, ограничения террас и число жителей сохранены. Нет animation LOD, пропуска кадров анимации или уменьшения населения.
- **Ресурсы и jobs:** condition-variable wakeup, синхронизация exactly-once/affinity и зависимостей; сериализация вне общего mutex; GPU commit и callbacks на game-loop. CPU shader preparation перенесена в сериализацию с ограниченными кешами точного исходника. FBX читается из stream, кадры заполняются непосредственно. Кеш подготовленных/retargeted clips применяется до дорогой подготовки каждого персонажа. XML кеш возвращает независимые DOM и проверяет содержимое файлов. Исправлены PNG row-pointer allocation и lifetime listener контроллера.
- **Время:** fractional monotonic dt и FPS между началами кадров; pause/resume сбрасывает накопленное время. Fixed-step Box2D не изменён.

Подробные границы и соответствие ID аудита: [ECS](optimization-ecs.md), [ресурсы](optimization-resources.md), [ocean/terrain](optimization-ocean-terrain.md).

## Ocean и terrain

Terrain создаётся один раз: 60 480 вершин, 20 160 треугольников. Ocean geometry также создаётся один раз; волны деформируются в шейдере. Повторной CPU-генерации мешей каждый кадр не обнаружено. Добавлены точные reserves, обновление mesh взаимодействий теперь выполняется при изменении его identity; transforms движущихся объектов продолжают обновляться каждый кадр.

В базовом ocean shader исключено вычисление пены и чтение field, используемых только отключёнными debug modes. Видимая пена по-прежнему рисуется отдельным проходом. В SSAO направление вычисляется один раз на четыре кольца. Соответствующие Metal shaders обновлены, но Apple runtime не проверен.

Разрешение, форматы targets, 12 JFA проходов, 32 SSAO samples, 25 blur taps, SSR march/refinement, геометрия, волновое поле и настройки видимой пены сохранены. GPU расчёт interaction distance field остаётся существенным bottleneck: в исходном контрольном замере около 1,91 мс из 4,66 мс GPU. Ocean shader занимал около 1,12 мс. Океан нельзя считать полностью оптимизированным.

## Измерения и ограничения сравнения

Windows Vulkan, i3-12100F, RTX 3060 Ti, 1280×720, 200 персонажей. Исходные замеры — 2026-10-10. Числа и настройки сохранены в [исходных измерениях](measurements.json) и [итоговых измерениях](optimization-measurements.json).

Обычный исходный Release с включённым validation: **13,697 мс/кадр (~73 FPS)**. Изменение default Release устраняет существенные диагностические накладные расходы; это не снижение визуального качества. Для оценки самих оптимизаций следует сравнивать запуски с одинаковым validation.

Финальный обычный Release без временной инструментации: **4,510 мс/кадр (~222 FPS)**, 56 агрегатов / более 30 секунд после первого отчёта, private memory **468 MiB**. Validation по умолчанию выключен, implicit layers отключены как в контрольном baseline; stderr пуст. Это приблизительно 3× относительно прежнего default Release, но не 3× ускорение рендера при одинаковых диагностических настройках. До оптимизации Release с отдельно отключённым validation уже работал значительно быстрее своего default.

Контрольные 30-секундные запуски временного профайлера с validation/implicit layers выключенными:

| Показатель | До | После |
|---|---:|---:|
| Среднее wall time | 4,757 мс | 5,301 мс |
| p99 отдельных кадров | 10,715 мс | 12,458 мс |
| CPU, эквивалент занятых логических ядер | 0,684 | 0,565 |
| Private memory процесса | ~668 MiB | ~469 MiB |
| CPU descriptor updates | 0,299 мс | 0,242 мс |
| HUD staging uploads после прогрева | 124 вызова | 0 |

Снижение CPU и private memory подтверждается этими запусками, но **ускорение общего GPU/frame time и улучшение p99 не подтверждены**. Изменяющиеся волны, SSR, частоты GPU и scheduling влияют на последовательные замеры. Private memory — память процесса, не VRAM. Конкурирующий запуск и старый probe с `getenv` на каждом draw исключены из сравнения. Native allocator stress также не является benchmark.

## Проверки

- Полная Windows Debug-сборка прошла: `./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64` (gbCore, gbSound, gbDatabase, gbUI, gbNetwork, gbDemo).
- Полная обычная Release-сборка после удаления профайлера прошла: `./gbScripts/build-gbDemo.ps1 -Configuration release-client -Platform x64`. Затем выполнены отдельный 30-секундный benchmark и финальный fixed-time capture; совпадение с baseline подтверждено заново.
- CPU ocean/terrain checks прошли; сохранены seams, геометрия и buoyancy.
- `./gbScripts/check-render-statistics.ps1` прошёл: reset кадра, submitted triangles/instances, empty/non-triangle draws и FPS.
- Scheduler, shader precompile, XML cache и FBX stream/frame probes прошли. Все sampled TRS в 91 idle + 25 walk кадрах совпадают точно с независимым ufbx reference.
- `./gbScripts/performance/check-ecs-optimizations.ps1 -Configuration release` прошёл с реальным `gb_core.lib`: pose equality/revision invalidation, query order/next-tick, transforms/masks; navigation и 200-resident confinement checks.
- Debug Vulkan: multiframe ocean, SSAO on/off/mask и 20-секундная автономная прогулка 200 жителей прошли без ошибок validation. SSAO image check исключает изменяющийся HUD, проверяя сцену, контакты, фон и открытую воду.
- Native Vulkan stress: 1 100 descriptor sets на кадр, uniform growth и несколько VBO/IBO updates внутри кадра; validation без ошибок. Временная инструментация удалена, восстановление всех 11 snapshots проверено по байтам.
- Fixed-time before/after image при одинаковой камере и без жителей: **все пиксели сцены совпадают точно** за пределами FPS HUD `(900,0)-(1280,120)`. Это доказательство для проверенного вида, не всех камер/подводных состояний или Apple backend.
- `git -c core.safecrlf=false diff --check` прошёл; `core.safecrlf=false` подавляет предупреждения нормализации LF/CRLF, не проверку whitespace. Воспроизводимый сбор результатов: `python gbScripts/performance/collect-optimization-results.py`.

Это focused probes и demo smoke, не engine-owned unit-test suite. OpenGL/Metal runtime, Apple targets и gbServer остаются **UNKNOWN**. Предупреждения компилятора не устранены целиком. Полный regression shutdown/многократных transitions для всех backend не заявляется.

После пользовательского сообщения о shutdown crash проведена отдельная строгая
[проверка lifetime Vulkan](VULKAN_SHUTDOWN_FIX.md). Предыдущие smoke-проверки не
требовали exit code 0 после закрытия; их нельзя считать доказательством успешного
shutdown. Новая проверка воспроизвела исходный `0xC0000005` и проверяет обычное
закрытие игрового окна с validation, остающимся активным до уничтожения device.

## Что остаётся

Не внедрены GPU skinning instancing/indirect crowd submission, общий асинхронный upload/retirement API, полная политика eviction ресурсов и статического/динамического field reuse, ping-pong aliasing targets, явная pass attachment policy, изменение Box2D timestep и Apple synchronization. Эти изменения затрагивают архитектурные контракты; их нельзя выдавать за исправленные благодаря локальным кешам.

Следующий большой GPU резерв — lifetime/aliasing и separation статических/динамических данных interaction field при сохранении полного качества. Для текстур нужен общий механизм lifetime staging и ресурсов до завершения submission. Pipeline cache сейчас освобождается на device teardown; долгие серии transitions могут накопить дополнительные поколения pipelines.
