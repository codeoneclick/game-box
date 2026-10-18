# Terrain Continuous LOD Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans for native execution, or superpowers:subagent-driven-development only if the user explicitly chooses delegated execution. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Уменьшить геометрию террас, добавить управляемый Continuous LOD и независимую грубую геометрию для shadowmap без нарушения редактора и берега.

**Architecture:** Логические данные и исходные CPU-чанки остаются источником истины. Worker подготавливает точную оптимизацию, варианты контуров и пространственную иерархию; игровой поток публикует согласованные GPU-поколения. Основное изображение, shadowmap и маска берега выбирают свои представления независимо.

**Tech Stack:** C++14/MSVC, совместимый с Apple C++11 прикладной код; gbUI; JsonCpp 1.7.2; GLSL/Vulkan и соответствующий Metal.

**Spec:** [Утверждённый проект](../specs/2026-10-16-terrain-continuous-lod-design.md). Перед исполнением прочитать также [аудит](../../terrain-geometry-audit.md).

Status: Implemented. Verification and limitations: docs/terrain-geometry-audit.md.

## Global Constraints

- Контур точен при P=0 и у ближайшего LOD при любом P. Высоты всегда дискретны.
- N=1–8, default 5; P=0–50%, default 0%, шаг UI 1 процентный пункт.
- N включает ближайший уровень; `P_i=P*i/(N-1)` для N>1; единственный уровень имеет 0%.
- При N=5, P=50: 0 / 12,5 / 25 / 37,5 / 50%. Удаления не накапливаются последовательно.
- Не изменять исходные поверхности генерации травы, building support, навигацию, undo/redo и сериализацию уровня.
- Не редактировать generated/vendored/staged assets. Новые C++ файлы включать в vcxproj и применимые Xcode targets.
- Не менять renderer core, backend presentation или синхронизацию ради прикладного LOD.
- CPU-геометрия строится worker-ом; GPU-ресурсы создаются в игровом потоке.
- Индексы 16-битные; не более 60 000 вершин в GPU-пакете.
- Мировой размер и resolution берутся из конфига. Число LOD независимо от глубины quadtree.
- Движение камеры не вызывает триангуляцию, навигацию или повторную загрузку готовых VBO.
- Для маски берега P всегда 0; для shadowmap — самый грубый допустимый вариант в проекции света.
- Не обещать FPS на основании числа полигонов. Windows Vulkan не подтверждает OpenGL/Metal.

## Review Focus

1. Контуры с отверстиями, узкими перешейками и почти коллинеарными точками не должны менять топологию — задачи 1 и 3.
2. Старый GraphicsSettings, неверные JSON-типы и дробное N не должны частично менять настройки — задача 2.
3. Загрузка уровня во время rebuild и быстрые смены N/P не должны публиковать устаревший родительский узел — задачи 4 и 6.
4. Объекты за камерой должны продолжать отбрасывать видимые тени; поворот камеры не должен менять пену — задача 6.
5. Маленький viewport, N=1 и перетаскивание за пределами трека не должны перекрывать UI или запускать sculpt — задача 7.

## Проверенные точки интеграции

- `terrain_editor_render_cache.cpp::initialize/rebuild/merge`: clipping, группы 4 × 4 чанка и стыки. `terrain_parts()` остаётся исходным CPU-представлением.
- `island_demo_scene.cpp::prepare_editor_mesh`, `editor_rebuild_jobs`, `publish`: упаковка PTNTC, async worker и GPU upload.
- `shape_3d::setup_components()` уже включает AABB-culling; не добавлять его второй раз.
- `ces_sun_shadow_system::on_feed_end`: сейчас два прокси получают исходный меш `ws.base`.
- `ces_ocean_system::add_interaction/on_feed`: автоматически копируются меш и видимость source.
- `ocean_shader_uniforms::values`: текущий размер 624 байта, `sky_rotation` в конце; Metal `ocean_uniforms` повторяет порядок.
- GLSL custom-uniform offsets определяются порядком деклараций в `shader_compiler_glsl.cpp`. Не путать индекс ECS-буфера с sampler binding.
- `terrain_editor_ui.cpp`: `daytime` 250 × 152 после Load; две строки Time/Skybox. Пересчитать ширину toolbar вместе с hit-test и Saved/Modified.

