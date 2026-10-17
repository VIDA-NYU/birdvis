# BirdVis Qt6 migration plan

Updated: 2026-10-16. Status: macOS Qt6 development port implemented; final verification recorded below. Distribution and other platforms remain out of scope.

## Current completion status

The implementation now provides the Qt6 development application on the current
macOS machine. The historical assessment and intermediate logs below are retained;
this section and the final verification entry supersede their open implementation
status. Packaging/signing, bundled R, and validation on other platforms remain
outside this development-port scope.

- [x] CMake/C++17 and current Qt6/GDAL/R integration.
- [x] Original map/font assets restored with hashes; all 20 datasets discovered.
- [x] Framebuffer/state preservation, Retina viewport/scissor conversion,
  current contexts during events, and context-aware GPU resource handling.
- [x] Scientific tests for cumulative-grid extraction, spatial totals, temporal
  curves, variation, invalid inputs, and loading/dates for every bundled dataset.
- [x] Native-display checks of all three environments, species replacement,
  selection curves, time controls, colormaps/dialogs, resize/restore, and a new
  window/context; screenshots inspected.
- [x] Working-directory-independent paths and configured R library defaults.
- [x] Setup/run instructions, resource manifest, automated checks, known limits.

Validation limits: native interactions are generated through Qt on the current
Retina display, rather than physical mouse/trackpad hardware. No claim of Linux
or distributable-bundle validation. Place search is now local state/county lookup;
the obsolete online city endpoint is not retained. Scientific tests validate the
stored cumulative-data representation, not the underlying ecological models.

## Objective and scope

Assess and track a Qt4-to-Qt6 migration that restores BirdVis's existing
single-species, multi-species, and variation workflows. Preserve scientific
calculations and the supplied datasets while bringing the application onto a
reproducible build. The assessment below records the original findings. Implementation has since
started in the working tree; the continuation log records verified progress.

Baseline: BirdVis `cca0a66`; comparison: `/Users/csilva/src/taxivis` at `28c2ed6`.
TaxiVis's PLAN.md records completion of its Qt6 migration and native interaction
checks. Its README still describes the migration as in progress; use the code,
commit history, and detailed verification log as the stronger evidence.

## Assessment

Feasible without initially rewriting the renderer. The direct Qt API surface is
small: one QGLWidget subclass, application/screen setup, mouse/wheel events, and
a few input/color dialogs. BirdVis has no QCustomPlot migration or .ui form
migration comparable to TaxiVis. However, TaxiVis's final Qt5-to-Qt6 step started
with QOpenGLWidget already in place. BirdVis still needs that transition, plus
embedded R, GDAL, missing resources, and runtime correctness work.

The main uncertainty is restoration of a working scientific application, rather
than the mechanical Qt replacements. A successful compilation is not the
completion criterion. Do an early context/data/resource feasibility milestone
before committing to a delivery estimate. Relative effort: Qt API changes low;
build/dependency integration medium; rendering validation and asset restoration
medium-to-high; dataset compatibility unknown until R is available.

## Findings and required work

### 1. Build and dependencies

- `birdvis.pro` is the only build definition. It invokes R for compile/link
  flags, hard-codes `/usr/include/gdal`, and links GLU, GLEW, GDAL, and FreeImage.
- Local qmake reports Qt 6.11.2. Boost and GLEW Homebrew prefixes exist. `R` is
  absent from PATH; Homebrew prefixes for R, GDAL, and FreeImage were not found.
  This is an environment observation, not proof that no other installations exist.
- Proposed CMake target: C and C++ sources, C++17, AUTOMOC, and Qt6 Core, Gui,
  Widgets, OpenGL, OpenGLWidgets. Add Qt Test only for test targets. BirdVis does
  not currently need TaxiVis's Network, PrintSupport, Concurrent, or AUTOUIC setup.
- Discover OpenGL/GLU, GLEW, GDAL, Boost, and initially FreeImage through targets
  and configuration queries, not copied Homebrew absolute paths. Query the chosen
  R executable for RHOME, flags, Rcpp and RInside include/link configuration;
  report missing packages at configure time. Validate architecture and runtime
  library discovery as well as compilation. Preserve C compilation for `.c` files.
