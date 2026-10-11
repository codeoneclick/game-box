# Game Box: аудит производительности core

Дата: 10 октября 2026. Проверена текущая рабочая копия с островом, океаном, SSAO и 200 Woodsman-персонажами. Незакоммиченные изменения пользователя сохранены. Это анализ и измерения для последующей оптимизации; постоянные изменения поведения движка не вносились.

## Главные результаты

1. **Главное измеренное ограничение текущей Release-сборки — принудительный Vulkan validation.** При одинаковой временной инструментации: **14,40 → 4,71 мс/кадр**, примерно **69 → 212 FPS**. GPU в обоих случаях выполняет кадр примерно за 4,6–4,7 мс. Значительная часть дополнительных CPU-затрат находится в подготовке descriptors: **7,08 → 0,30 мс/кадр**. Validation запрашивается независимо от `NDEBUG` в [graphics_context_win32.cpp](../../gbCore/sources/graphics_context_win32.cpp#L122). Рекомендуется явная политика development/shipping и возможность включать проверку для диагностических запусков.
2. **FPS HUD создаёт измеренные периодические задержки.** Изменение текста загружает VBO и IBO через отдельные синхронные submit/fence wait. В 30-секундном контрольном интервале: 62 обновления после прогрева, 124 upload, в среднем **8,36 мс на пару**. Следующий frame-to-frame интервал в среднем **13,44 мс**, против **4,67 мс** у интервалов без upload. При замороженном HUD эти upload исчезли; p99 снизился **10,72 → 8,05 мс**. Это причина части длинных кадров, а не всех задержек.
3. **После отключения validation сцена ограничена преимущественно GPU.** Из 4,66 мс GPU-кадра: поле взаимодействия воды **1,91 мс**, океан вместе с отдельным foam-проходом **1,45 мс**, SSAO + blur **0,86 мс**, opaque base **0,36 мс**. Первые GPU-эксперименты стоит направить на разрешение и организацию этих проходов.
4. **Анимация уже использует GPU skinning.** На CPU остаются вычисление поз и небольшое число контактов ног. В длинном Release-профиле CPU animation — **0,65 мс для 200 персонажей**, логика движения — **0,38 мс**, grounding/подготовка uniforms — **0,24 мс**. Перенос всей анимации в compute сейчас уступает по приоритету validation, upload и водным проходам.
5. **В кадре 258 draw calls и около 883 416 отправленных треугольников.** Без жителей — 58 draws, с 50 — 108: каждый житель добавляет отдельный draw. Эти числа включают все проходы и повторный рендер геометрии; это не число уникальных полигонов сцены.

Составлен каталог **53 конкретных кандидатов/ограничений** ниже. Измеренные причины, статические затраты, проблемы жизненного цикла и неподтверждённые backend-кандидаты разделены. Для статических пунктов ускорение в миллисекундах неизвестно.

## Условия и методика

| Параметр | Значение |
|---|---|
| CPU | Intel Core i3-12100F |
| GPU / драйвер | RTX 3060 Ti, 8 GiB / 591.86 |
| Основной backend | Windows Vulkan; приложение запрашивает Vulkan 1.1 |
| Сборка | VS/MSBuild, `release-client|x64`, `Optimization=MaxSpeed`, `NDEBUG`; сравнение с `debug-client|x64` |
| Сцена | Основной остров, штатная начальная камера, SSAO и океан включены, автономное движение |
| Размер | По умолчанию 1280×720; отдельные проверки 640×360 и 1920×1080 |
| Прогрев | 60 кадров; системные блоки по 120 кадров |
| Короткие сравнения | Не менее пяти блоков; это предварительные средние, не продолжительный stress test |
| Длинные сравнения | 30 секунд после первого отчёта; около 6500–6600 сырых кадров после прогрева |
| GPU timer | `timestampPeriod=1 ns`, `timestampValidBits=64`, три swapchain image |

CPU измерялся monotonic clock вокруг ECS phases и отдельных render/upload участков. Эти интервалы включают ожидания и вытеснение потока ОС, поэтому `feeder_ms` и время render-system нельзя считать чистым временем исполнения CPU. Дополнительно собраны process/thread CPU consumption и private/working-set memory. Один эквивалент занятого логического ядра означает CPU seconds / elapsed seconds, а не 100% всего процессора.

GPU timestamps поставлены на границах проходов в существующем command buffer. Результаты читаются после штатного image fence, без дополнительного wait, submit или barrier. Интервал показывает время выполнения очереди между отметками; он не измеряет отдельно ALU, bandwidth или аппаратную occupancy и не включает presentation. Bottom-of-pipe отметки и инструментация имеют накладные расходы.

Обычная Release-сборка с validation: 13,37 мс; инструментированная с ним: 14,40 мс. Поэтому эффект validation сравнивается **между одинаково инструментированными запусками**, а не приписывается разнице Debug/Release. Сырые percentile вычислены из отдельных кадров, не из средних 120-кадровых блоков.

Штатный запуск загружал `VkLayer_khronos_validation.dll` и `RTSSVkLayer64.dll`. Для длинных контрольных запусков подавлен явный запрос validation и применён process-local `VK_LOADER_LAYERS_DISABLE=~implicit~`; отсутствие этих DLL проверено в `modules.json`. Отключение implicit layers не дало сопоставимого с validation эффекта: короткий запуск без validation с RTSS — 4,71 мс, длинный без обоих — 4,76 мс. Это не доказывает нулевые затраты любого overlay в других настройках. Механизм фильтрации описан в [Vulkan Loader documentation](https://vulkan.lunarg.com/doc/view/latest/windows/LoaderLayerInterface.html).

Камера и функции сцены одинаковы, но фаза волн, состояние idle/walk, частоты CPU/GPU и нагрузка ОС не фиксировались аппаратно. Результаты — профиль этой машины и рабочей копии, а не обещание универсального ускорения.

## Динамические измерения

### Масштабирование жителей и влияние validation

Среднее frame-to-frame время из системных блоков:

| Сборка / режим | Жители | мс/кадр | FPS, 1000/мс | Комментарий |
|---|---:|---:|---:|---|
| Debug, штатный validation | 200 | 42,41 | 23,6 | Анимация около 9,2 мс; resident phases около 9 мс |
| Release, штатный validation | 0 | 4,65 | 215,3 | Обычный executable |
| Release, штатный validation | 50 | 5,66 | 176,7 | Обычный executable |
| Release, штатный validation | 200 | 13,37 | 74,8 | Обычный executable |
| Release + probe, validation | 200 | 14,40 | 69,4 | Контроль инструментации |
| Release + probe, без validation | 0 | 4,51 | 221,6 | RTSS ещё загружен |
| Release + probe, без validation | 50 | 4,55 | 219,8 | RTSS ещё загружен |
| Release + probe, без validation | 200 | 4,71 | 212,3 | RTSS ещё загружен |
| Release + probe, без validation/implicit layers | 200 | 4,76 | 210,2 | Длинный запуск, штатный HUD |
| То же, HUD обновлён один раз | 200 | 4,66 | 214,8 | Длинный контроль upload |

Сейчас разница между 0 и 200 жителями после отключения validation небольшая по общему времени кадра. CPU-затраты продолжают расти, но перекрываются GPU-работой. Это не означает, что дальнейшее увеличение населения бесплатно: текущий harness ограничен 200 и shader uniform arena имеет конечную ёмкость.

### GPU по проходам

Длинный запуск `core-audit-clean-long-200`, средние GPU интервалы:

| Участок | мс | Доля GPU-кадра |
|---|---:|---:|
| Water mask + seed + 12 JFA + distance | 1,908 | 41,0% |
| Из них только JFA | 1,782 | 38,3%, включено в строку выше |
| Ocean | 1,123 | 24,1% |
| Ocean foam | 0,325 | 7,0% |
| SSAO | 0,503 | 10,8% |
| SSAO blur | 0,360 | 7,7% |
| Opaque base, включая жителей | 0,361 | 7,8% |
| UI + composition + output | около 0,062 | 1,3% |
| Весь command-buffer интервал | **4,657** | 100% |

Строки с группами и их подстроками не складываются повторно. Малый остаток — интервалы между отметками. Общая стоимость ocean не устанавливает отдельно цену SSR, noise или wave deformation: для этого нужны дополнительные shader ablations либо аппаратные GPU counters.

### CPU и ожидания

Длинный запуск без validation/implicit layers, обычный HUD:

| Участок | Среднее, мс/кадр | Интерпретация |
|---|---:|---|
| Core animation/feed | 0,648 | Вычисление pose/palette для всех жителей |
| Resident/start | 0,382 | Движение, маршруты, transforms; application consumer core |
| Resident/feed | 0,241 | Grounding, палитры и uniforms |
| Ocean/feed | 0,114 | CPU-параметры и sparse buoyancy |
| Render/start | 1,431 | В основном штатный fence wait |
| Из него frame-fence wait | 1,379 | Следствие GPU/pipeline scheduling; не «лишний CPU расчёт» |
| Render/end | 1,821 | CPU recording/submit/present, возможные ожидания |
| Descriptor construction | 0,299 | Вложено в проходы/render-end |
| Opaque base recording | 0,855 | Включает свои descriptors |
| Queue present | 0,151 | Вложено в render-end |
| Upload flush, амортизировано | 0,080 | Редкие вызовы внутри actions, а не ровные 0,08 мс каждый кадр |

Вложенные scopes нельзя суммировать как независимые расходы. Process CPU consumption около **0,68 занятого логического ядра** без validation против **0,98** в инструментированном validation-контроле. Потоки ресурсов/jobs не показали заметного устойчивого CPU потребления в этой idle/steady сцене; их busy-wait кандидаты относятся прежде всего к периодам загрузки и зависимым jobs.

### Разрешение и память

200 жителей, одинаковая probe-сборка без validation:

| Размер | Пикселей относительно 720p | Средний кадр | Private bytes процесса |
|---|---:|---:|---:|
| 640×360 | 0,25× | 2,97 мс | около 457 MiB |
| 1280×720 | 1× | 4,71 мс | около 669 MiB |
| 1920×1080 | 2,25× | 9,96 мс | около 1021 MiB |

При 360p заметнее ограничение CPU; при 1080p — GPU. Изменено разрешение всей сцены, поэтому эти измерения не доказывают такое же ускорение только от half-resolution SSAO или field.

Private bytes — показатель CPU virtual-memory accounting процесса, **не замер VRAM**. Он не устанавливает, какие конкретно allocation отвечают за рост. Статически seed + 12 JFA + distance имеют 14 RGBA32F color targets: при 1280×720 это **196,875 MiB номинальных color texels**, без depth, allocator padding и остальных targets. Их можно потенциально заменить ping-pong storage, сохранив нужные потребителям результаты. Реальный peak/resident GPU-memory budget и allocation high-water пока UNKNOWN.

### Длинные кадры и пределы атрибуции

| 30-секундный контроль | Кадры | p50 | p95 | p99 | Максимум |
|---|---:|---:|---:|---:|---:|
| HUD обновляется | 6502 | 4,57 | 6,18 | 10,72 | 48,66 мс |
| HUD заморожен | 6655 | 4,46 | 6,31 | 8,05 | 39,68 мс |
| Повтор HUD freeze + phase spike logging | 6655 | 4,47 | 6,14 | 7,44 | 21,71 мс |

GPU p99 в первом запуске — 5,70 мс, максимум 6,25 мс: длинные wall-frame интервалы не объясняются только GPU-исполнением кадра. UI upload прямо подтверждён событиями и контрольным отключением. Разброс оставшихся максимальных задержек между повторами значительный. Phase logging отмечает отдельные spikes в render-end, resident/start и resident/feed, но elapsed timer включает вытеснение ОС. Он **не доказывает**, что все эти spikes вызваны BFS или grounding. Для остаточных hitch нужны CPU sampling/context-switch trace, driver/API scopes и per-frame navigation counters. Эта атрибуция остаётся UNKNOWN.

## Каталог кандидатов

Обозначения: **M** — динамически измерен соответствующий участок/эффект; **S** — затрата подтверждена кодом, индивидуальный вклад не измерен; **L** — lifecycle/correctness проблема; **U** — backend или workload динамически не проверен. M для группы не означает, что измерена цена каждой внутренней инструкции.

Подробные call paths и точные source locations: [рендер](core-audit-render.md), [ECS/анимация/навигация](core-audit-ecs.md), [ресурсы и scheduling](core-audit-resources.md). Номера строк относятся к исходникам после удаления временной probe.

### Рендер — 21 пункт

| ID | Кандидат | Доказательство / приоритетное действие |
|---|---|---|
| R01 | Принудительный validation в Release — M | `graphics_context_win32.cpp:122`; главный текущий CPU bottleneck. Отделить development/shipping policy, оставить диагностическое включение. |
| R02 | Новые descriptors и девять descriptor-update calls на draw — M/S | `shader.cpp:1112–1161`; 258 sets и около 2322 `vkUpdateDescriptorSets` calls/frame по этому пути. Cache limits, объединить writes; затем reusable frame/material sets и dynamic offsets. [Официальный Vulkan пример descriptor caching](https://docs.vulkan.org/samples/latest/samples/performance/descriptor_management/README.html). |
| R03 | Полный render-query обход для каждого WS pass — S | `ces_render_system.cpp:181,553–570`; 19 повторных обходов. Technique membership lists, visibility cache, packet collection. |
| R04 | Pipeline создаётся на экземпляр material без VkPipelineCache — S | `material.cpp:1359,1685`; прежде всего startup/first-visible. Общий immutable pipeline cache по полному state key. |
| R05 | Отдельный draw каждого одинакового жителя — M/S | `mesh_3d.cpp:323`; 58/108/258 draws при 0/50/200. Instancing с transform/palette index; существующее CPU batching не подходит для GPU-skinned crowd. |
| R06 | Inverse-transpose normal matrix вычисляется на draw — S | `ces_render_system.cpp:487`; cache по версии transform. |
| R07 | Копии technique/vertex-buffer/Metal-uniform containers — S | `ces_render_system.cpp:142,523,542,553`; immutable references при безопасном ownership. |
| R08 | Поле воды каждый кадр: 12 JFA + вспомогательные passes — M | 1,91 мс GPU; `transition.island_demo.xml:6`, `shader_water_jfa0.frag:2`. Разрешение, dirty/static+dynamic masks, допустимая частота обновления и dynamic step count. Движущиеся buoyant props требуют обновления поля. |
| R09 | WS/SS размеры принудительно равны окну — S/M | `game_transition.cpp:87,114`; изменение XML размеров сейчас не даст half-resolution. Сначала явная render-resolution policy. |
| R10 | Отдельный RGBA32F target на каждый JFA шаг — S | `vk_render_pass_descriptor.cpp:388`, water configs; 196,875 MiB nominal seed/JFA/distance. Ping-pong/aliasing после анализа last consumers. |
| R11 | Depth и store для fullscreen passes — S | `vk_render_pass_descriptor.cpp:131,155,164,302`; убрать ненужную depth attachment, определить load/store по actual use. Shared pooled depth не означает отдельную depth texture на каждый pass. |
| R12 | SSAO 8 направлений × 4 кольца в полном размере — M/S | 0,503 мс GPU; `shader_ss_ssao.frag:23`. Half resolution + depth-aware upscale, quality tiers, заранее рассчитанные направления. |
| R13 | SSAO bilateral blur 25 taps — M/S | 0,360 мс GPU; `shader_ss_ssao_blur.frag:13`. Оптимизированный/separable bilateral filter с проверкой contact shadows и тонкой геометрии. |
| R14 | Ocean SSR до 32 шагов + refinement — S, pass M | `shader_ocean.frag:99,106`; отдельная цена SSR UNKNOWN. Quality/distance tiers и controlled shader ablation. |
| R15 | Повторная procedural foam/noise работа в ocean и foam — S, passes M | `shader_ocean.frag:76`, `shader_ocean_foam.frag:76`. Общий field/texture только после ALU-versus-bandwidth проверки. |
| R16 | Синхронные VBO/IBO upload при изменении текста — M | `vbo.cpp:542`, `ibo.cpp:133`, `vk_utils.cpp:106–149`; staging arena + deferred submit + GPU retirement, per-frame glyph storage. Уменьшить HUD rebuild, сохранив актуальность UI. |
| R17 | Upload копирует allocation capacity вместо used bytes — S | `vk_utils.cpp:143`; короткая строка загружает capacity 256-symbol buffers. Явный used range, затем batching. |
| R18 | Фиксированные 2 MiB uniform arena и 1024-set pool на shader/image — S/L | `shader.cpp:1082,1098,1121`; palette 4096 bytes/жителя плюс другие blocks/alignment. Track high-water, безопасное расширение. Current uniforms уже persistently mapped. |
| R19 | Metal WS `waitUntilCompleted` каждый кадр — S/U | `mtl_device.cpp:15,116`; serial CPU/GPU. Для pipelining сначала per-frame uniform storage, затем ownership/semaphore audit. |
| R20 | Metal custom/MVP single-buffer reuse — L/U | `material.cpp:621,1710`, `mtl_buffer.cpp:66`; overwrite/multi-block hazard. Отдельные ring slices; нельзя просто убрать wait. |
| R21 | Legacy GL uniform-array и buffer-data uploads — S/U | `shader.cpp:747`, `vbo.cpp:537`, `ibo.cpp:128`; частичная state caching уже есть. Проверить в реальном GL build перед redesign. |

### ECS, анимация, сцена и gameplay consumers — 23 пункта

| ID | Кандидат | Доказательство / действие |
|---|---|---|
| E01 | Animation обновляет всех персонажей независимо от visibility/distance — S, system M | `ces_animation_3d_system.cpp:38,124,200`; update-rate tiers, advance time отдельно от pose, shared/quantized palettes. |
| E02 | Root-bone `std::set` возвращается по значению на каждый frame — S | `ces_skeleton_3d_component.cpp:133`, animation `:200`; const reference/immutable dense roots, убрать node allocations/shared_ptr traffic. |
| E03 | Additional-animation state и временные vectors копируются — S | animation `:71–102`, mixer `:678`; `const auto&`, reusable scratch. В текущем main-only idle/walk массив пуст, его heap cost не измерен. |
| E04 | Mask lookup линейный и вызывается трижды на bone/layer — S | animation `:178,183,188`, mask `:54,120`; заранее bone-indexed weights. Workload с дополнительными слоями не измерен. |
| E05 | Несколько pose/palette copies и fixed 64-matrix preparation — S | animation `:206`, `resident_shader_uniforms.cpp:11`; identity tail один раз, active bone range/shared ownership. Uniform setter хранит pointer: это не ещё одна 4 KiB memcpy. |
| E06 | Grounding и uniform/material lookup каждого жителя каждый кадр — S, system M | resident system `:61–84`; sparse contacts уже shared. Связать с pose LOD; не делать GPU readback ради нескольких ног. |
| E07 | Idle state всё равно устанавливает position/rotation — S | resident system `:36,38`, transformation `:26,34`; equality/dirty guards, сохранить Euler semantics. |
| E08 | Иерархия рекурсивно обновляется при каждом отдельном property setter — S | `game_object_3d.cpp:31,58,76`, transformation extension `:55`; one dirty traversal, borrowed child containers. Это не текущий flat-resident movement path. |
| E09 | ECS change/removal линейно сканирует query vectors для каждого события — S | `ces_systems_feeder.cpp:56–99`; coalesce entity changes, membership index, bulk rebuild. Сохранить next-tick/phase contract. |
| E10 | >2048 query references вызывает повторные cleanup/logging — S | feeder `:67–75`; live/expired accounting и rate limit, а не скан на каждом change. При нынешних 200 жителях порог не установлен как hotspot. |
| E11 | Weak locks/shared_ptr/component footprint в hot queries — S | base system `:67–77`, entity.h `:23,114`; lifetime-safe typed iteration/dense storage после CPU cache/allocation profiling. 255 shared_ptr slots — около 4 KiB на x64 entity, lookup уже O(1). |
| E12 | `new[]` matrix arrays не освобождаются в skeleton/mixer — L | skeleton `:23,33`, mixer `:126,135`; RAII и повторный setup. Две матрицы по 64 bytes × B × N на lifecycle; для N=200/B=19 около 475 KiB, не доказанный steady-frame bottleneck. |
| E13 | Rig/bind setup, inverse matrices и flushed bone logs на каждого жителя — S | skeleton `:35–45`, animation `:257,262`, bone `:171`; immutable rig shared, mutable pose отдельно, logs opt-in. Startup. |
| E14 | Retarget cache lookup поздний; повторная clip validation — S | `scene_fabricator.cpp:156–184`, mixer `:521`; mesh-level rig signature и early cache hit, validate immutable clip один раз. Полный retarget уже кешируется. |
| E15 | Reachability BFS/vector дублируется для каждого жителя — S | behavior `:33`, navigation `:61–63`; connected components/reachable cells общие для terrace/component. Startup и память. |
| E16 | BFS allocations и до 16 path attempts на main thread — S | navigation `:23–36`, behavior `:43–46`; scratch generation stamps, A*, per-frame request budget/worker snapshots. Отдельная hitch-цена UNKNOWN. |
| E17 | 16 cos/sin footprint probes при каждой safety check — S | navigation `:17–21,56–59`; precomputed offsets, certified safe interior cells, точный contour fallback. Spatial triangle index уже существует. |
| E18 | Touch hit-test рекурсивно проходит всю сцену и копирует children — S | touch system `:51,195–205`; touchable query/broad phase, coalesce drag с сохранением semantics. Обычный `source_none` move отбрасывается. |
| E19 | Ray unprojection повторяется по candidates, callback lists копируются — S | touch `:71,106,148,179,286`; event-level ray cache, mutation-safe dispatch. |
| E20 | Box2D синхронизирует sleeping bodies и subtree transforms — S/U | Box2D system `:209–238`; awake/changed-state sync, один transform flush. Island не проверяет racing physics workload. |
| E21 | Fixed Box2D step привязан к render frame, игнорирует dt — L/U | Box2D system `:45–55`, world `:22`; correctness-first fixed accumulator, bounded catch-up. World body сам делает GetBodyCount()>0. |
| E22 | Contact ignore-list copies и линейный weak-pointer поиск — S/U | Box2D world `:68,90`; borrowed snapshot/filter или ID set при больших списках. |
| E23 | dt квантуется до 1 мс, старый FPS использует `clock()` — L/S | `game_loop.cpp:41–65`; fractional monotonic dt и единая frame metric с тестами simulation semantics. Это ограничение измерений/движения, не установленный большой расход CPU. |

### Ресурсы, jobs и startup — 9 пунктов

| ID | Кандидат | Доказательство / действие |
|---|---|---|
| Q01 | Workers spin, когда работа есть только в main queue — S/U | `thread_operations_pool.cpp:74–123`; ready queues/condition variables. Current steady scene этого заметно не показала. Проверить cross-affinity dependencies и shutdown. |
| Q02 | Resource worker polling, parse под mutex, unsynchronized map/status — S/L | `resource_accessor.cpp:35–74`, `.hpp:30`; take work под коротким lock, parse вне lock, synchronized completion queue. GPU commit/callback остаются main-thread. |
| Q03 | PNG/Vulkan texture uploads синхронно ждут fence — S | PNG committer/vk_utils, подробности resource appendix; batch staging/copy, safe completion/retirement. Cold/streaming workload, не цена каждого steady frame. |
| Q04 | Runtime shaderc compile + повторные regex/reflection conversions — S | shader compiler + `shader.cpp:586–587`; cooked SPIR-V, versioned compile/reflection cache. Startup. |
| Q05 | Runtime FBX parse/sampling/packing и временные allocations — S | FBX serializers/committer, resource appendix; cooked mesh/clip formats, reusable import representation. Это работа CPU/offline toolchain. |
| Q06 | Strong resource/config/font caches и listener lifetime без accounting/eviction — S/L | resource accessor `:24`, controller `:62–68`, generated config accessor; residency budgets, explicit unregister, safe GPU retirement. Repeated transition memory growth не измерен. |
| Q07 | Nested material configurations обходят общий top-level cache — S | generated shape config `:76–78`, material config `:386–447`; исправлять generator/schema, а не hand-edit generated outputs. Startup уникальных shapes. |
| Q08 | PNG row-pointer array выделяет в 8 раз больше pointers на x64 — S | PNG serializer `:108,123`; низкий приоритет, исправить element count. Pixel data освобождается после commit: постоянная CPU-копия PNG не установлена. |
| Q09 | Screen quad создаётся заново при render-target enumeration — S/U | render system `:586` → mesh constructor → upload; shared immutable quad. Наличие активных render-target entities в текущей island-сцене не установлено. |

## Что переносить на GPU, а что оставлять CPU

| Работа | Предлагаемое размещение | Обоснование и условия |
|---|---|---|
| Vertex skinning | GPU, уже реализовано | Resident vertex shader выполняет weighted bone transforms. Отдельный compute skinning полезен только при достаточном повторном использовании деформированных vertices; сейчас resident material только в base. |
| Pose evaluation | CPU сейчас; GPU/baked crowd как поздний эксперимент | Измерено 0,65 мс/200. Сначала roots-by-reference, pose tiers, общие/квантованные palettes и instancing. GPU-перенос должен сохранить blends, rig orientation и CPU foot grounding без полного readback. |
| Water distance field, SSR, SSAO, blur, foam | GPU | Уже GPU; наиболее полезны pixels/passes/storage оптимизации. Переписывание fragment JFA на compute само по себе не гарантирует ускорения. |
| Sparse buoyancy/foot contacts | CPU | Gameplay нужны результаты сейчас, probes немного. GPU readback добавит latency/synchronization; geometry deformation остаётся GPU. Поддерживать совпадение wave functions/parameters. |
| Navigation/pathfinding | CPU, immutable worker snapshots при доказанных spikes | Статические terrace grids, небольшие независимые запросы; shared components и budget проще GPU BFS/readback. |
| ECS membership, input/picking, Box2D | CPU | Pointer ownership, callbacks и authoritative gameplay. Сначала структура данных и timing contracts. |
| Descriptors/render packets | CPU preparation, GPU-friendly indexed buffers | Уменьшить API calls и copies, не переносить Vulkan driver calls в compute. Instancing/indirect culling рассматривать после safe per-frame storage. |
| FBX/XML/PNG import, retarget setup | Offline/CPU workers | GPU плохо подходит для parsing и callbacks. Cooked assets, cache, короткие locks; GPU commit остаётся на штатном lifecycle пути. |
| Dynamic glyph uploads | CPU генерация + asynchronous GPU transfer | Здесь проблема синхронизации и reuse, а не необходимость GPU генерации текста. |

## Порядок будущих изменений

1. **Условия измерений и shipping policy:** Release + управляемый validation; фиксированная сцена/камера и repeatable captures. Затем async per-frame glyph upload. Проверять mean и tail одновременно, не только FPS.
2. **Основной GPU budget:** независимое разрешение поля и SSAO; JFA ping-pong/step policy; AO blur; затем SSR/foam ablations. Сравнивать shore contact, gap, moving props, refraction, character contacts и разные zoom/viewport. Это может менять качество, поэтому без визуальной проверки внедрять нельзя.
3. **Малые CPU/lifecycle исправления:** roots getter, precomputed navigation offsets, equality guards, device limits/write batching, matrix-array RAII, worker synchronization. Сначала исправить ownership/races, затем параллелить.
4. **Масштабирование населения:** shared reachability/path scratch + budget, immutable rig/cache, animation tiers, shared pipelines, resident instancing. До >200 проверить uniform arena/pool high-water и backend layouts.
5. **Большие изменения core:** dense ECS queries, deferred scene transforms, asynchronous transfers/command recording, GPU animation/culling — отдельные эксперименты после предыдущих замеров. Для них нет измеренного обещания FPS в текущей сцене.

## Артефакты и воспроизведение

- [measurements.json](measurements.json): компактные результаты всех запусков, CPU/GPU scopes, frame percentiles, memory и HEAD. Это снимок dirty working tree, не benchmark чистого HEAD.
- [core-audit-render.md](core-audit-render.md), [core-audit-ecs.md](core-audit-ecs.md), [core-audit-resources.md](core-audit-resources.md): независимые статические приложения с call paths и ограничениями.
- [Инструкция к probes](../../gbScripts/performance/README.md), [benchmark harness](../../gbScripts/check-island-performance.ps1), [temporary installer](../../gbScripts/performance/core-probe.py), [GPU/CPU probe](../../gbScripts/performance/core-probe.h), [parser](../../gbScripts/performance/summarize-core-profile.py).
- Полные локальные logs: `gbDemo/output/island-check/perf-core-audit-*/runtime.log`, `errors.log`, `summary.json`, `profile-details.json`, `thread-cpu.json`, `modules.json`. Они игнорируются Git; компактный JSON выше сохранён в документации.
- Build logs: `core-audit-release-build.log`, `core-audit-probe-build.log`, `core-audit-probe-v2-build.log`, `core-audit-probe-v3-build.log`, финальный `core-audit-restored-build.log` в том же output/island-check.

Основные команды:

```powershell
./gbScripts/build-gbDemo.ps1 -Configuration release-client -Platform x64
./gbScripts/check-island-performance.ps1 -Configuration release -Residents 200 -Samples 5 -Label baseline
python gbScripts/performance/core-probe.py install
./gbScripts/build-gbDemo.ps1 -Configuration release-client -Platform x64
$env:GB_PROFILE_CORE='1'
$env:GB_PROBE_VALIDATION_OFF='1'
$env:VK_LOADER_LAYERS_DISABLE='~implicit~'
./gbScripts/check-island-performance.ps1 -Configuration release -Residents 200 -Samples 5 -SteadySeconds 30 -Label clean
python gbScripts/performance/core-probe.py remove
./gbScripts/build-gbDemo.ps1 -Configuration release-client -Platform x64
```

Перед воспроизведением сохранить собственные значения этих environment variables, после — восстановить. Installer делает byte-exact snapshots и отказывается удалять probe поверх последующих изменений исходников. Во время capture не запускать одновременно build/другой benchmark.

API timestamp support, valid bits и результаты query проверены по [официальной Vulkan спецификации](https://docs.vulkan.org/spec/latest/chapters/queries.html). Render timers сохраняют существующие fence/submit/phase contracts. Это focused profiling/compile/runtime checks, а не engine-owned unit tests.

## Проверки и незакрытые области

Доступный Windows Vulkan Release target собран; все перечисленные captures завершились штатно. Инструментация временная, восемь исходных файлов восстановлены byte-exact с проверкой SHA-256, обычный executable пересобран после её удаления. Debug executable использован как исходная контрольная сборка. `errors.log` этих captures пусты; полный отдельный аудит Vulkan teardown/lifetime этим не заменяется.

Финальный запуск восстановленного обычного Release: 200 жителей, три 120-кадровых блока, 13,70 мс/кадр; validation снова загружен, `PROBE_*` событий нет. `git diff --check` и отдельная проверка новых audit-файлов пройдены. Проверены PowerShell syntax, JSON и восстановление SHA-256; это не unit-test suite.

**UNKNOWN / не проверено динамически:** OpenGL, Metal и Apple targets; gbServer; racing Box2D/contact workload; network/audio/database нагрузка; resource streaming/cancellation; repeated scene transitions и cache eviction; >200 residents; allocation stack trace/CPU cache misses; peak VRAM; GPU ALU-versus-bandwidth counters; точная причина всех остаточных hitch. Windows Vulkan замеры не подтверждают работоспособность этих backends и workloads.

Нельзя использовать этот аудит как основание убрать синхронизацию Metal/Vulkan, перенести GPU commit на resource worker либо изменить ECS phase order без отдельного correctness audit.