Все новые API ниже — **предлагаемые прикладные API**, а не существующие функции движка.

### Task 1: Точное упрощение Terrain-пакетов

**Files:** создать `gbDemo/sources/terrain_mesh_optimizer.h/.cpp`, `gbScripts/check_terrain_mesh_optimizer.cpp`, `gbScripts/check-terrain-mesh-optimizer.ps1`; изменить `terrain_editor_render_cache.cpp`, `gbDemo.vcxproj`, `gbDemo.xcodeproj/project.pbxproj`, `profile_terrain_geometry.cpp`.

**Interfaces:**
- `TerrainOptimizeConstraints`: `std::vector<uint16_t> locked_vertices`; `size_t vertex_limit=60000`.
- `TerrainOptimizeResult`: `std::vector<terrain_mesh_data> parts`; `size_t input_triangles, output_triangles, fallback_regions`.
- `TerrainOptimizeResult optimize_terrain_mesh(const terrain_mesh_data&, const TerrainOptimizeConstraints&)`.

- [ ] Написать CPU-проверки `flat_grid_keeps_boundary`, `holes_stay_holes`, `walls_and_ids_unchanged`, `gradient_region_falls_back`, `split_at_vertex_limit`. Для сетки 16 × 16: площадь 1024 м² при cell=2, выход ≤62 треугольника, все 64 исходных периметровых сегмента сохранены. Добавить несвязные области и почти коллинеарные точки.
- [ ] Создать runner по образцу `check-terrain-editor.ps1`, запустить `./gbScripts/check-terrain-mesh-optimizer.ps1`; подтвердить ожидаемый RED до реализации.
- [ ] Реализовать точное индексирование всех атрибутов без округления. Зафиксировать границы/стены/швы; для горизонтальных областей с постоянными атрибутами удалять только внутренние вершины через локальную повторную триангуляцию. Неоднозначный fan/невалидная область сохраняет исходную топологию.
- [ ] Проверять локальные изменения: ориентация, площадь, boundary segments, отсутствие новых пересечений. Исключить неиспользуемые вершины; разделить пакеты при 60 000 вершинах.
- [ ] Подключить оптимизатор только к публикации `EditorRenderBatch::Terrain` после merge. Grass/Trees и `terrain_parts()` не изменять. Добавить файлы во все применимые проекты.
- [ ] Запустить новый runner и `./gbScripts/check-terrain-editor.ps1`. Ожидается GREEN без изменения прежних grass roots/mesh checks.
- [ ] Расширить `./gbScripts/profile-terrain-geometry.ps1` фактическими оптимизированными counts. Сравнивать те же сценарии 55 126 и 621 302, отдельно от оценок аудита. Снять фиксированный baseline до включения дальнейших задач.

### Task 2: Настройки N/P и GraphicsSettings

**Files:** создать `gbDemo/sources/terrain_lod_settings.h`; изменить `graphics_settings.h/.cpp`, `gbResources/GraphicsSettings.json`, `gbScripts/check_graphics_settings.cpp`.

**Interfaces:**
- `TerrainLodSettings`: поля `int lodCount=5`, `float contourSimplificationPercent=0`, `uint64_t revision=0`.
- `bool set_lod_count(int)`, `bool set_contour_percent(float)`: безопасный clamp для UI, увеличение revision только при изменении.
- `float contour_percent(int level) const`: формула из spec, N=1 → 0.
- Новые overload `load_graphics_settings(path, post, lighting, TerrainLodSettings&, error)` и соответствующий `save_graphics_settings` с const settings. Существующие overload сохранить для прежних потребителей; scene перевести на новый.
- JSON: `terrainLOD.lodCount` и `terrainLOD.contourSimplificationPercent`; runtime revision не сериализуется.