- Make dependency versions and actual configure/build commands part of the log.
  Identify which compiled Boost libraries are really needed; do not copy
  TaxiVis's component list or BirdVis's obsolete pkgconfig files blindly.

### 2. Qt API changes

| Location | Required change |
| --- | --- |
| `GLContext.hpp`, `GLContext.cpp` | QGLWidget → QOpenGLWidget; QGLFormat → QSurfaceFormat; replace stencil capability query. |
| `app.cpp` | QDesktopWidget/QApplication::desktop() → QScreen available geometry; choose the window's screen. |
| `GLContext.cpp` mouse handlers | Use position(), preserve bottom-left coordinate conversion, replace Qt::MidButton with Qt::MiddleButton. |
| `GLContext.cpp` wheel handler | Replace delta() with angleDelta(); establish intended horizontal/vertical mapping and test trackpad behavior. Existing code forwards the same delta twice. |
| `Environment.cpp`, `SimpleEnvironment.cpp`, `ColormapModules.cpp` | Explicit Widgets headers for QInputDialog, QLineEdit, QColorDialog, etc.; check dialog cancellation paths. |
| `Global.hpp`, `modules.hpp`, `gl_util.hpp`, GL/UI sources | Replace broad QtGui/QtOpenGL includes with narrow includes; resolve GLEW/Qt header conflicts. |
| `GLContext.cpp` repaint paths | Prefer update(); avoid synchronous repaint inside resizeGL; validate redraw scheduling. |