- [ ] Добавить в CPU-проверки утверждённый пример `0,12.5,25,37.5,50`, N=1, N=8, P=0, defaults, round-trip, старый JSON, N=5.5, N=0/9, P=-1/51, неверные типы/NaN. Для неверного файла исходные post/lighting/LOD должны остаться целиком прежними.
- [ ] Выполнить `./gbScripts/check-graphics-settings.ps1`, подтвердить RED на новых полях/overload.
- [ ] Реализовать settings и JSON validation до применения. Не путать допустимый UI clamp с отклонением неверного файла. Старые файлы без раздела получают defaults.
- [ ] Повторить runner до GREEN; проверить, что Save F8 сохраняет также прежние renderPasses/lighting/postProcess.

### Task 3: Вложенные варианты контура и безопасный morphing

**Files:** создать `gbDemo/sources/terrain_contour_lod.h/.cpp`, `gbScripts/check_terrain_contour_lod.cpp`; расширить CPU-runner задачи 1 и проектные файлы.

**Interfaces:**
- `TerrainLodVariant`: `terrain_mesh_data mesh`; parallel-массивы `std::vector<std::array<float,2>> morph_delta_xz, morph_normal_xz`; `float requested_percent, actual_percent, world_error`; topology/contour signatures.
- `TerrainContourConstraints`: закреплённые boundary segments, footprint-ограничения зданий/лестниц/растительности из snapshot уровня.
- `std::vector<TerrainLodVariant> build_terrain_contour_lods(const terrain_mesh_data&, const TerrainLodSettings&, const TerrainContourConstraints&)`.

- [ ] Написать RED-проверки: при P=0 все contour signatures совпадают; LOD0 точен при P=50; контур с 320 свободными сегментами получает цели 320/280/240/200/160; ограничения могут дать меньшее фактическое сокращение и должны его сообщить.
- [ ] Проверить отверстие, узкий перешеек, две близкие террасы, крайние высоты, лестницу на кромке и здание возле уступа. Удаления не пересекают закреплённые сегменты или footprint.
- [ ] Реализовать одну детерминированную последовательность удалений от LOD0. Каждый уровень использует её префикс; не упрощать уже упрощённый предыдущий mesh по проценту повторно.
- [ ] Контур, верх/низ стены и прилегающие площадки перестраивать как единый участок. Высоты и outline ID сохранять; нормаль стены пересчитать отдельно от нормали площадки.
- [ ] Подготовить переход fine→coarse: удаляемая вершина движется в XZ, Y неизменен. Группировать только независимые локальные изменения; консервативно отклонять изменения, чья swept-область пересекает несоседние контуры/границы/опоры. Проверить ориентацию промежуточных треугольников на всём интервале аналитически, не только в нескольких кадрах.
- [ ] Уровень без экономии переиспользует уже подготовленный вариант. Измерить world_error и проверить actual_percent. CPU-runner должен дать GREEN.

### Task 4: Иерархия и выбор CLOD

**Files:** создать `gbDemo/sources/terrain_lod.h/.cpp`, `gbScripts/check_terrain_lod.cpp`, `gbScripts/check-terrain-lod.ps1`; изменить `terrain_editor_render_cache.h/.cpp` и проекты.

**Interfaces:**
- `TerrainLodVersion`: `uint64_t load_generation, terrain_revision, settings_revision`.
- `TerrainLodNode`: id/parent/4 children, AABB, footprint, versions, варианты задачи 3 и coarse-cost summary.
- `TerrainLodGeneration`: nodes и неизменяемые CPU-пакеты; source chunks остаются отдельно.
- `TerrainLodView`: `std::array<float,16> view_projection`; camera XYZ, viewport height, projection scale.
- `TerrainLodSelection`: выбранные node/variant ids и morph factors, tested/culled counts.
- `TerrainLodSelection select_terrain_lod(const TerrainLodGeneration&, const TerrainLodView&, const TerrainLodSelection* previous)`.

- [ ] Написать RED-проверки: неполный размер мира, пустые листья, ровная площадка вблизи, кольцевая терраса, разный LOD соседей и изменение viewport. На участок выбирается родитель или дети, без двойного покрытия; соседи сбалансированы до разницы в один пространственный уровень.
- [ ] Построить quadtree от имеющихся 16-cell чанков; родители точно объединяют источники до опционального contour LOD. Сохранять исходные точки внешнего периметра у каждого варианта, даже коллинеарные.
- [ ] Реализовать frustum selection и `screen_error=world_error*projection_scale/distance_to_AABB`; при P>0 начальный предел 1 px, ближайшая зона LOD0. Для zero-error выбирать более экономичную топологию без искусственного увеличения ближней сетки.
- [ ] Привести соседей к совместимым индексам/коэффициентам morphing; использовать одну world-distance функцию. Добавить 10% гистерезис стоимости переключения, не ослабляя предел ошибки. Подготовленные меши не пересоздавать при выборе.
- [ ] Проверить invalidation: dirty leaves+стыкуемые соседи+их предки; неизменённые ветви переиспользуются. Смена N/P имеет собственную revision и не вызывает navigation. Старый результат после load или нового значения ползунка отвергается.
- [ ] Запустить `./gbScripts/check-terrain-lod.ps1`; подтвердить GREEN и нулевые rebuild/upload counters при повторном выборе неподвижной камеры.

### Task 5: GPU-представление и непрерывный переход

**Files:** создать `gbDemo/sources/terrain_lod_renderer.h/.cpp`; изменить `island_demo_scene.cpp`, `ocean_shader_uniforms.h/.cpp`, `shader_island_terrain.vert`, `metal/mtl_library.metal`, `metal/mtl_ocean.h`, проектные файлы.

**Interfaces:**
- `TerrainLodRenderer(fabricator, ocean_system)` — прикладной владелец shape и готовых GPU-вариантов.
- `void stage(std::shared_ptr<const TerrainLodGeneration>)`, `bool publish_ready(const TerrainLodVersion&)`, `void update_main(const TerrainLodView&)`.
- `ocean_shader_uniforms::set_terrain_lod(float morph, bool enabled)` — новое дополнение существующего uniform holder.

- [ ] Добавить проверку packing: исходные normal/outline bits сохранены, morph=0 не меняет координаты, morph=1 совпадает с coarse mesh с учётом измеренной точности half2. Невместимый/nonfinite delta вызывает fallback, не clamping формы.
- [ ] Сохранить PTNTC: для **LOD Terrain** texcoord хранит half2 delta XZ, tangent.xy — целевую горизонтальную нормаль стены. Tangent.z, normal.w и color.a остаются упаковкой outline; tangent.w остаётся Terrain sentinel. CPU `editor_palette` не удаляется. Fragment террас использует мировые координаты; grass/tree upload остаётся прежним.
- [ ] Добавить `terrain_lod` vec4 **в конец** ocean uniform values после sky_rotation: размер 624→640 байт; neutral zero по умолчанию. Existing vertex slot 2/fragment slot 0 сохраняются; новых descriptor bindings нет. В GLSL vertex объявить полный предшествующий layout (включая семь lighting vec4 и sky_rotation), чтобы offset нового поля соответствовал C++/Metal.
- [ ] Менять координаты только у enabled Terrain; XZ += delta*morph, Y без изменения. Для стен нормаль плавно нормализуется между исходной и целевой; у площадок остаётся up. В Metal повторить те же условия. Остальные потребители расширенного ocean buffer видят прежние offsets.
- [ ] GPU-пакеты загружать в игровом потоке заранее. Игровой update выставляет morph через существующие uniform holders; ocean update не затирает его. После завершения перехода перейти на coarse IBO, не отправляя исчезнувшие треугольники бесконечно.
- [ ] Существующие 12-update retirement и per-frame dynamic-buffer правила сохранить. Проверить, что пик подготовки ограничивается очередью upload; не создавать все дубли в один кадр.
- [ ] Выполнить build release-client и native smoke с Vulkan validation при P=0/P=50 и morph 0/0.5/1. Проверить отсутствие изменений травы/деревьев и ошибок uniform layout. Metal/OpenGL отметить отдельно, если запуск недоступен.