Qt documents removal of QGL classes and the OpenGLWidgets module requirement:
[Qt6 OpenGL changes](https://doc.qt.io/qt-6/opengl-changes-qt6.html).

### 3. OpenGL behavior: principal migration risk

The renderer uses immediate mode, display lists, matrix stacks, GLU mipmaps,
stencil clipping, and EXT framebuffer operations. Retain these initially.
Proposed initial request: desktop OpenGL 2.1 with NoProfile, stencil buffer,
and multisampling, set before QApplication construction. Log the actual format
and capabilities. Do not request a core profile for this renderer. Profiles
are defined for 3.2+, so calling a 2.1 context a formal CompatibilityProfile
would be misleading. See [QSurfaceFormat](https://doc.qt.io/qt-6/qsurfaceformat.html).

Concrete audits:

- `Global::estimateUSAreaCoefficientAtRectangle()` binds framebuffer 0 at
  Global.cpp:232 and :297. Save and restore the caller's framebuffer binding,
  including read/draw targets where applicable. Use the widget framebuffer
  when returning explicitly to widget rendering. Never cache its ID across resize.
- This helper also changes viewport, matrices, blend/depth state and other GL
  state. Restore its complete state contract, not just the framebuffer. It is
  called both during initialization and by selection calculations in BasicModules.
- `RenderSystem.cpp` directly calls glViewport/glScissor with layout coordinates.
  Keep input and layout in logical pixels; convert viewport/scissor rectangles
  consistently to framebuffer pixels. Test Retina and different display scales.
- Ensure GL calls outside initializeGL/resizeGL/paintGL run with a current context.
  Audit texture uploads and selection-area calculations reached from event paths.
- Define ownership and context-aware cleanup/recreation for textures, display
  lists, and offscreen buffers; do not assume a context lives forever.
- Confirm each frame clears/rebuilds the required color/stencil contents.
- Resolve GLEW/header dispatch centrally. TaxiVis contains local GLEW macro
  workarounds; these are evidence of a pitfall, not a pattern to spread here.
- Replace the variable-length stack buffer in the area estimator with a bounded
  allocation and validate zero/negative dimensions before aspect-ratio arithmetic.

Qt's widget framebuffer and context lifetime requirements are documented in
[QOpenGLWidget](https://doc.qt.io/qt-6/qopenglwidget.html).

### 4. Assets and launch paths: blockers independent of Qt

- `Global.cpp` discovers datasets through cwd/data and loads cwd/resources/bvlogo.png.
- `GLContext.cpp` assumes executable/../Resources for fonts and map boundaries.
- Required missing files: `usa_st.shp`, `counties.shp` and their shapefile sidecars;
  `fonts/arial_{7..19,200}.description` and corresponding `.sgi` textures.
  The tracked resources directory contains only bvlogo.png. TaxiVis's file
  inventory contains no matching font/map assets; its ZIP counterpart was not
  assumed to provide them. BirdVis's images/Archive.zip contains bird photographs.
- Missing maps currently call exit(1); font loading does not provide safe startup
  recovery. Add preflight validation with resolved paths and actionable errors.
- First try recovering original assets and provenance. If unavailable, choose
  replacement map data and/or a font atlas generator/text renderer explicitly.
  Maps affect area estimates and stencil masks, not only appearance: preserve
  coordinates, state/county naming, continental-US filtering, and verify results.
- Proposed resolution: explicit data/resource overrides, development defaults,
  and installed/bundle locations; launch must work outside the repository cwd.
  Do not silently ship a blank map as a completed migration.

### 5. R, GDAL, image loading, and C++ compatibility

- Keep RInside/Rcpp and existing .RData input for the first port. Load all nine
  bundled datasets with the chosen R version and inventory metadata, dates,
  dimensions, predictors, photos, and numerical ranges. They have roughly
  234–247 MB uncompressed gzip payloads each; measure startup and memory use.
- The R helpers and get2DGrid use summed-area matrices/inclusion-exclusion.
  Preserve this representation; do not treat the matrices as raw cell values.
  Use synthetic grids and direct summation to check extraction, spatial sums,
  temporal curves, and first/subsequent variation frames.
- Fix evalInt/evalNumericVector/evalNumericMatrix falling through non-void
  functions after R errors. Use a consistent exception/result policy; do not
  rely on a later evalOK() check after undefined behavior.
- Fix null-directory handling and missing closedir in addFilesFromDirectory.
  Guard missing R_HOME in diagnostics and validate R initialization early.
- `Regions.hpp` has a nonportable `<GDAL/ogrsf_frmts.h>` include. Move legacy
  OGRSFDriverRegistrar::Open/OGRDataSource usage to GDALOpenEx/GDALDataset and
  GDALClose; check layer lookup. Clone/own geometries before releasing features.
  See [GDAL migration background](https://gdal.org/en/stable/development/rfc/rfc46_gdal_ogr_unification.html).
- FreeImage is isolated largely in Texture.cpp. Retaining it minimizes initial
  change; QImage is an alternative if installation is troublesome. Any replacement
  must test vertical orientation, row stride, channel order, and alpha behavior.
- Expect modern-compiler fixes: Boost bind placeholders, include dependencies,
  nonstandard arrays, and ownership/error handling. Limit initial cleanup to
  build blockers and paths necessary for reliable migration tests.
- Keep embedded R on its owning thread initially. TaxiVis's worker-query design
  cannot simply be copied into an embedded-R application.

## Reuse from TaxiVis

Reference files under `/Users/csilva/src/taxivis`:

- `src/TaxiVis/CMakeLists.txt`: Qt6 target linkage, reusable production library
  for tests, CTest organization. Simplify to BirdVis dependencies.
- `src/TaxiVis/QMapWidget.cpp`: QOpenGLWidget setup and update scheduling.
  TaxiVis embeds it in QGraphicsView; BirdVis renders directly in its subclass.
- `src/TaxiVis/layers/TripLocation.cpp` and `GridMap.cpp`: context capability
  checks and GL integration pitfalls.
- `tests/native_interaction_check.cpp`, `PLAN.md`: separate data/automated tests
  from real-display interaction checks. Offscreen test success is not GL proof.
- Dataset path overrides and explicit startup diagnostics are useful patterns.
  Tile caching, QCustomPlot, and asynchronous trip queries are not prerequisites.

## Decision register

| ID | Status | Decision / rationale |
| --- | --- | --- |
| D01 | In progress | Qt6 migration implementation continued in the working tree; full runtime acceptance remains open. |
| D02 | Proposed | Target the current macOS machine first; retain portable code but claim Linux support only after testing. |
| D03 | Proposed | Use CMake/C++17, following TaxiVis's target/test structure with BirdVis-specific dependencies. |
| D04 | Proposed | Port directly to Qt6; an intermediate maintained Qt5 branch adds little value here. |
| D05 | Proposed | Preserve legacy desktop OpenGL initially; defer core-profile/Metal/shader rewrite. |
| D06 | Proposed | Preserve R/dataflow and dataset semantics; defer asynchronous execution and data-format redesign. |
| D07 | Open, early gate | Recover original fonts/maps or select documented substitutes with numerical validation. |
| D08 | Open | Retain FreeImage vs replace with QImage after dependency feasibility check. |
| D09 | Proposed | Add explicit data/resource path overrides and bundle-aware resolution. |
| D10 | Proposed | Defer distributable R bundling/signing until a verified development build exists. |

## Milestones and acceptance checks

- [x] M0: inspect BirdVis and TaxiVis; record findings and proposed decisions.
- [ ] M1: resolve asset inventory/provenance; establish dependency versions;
  verify a minimal Qt6 legacy-GL context with stencil/FBO support and load a
  bundled dataset through RInside. Record feasibility results before broad edits.
- [ ] M2: add CMake, Qt API changes, modern dependency APIs, and essential error
  handling. Clean out-of-tree build; failures identify dependencies/resources.
- [ ] M3: reach a visible first frame with maps, text, images, and species list;
  fix framebuffer restoration, context access, state handling, and display scale.
- [ ] M4: validate dataset schema and numerical extraction/aggregation against
  synthetic/direct-R references, including invalid input and time boundaries.
- [ ] M5: native interaction verification with recorded dataset and screenshots:
  - [ ] Single species, multiple species, and variation environments.
  - [ ] Species drag/drop, panel layout/resizing, closing/recreating views.
  - [ ] Pan/zoom, wheel/trackpad, keyboard modifiers, spatial selection lifecycle.
  - [ ] Time stepping, offsets, linked plots and selection curves.
  - [ ] Colormap editing, dialogs, interpolation, stencil toggle, predictor clouds.
  - [ ] Area estimation followed by redraw without blank/corrupted frames.
  - [ ] Retina sizing, clipping, text, cursor alignment; resize/minimize/restore.
  - [ ] Missing/corrupt data/resources produce useful errors rather than crashes.
  - [ ] Launch from a different cwd; document unresolved image/browser actions.
- [ ] M6: README setup/run instructions, resource manifest, meaningful automated
  tests and native check instructions; record remaining platform limitations.

## Verification log

- 2026-10-16: read-only source review of both repositories; checked local Qt and
  selected dependency paths and official Qt/GDAL documentation. No full BirdVis
  build or application launch performed; no dependency installation attempted.
- Initial repository review: clang++ syntax-only checks of Dataflow.cpp,
  Labeling.cpp, and Tokenizer.cpp passed with warnings. This is neither a Qt6
  build nor evidence of runtime correctness.
- Only PLAN.md was added for this assessment. Future entries should include
  commands, versions, dataset, results, and which decisions they resolve. Keep
  proposals distinct from accepted decisions and untested checks unchecked.

### Implementation continuation — 2026-10-16

- Existing working-tree changes provide CMake/C++17, Qt6 QOpenGLWidget and input
  APIs, GDALOpenEx, dependency discovery, resource/data environment overrides,
  and modern compiler compatibility fixes. FreeImage is retained.
- Dependencies verified locally: Qt 6.11.2, R 4.6.1, GDAL 3.13.3. Rcpp and RInside
  are installed under `.R-library`; configure with
  `R_LIBS_USER="$PWD/.R-library" cmake -S . -B build`, then
  `cmake --build build -j 4` and `ctest --test-dir build --output-on-failure`.
- The application and both probes build. The R probe loaded all nine bundled
  datasets and checked required top-level schema fields; each has 52 dates.
  This does not yet validate numerical extraction, memory use, or all predictors.
- Native `./build/gl_probe` passed outside the sandbox: OpenGL 2.1 Metal - 91.7,
  depth 32, stencil 8, actual samples 0, device pixel ratio 2. Legacy immediate
  drawing reported GL_NO_ERROR and framebuffer readback returned the expected red
  center pixel. This is context feasibility evidence, not application rendering QA.
- Resource validation now precedes QApplication initialization and checks readable
  regular files. The `missing_resources` CTest checks an actionable exit from a
  different cwd with an unavailable Qt platform, without requiring a display.
- M1 remains incomplete because original map/font resources are absent. A fresh
  `build-clean` configure/build also passed, with both CTests passing. M2 build
  feasibility is established; dependency and failure-path audits remain open.
  M3–M6 remain open, including framebuffer/state restoration, Retina coordinate
  conversion, context lifetime handling, scientific numerical tests, and native
  workflow checks. No full application first frame has been verified.
- Next gate: recover `usa_st` and `counties` shapefile sets and the Arial font
  atlases listed in README.md, with provenance. Do not substitute map geometry
  without validating its scientific impact on area estimates.

- Clean-build verification: `R_LIBS_USER="$PWD/.R-library" cmake -S . -B build-clean`,
  `cmake --build build-clean -j 4`, and
  `ctest --test-dir build-clean --output-on-failure` passed (2/2 tests).
  Legacy compiler warnings remain; `git diff --check` passes.

- Resource recovery follow-up: inspected `../old.birdvis`, including hidden and
  ignored files, its resource history across local Git refs, and the contents of
  `images/Archive.zip`. The resources directory contains only `bvlogo.png`; no
  shapefiles or font atlas files were found. The ZIP contains bird photographs.
  This copy does not resolve the missing-resource gate.

### Original resources recovered — 2026-10-16

- User identified `/Users/csilva/birdvis_06_16_2011`. Restored 34 files from its
  `birdvis.app/Contents/Resources`: both shapefile triplets and all 28 required
  font description/SGI files. Each font file is byte-identical to `fonts.zip`.
  Source paths, byte sizes, and SHA-256 hashes are in `resources/provenance.json`.
  This resolves the missing-resource gate and D07 using original assets.
- GDAL opens the original `usa_st` (51 polygon features) and `counties` (3,140
  polygon features) layers. The supplied shapefiles have no declared SRS;
  geometry and metadata were preserved unchanged.
- Native startup initially failed to find Rcpp without `R_LIBS_USER`. Retrying
  with `R_LIBS_USER=/Users/csilva/src/birdvis/.R-library ./build-clean/birdvis`
  initialized all 14 font textures and both maps, reporting area coefficient
  0.563727 for the startup rectangle. The process stayed running and was stopped
  after this smoke check. README now includes the runtime R library setting.
- Startup logs establish resource loading, not visual correctness. Framebuffer
  state, Retina layout, numerical validation, and full interaction checks remain
  open; no screenshot or native interaction acceptance is claimed here.

- Copied the 11 additional RData files from the original distribution's
  `data/BVdata_sotb_experiments/` into the same repository subdirectory (88,461,125
  bytes total), verifying each copy by SHA-256. Loaded all 11 in R and verified
  that `bv.dates` contains exactly the same 52 weekly dates from 2009-01-05 through
  2009-12-28. The current application scans only its selected data directory,
  so use `BIRDVIS_DATA` pointing to this subdirectory to list these experiments.
  Full rendering and numerical compatibility of these datasets remains untested.

### Qt6 implementation and verification — 2026-10-16

- Moved production code into `birdvis_core` so regression tests exercise the same
  implementation as the application. Added numerical, resource, and native checks.
- Area estimation now uses bounded heap storage, checks invalid/empty rectangles,
  preserves caller GL state and separate read/draw framebuffer bindings, and returns
  to the widget framebuffer without binding framebuffer zero. Synthetic half-area
  geometry verifies the result and framebuffer/viewport/scissor restoration.
- RenderSystem converts logical viewport/scissor edges to framebuffer pixels.
  Input remains in logical coordinates. Mouse events refresh hit targets and make
  the GL context current; wheel events support both angle and pixel deltas.
- Added context ownership for image/scalar-field GPU objects, cleanup for map/font
  resources, reinitialization for recreated windows, shared-context configuration,
  and useful errors for unsupported GL contexts and failed event operations.
- Recovered ordinary cell values from cumulative matrices in the spatial-sum path
  using `bv.integral`, replacing the incorrect sum of cumulative entries. Synthetic
  tests compare all subrectangles with direct sums and check curves and variation.
  Variable quads now use their dataset's bounds; unknown variables reject cleanly.
- Dataset loading validates required metadata, parses dates through R formatting,
  accepts the original no-predictor sentinels, and uses a bound R path variable.
  All 20 files load through the actual Repository/File code and have 52 dates.
- Resource handling: fixed the font character-table allocation and malformed font
  handling; SGI decoding uses FreeImage and is byte-for-byte checked against the
  legacy decoder for all atlas pixels. Fixed image conversion leaks and stale IDs.
- Recursive deterministic data discovery includes the restored experiments. Images
  and configured R libraries resolve without depending on cwd. Missing auxiliary
  images report their absence; original data and recovered assets remain unchanged.
- Dialog cancellation preserves the environment; layout dimensions and colormap
  stops are validated. Corrected missing returns found by the compiler. Toolbar
  buttons fit their assigned panel width during resizing.
- Removed the shell-based geocoding path. Local exact state/county names use GDAL
  envelopes; Google Maps and image actions use Qt desktop URL handling. City lookup
  is explicitly unavailable instead of leaving uninitialized map bounds.
- Native checks and inspected screenshots cover American Three-toed Woodpecker,
  Bobolink, and Indigo Bunting, including midsummer occurrence, weekly variation,
  selection curves, county borders, predictors, color/stop editing, cancelled
  dialogs, state/county search, pan/zoom, time stepping, and window recreation.
- Verified dependencies: CMake 4.4.3, Qt 6.11.2, R 4.6.1, Rcpp 1.1.2, RInside 0.2.19,
  GDAL 3.13.3, GLEW 2.3.1. Native GL reports OpenGL 2.1 Metal with an 8-bit stencil
  buffer and device pixel ratio 2. Legacy deprecation warnings remain.

- Reconfiguration regression: cached the configure-time R user library so automatic
  CMake regeneration works without exporting R_LIBS_USER again. Both explicit
  configure without that environment variable and the subsequent build pass.
- Native dialog automation uses visible-dialog polling and Qt dialogs; this avoids
  timing-dependent responses to macOS native sheets. OS-native dialog automation
  is not part of the repeatable regression claim.

### Final recorded checks

- Fresh `build-verify` configuration and complete compilation passed. Final rebuilds
  also passed after the review fixes; no missing-return diagnostics remain.
- `ctest --test-dir build-verify --output-on-failure`: 4/4 passed
  (`missing_resources`, `resource_decoding`, `scientific_calculations`,
  `bundled_r_data`). The numerical test includes malformed RData rejection.
- `./build-verify/native_check
  /Users/csilva/src/birdvis/build-verify/verification/birdvis.png`: exited 0.
  This final run includes synthesized pixel-wheel input, selection deletion with
  framebuffer pixel verification, colormap stop/color editing, cancelled layout
  dialogs, and all prior native workflows. It switches cwd to the temporary
  directory before startup. Screenshots are under `build-verify/verification/`.
- Visually inspected the single-species selection/colormap, midsummer multi-species,
  variation, resized, and recreated-window screenshots. The final normal build
  remains available as `./build/birdvis`. `git diff --check` passes.
- Remaining validation limits are listed in the current completion status and
  README. No distribution/signing, other-platform, or physical-trackpad claims.