### Task 6: Независимые проходы и атомарная публикация

**Files:** изменить `terrain_lod_renderer.h/.cpp`, `ces_sun_shadow_system.h/.cpp`, `ces_ocean_system.h/.cpp`, `island_demo_scene.cpp`, `check_sun_shadows.cpp`, `check_terrain_lod.cpp`.

**Interfaces:**
- `TerrainShadowCaster`: stable id, `mesh_3d_shared_ptr mesh`, model matrix, bounds и version.
- `ces_sun_shadow_system::set_terrain_casters(const std::vector<TerrainShadowCaster>&)` — явный список, независимый от ws.base visibility.
- `TerrainLodRenderer::update_shadows(near_view, far_view)` — coarsest допустимый для каждой карты; ограничение отклонения 1 texel при P>0.
- Для water mask создать независимые stable source shapes с точным P=0 мешем; зарегистрировать через уже существующий `add_interaction`. У этих sources нет `ws.base`; их visibility не наследуется от main LOD selection. Дополнительный ocean API нужен только для явного снятия старых sources при публикации/удалении.

- [ ] Написать RED-проверки: caster вне камеры внутри light frustum, отсутствие двойного Terrain-caster, неподвижная маска при camera selection, N/P=1/0/50, низкое солнце.
- [ ] Для зарегистрированного Terrain исключить автоматический shadow proxy по main shape; skin/building/tree путь не менять. Для P=0 контур точен; при P>0 самый грубый, не проходящий light-space error, заменить более подробным.
- [ ] Создавать/обновлять mask sources в одном поколении с исходным берегом. Сохранять покрытие world interaction; скрытие main shape не скрывает маску. Проверить береговую пену на стыках LOD.
- [ ] Разделить stage/upload/publish: новые meshes невидимы до готовности минимального совместимого набора соседей и проходов. Сверять load/terrain/settings revisions непосредственно перед publish; остальные ветви сохраняются. Предыдущая согласованная версия живёт до переключения.
- [ ] Добавить проверку быстрых N/P и load во время rebuild: опубликованная revision не уменьшается; старые родители не возвращаются. Проверить ограниченный кэш дополнительных вариантов: soft budget 64 МиБ сверх pinned active/base, LRU для неактивных, fallback к подробному мешу без синхронной генерации.
- [ ] Выполнить `./gbScripts/check-terrain-lod.ps1`, `./gbScripts/check-sun-shadows.ps1`, native Vulkan smoke обоих shadowmap и water mask. Проверить выход приложения без ошибок lifetime/validation.

### Task 7: Слайдеры и сохранение

**Files:** изменить `terrain_editor_ui.h/.cpp`, `island_demo_scene.cpp`, `graphics_settings.h/.cpp`, `gbScripts/check_graphics_settings.cpp`, `docs/graphics-settings.md`.

**Interfaces:**
- `TerrainEditorUI::bind_terrain_lod_settings(std::shared_ptr<TerrainLodSettings>)`.
- UI вызывает setters задачи 2; scene наблюдает revision и ставит работу задач 3–6 в существующую worker-очередь. Прямого обращения UI к mesh builder нет.

- [ ] Проверить settings с N=1/5/8, P=0/50 и старым JSON через `./gbScripts/check-graphics-settings.ps1`.
- [ ] Расширить daytime-панель до двух колонок: Time/Terrain LODs и Skybox/Contour simplification. Сохранить прежние размеры треков и стиль; toolbar масштабировать с учётом новой ширины и области HUD. Переместить Saved/Modified из нового прямоугольника.
- [ ] Создать integer slider 1–8 и percent slider 0–50. При drag использовать существующий captured/clamped подход Time/Skybox. При N=1 заблокировать percent track, сохранив предпочтение P. UI update не вызывает setters повторно и не увеличивает revision.
- [ ] Загрузить TerrainLodSettings до создания scene/render cache. Перевести Save F8 на новый overload; Save уровня не изменять. Объединять drag events с задержкой 150 мс перед rebuild; отпускание ползунка применяет последнюю revision сразу.
- [ ] Native smoke на 1280×720 и меньшем поддерживаемом viewport: контролы в нужных строках, ввод не проваливается в sculpt/orbit, drag за край ограничен. N/P влияют на F8 counts/выбор геометрии после подготовки.
- [ ] Save F8, закрытие, запуск: восстановлены N/P и прежние свет/skybox/pass settings. Проверить N=1→5 возвращает сохранённый P.

### Task 8: Диагностика, сравнительные кадры и приёмка

**Files:** изменить `render_debug_state.h`, `render_debug_controller.cpp`, `terrain_editor_ui.cpp`, `profile_terrain_geometry.cpp`, audit/doc; создать `gbScripts/check-terrain-lod-demo.ps1`.

- [ ] Добавить read-only Terrain diagnostics: input/output triangles+vertices, node counts по LOD, requested/actual contour reduction, culled nodes, worker/select/upload times, GPU memory. Существующий pass label остаётся фактическим submitted count, а не оценкой геометрии.
- [ ] Demo runner запускает только свой процесс, включает Vulkan validation и screenshot layer, использует уникальный output directory, закрывает собственное окно и проверяет exit code. Не останавливать пользовательский экземпляр приложения.
- [ ] Снять before/after кадры при одинаковых config/camera/lighting/skybox и `GB_OCEAN_FIXED_TIME`. Для строгого terrain-diff отключить траву/деревья штатными GB_ISLAND_GRASS/GB_ISLAND_TREES; персонажей зафиксировать существующими `GB_RESIDENT_POSE=idle` и `GB_RESIDENT_TIME=0`. Дополнительный кадр полной сцены обязателен.
- [ ] Проверить P=0: силуэты, depth, normal/ID и world mask неизменны; исследовать каждый ненулевой image diff вне HUD. Проверить P=50: ближайший точен, дальние реально сокращены, morph плавный, нет разрывов с пеной и опорами.
- [ ] Повторить аудит исходного острова, одного штампа и полностью remeshed острова. Для оптимизированного кадра отдельно привести ws.base, обе shadowmap, ws.water.mask и total; не включать траву в число Terrain triangles.
- [ ] Запустить CPU runners задач 1–4, `check-terrain-editor.ps1`, `check-stairs.ps1`, `check-resident-stairs.ps1`, `check-island-resident.ps1`, `check-ocean.ps1`, `check-sun-shadows.ps1`, `check-graphics-settings.ps1`. Требуется exit 0 каждого; это CPU/smoke checks, не движковая unit-suite.
- [ ] Выполнить `./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64` и доступную Release-сборку; при PDB lock использовать изолированный output/PDB, не удалять занятые файлы. Выполнить focused demo flow: edit/undo/redo/save/load, лестницы, здания и движение персонажей.
- [ ] Fresh `git diff --check`, полный `git status --short`, проверить включение новых файлов в Windows/Xcode. Metal/OpenGL/Apple runtime пометить UNKNOWN, если не проверены отдельно.
- [ ] Обновить audit фактическими результатами. Зафиксировать регрессии/ограничения и не объявлять работу законченной, пока обязательные проверки или исправления не выполнены.

## Execution and review

Задачи выполняются последовательно: интерфейсы и поколения зависят друг от друга.
Рекомендуется native execution в текущей сессии. Subagent execution допустим
только при явном выборе пользователя, согласно активному правилу делегирования.

Репозиторий содержит значительный объём прежних незакоммиченных изменений.
Сохранять их; не делать reset/cleanup или общий commit. Каждый task завершается
просмотром только своего diff и доказательствами проверок. Integration/commit
не входят в автоматически подразумеваемую цель этого плана.

Перед исполнением требуется просмотр этого плана и выбор способа выполнения
по writing-plans. План не является утверждением, что описанные API уже существуют
или что CLOD уже реализован.
