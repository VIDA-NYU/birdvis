# BirdVis Qt6 migration

Updated: 2026-10-17. Status: **complete for the macOS development build; the Linux
build compiles and passes the headless tests.** The Qt4
application now builds with CMake/C++17 against Qt6, keeps the legacy OpenGL renderer
and embedded R calculations, and loads all 20 supplied datasets. Packaging, GUI
testing on Linux and physical-input testing remain out of scope (see
[Limits and open work](#limits-and-open-work)).

Baseline: BirdVis `cca0a66`. Comparison project: `/Users/csilva/src/taxivis`, whose
Qt6 migration and native interaction checks were used as a pattern for the build
layout and test structure. Merged through PRs #1 (port) and #2 (source layout).

## What was done

- **Build.** `CMakeLists.txt` replaces the Qt4 `birdvis.pro`. Production code is a
  static `birdvis_core` library, so tests exercise the same code as the application.
  Dependencies are discovered through CMake targets; `cmake/FindEmbeddedR.cmake`
  queries R for flags, Rcpp and RInside, and records the R home and package library.
- **Qt API.** `QGLWidget` became `QOpenGLWidget` (desktop OpenGL 2.1, no profile,
  stencil buffer, requested before `QApplication`). Screen, mouse (`position()`,
  `MiddleButton`) and wheel (`angleDelta()` plus pixel deltas) APIs updated; Widgets
  headers made explicit; dialog cancellation preserves the current view.
- **OpenGL behaviour.** The area estimator uses bounded heap storage, validates empty
  rectangles, and restores framebuffer, viewport, scissor and matrix state. Logical
  pixels are converted to framebuffer pixels for viewport and scissor (Retina).
  Events make the GL context current. Image and scalar-field GPU objects have
  context-aware ownership and cleanup, so windows can be recreated.
- **Assets.** The original `usa_st` and `counties` shapefiles and the 28 font
  description/SGI files were recovered from `birdvis_06_16_2011/birdvis.app`
  (hashes in `resources/provenance.json`). The 11 `BVdata_sotb_experiments` files were
  copied from the same distribution. Resource validation runs before `QApplication`
  starts and reports resolved paths on failure.
- **Paths.** Data, resources, images and the R library resolve independently of the
  working directory, with `BIRDVIS_DATA`, `BIRDVIS_RESOURCES` and `BIRDVIS_IMAGES`
  overrides. Dataset discovery is recursive and deterministic.
- **R, GDAL, images.** R evaluation helpers no longer fall through after errors.
  Regions use `GDALOpenEx`/`GDALDataset`. FreeImage was kept; SGI decoding is checked
  byte for byte against the legacy decoder. Dataset loading validates metadata,
  parses dates through R, and normalizes the several encodings of absent predictors
  in memory without altering the files.
- **Scientific fix.** The spatial-sum path summed cumulative entries. It now recovers
  ordinary cell values from the `bv.integral` summed-area matrices. Synthetic tests
  compare every subrectangle with direct sums and check temporal curves and variation.
- **Search.** The obsolete online geocoder (shell-based) was removed. Search accepts a
  state or `county, state` using the original local boundaries; city lookup reports
  that it is unavailable.
- **Layout.** Sources live under `src/{app,dataflow,modules,env,data,render,ui,math}`,
  with each subdirectory on the include path (includes remain flat, e.g.
  `"Dataflow.hpp"`). Obsolete `birdvis.pro` and `pkgconfig/` were deleted.

## Architecture notes

- **Dataflow.** `src/dataflow` is a pull-based module graph. `Module::run()` does
  nothing unless the module is outdated; otherwise it runs upstream modules, then
  `process()`. Renderer modules are the sinks: each frame pulls only stale parts of
  the graph, and `setData(..., true)` invalidates downstream modules. The graphs are
  wired by hand in the `Environment` and `SimpleEnvironment` constructors, with no
  scheduler, threads or serialization. Mouse/key events use the separate
  `EventSystem` and Boost.Signals2.
- **Rendering.** Immediate mode, display lists, matrix stacks, GLU mipmaps, stencil
  clipping and EXT framebuffer calls were retained on purpose. A core-profile or
  shader rewrite is not part of this migration.
- **R.** Embedded R stays on its owning (UI) thread, so dataset loading blocks the
  UI. TaxiVis's worker-query design cannot be copied here.

## Decisions

| ID | Status | Decision |
| --- | --- | --- |
| D01 | Done | Port directly to Qt6 (no intermediate Qt5 branch). |
| D02 | Accepted | Target macOS first; claim Linux only after testing it. Linux now builds and passes the headless tests; GUI untested. |
| D03 | Done | CMake/C++17 with a core library plus CTest targets. |
| D04 | Done | Preserve legacy desktop OpenGL; defer core profile/Metal/shaders. |
| D05 | Done | Preserve R/dataflow and dataset semantics; defer async execution. |
| D06 | Done | Restore the original fonts and maps rather than substitute them. |
| D07 | Done | Keep FreeImage (isolated in `Texture.cpp`); QImage remains an option. |
| D08 | Done | Explicit data/resource overrides plus compiled-in development defaults. |
| D09 | Deferred | Relocatable bundle, bundled R and signing. |

## Verification

Dependencies on the verified machine: CMake 4.4.3, Qt 6.11.2, R 4.6.1, Rcpp 1.1.2,
RInside 0.2.19, GDAL 3.13.3, GLEW 2.3.1; OpenGL 2.1 (Metal) with an 8-bit stencil
buffer and device pixel ratio 2.

```sh
R_LIBS_USER="$PWD/.R-library" cmake -S . -B build
cmake --build build -j 4
ctest --test-dir build --output-on-failure        # headless, 4 tests
./build/gl_probe                                  # needs a display
./build/native_check /tmp/birdvis-check.png       # needs a display
```

- **CTest (4/4):** `missing_resources`, `resource_decoding`, `scientific_calculations`
  (includes malformed RData rejection), `bundled_r_data` (all datasets load and have
  52 weekly dates, 2009-01-05 to 2009-12-28).
- **`native_check`:** exits 0. It drives all three environments, species drag/drop,
  selection create/move/delete with framebuffer pixel checks, pan/zoom including
  pixel wheel input, time stepping, interpolation/stencil/borders, colormap stop and
  color editing, cancelled dialogs, local place search, resize/minimize/restore and
  window recreation, and saves screenshots.
- Re-run after the `src/` reorganization on 2026-10-17: clean build, 4/4 CTests,
  `native_check` exit 0, and a launch from a different working directory.

### Linux build (2026-10-17)

Ubuntu 24.04 (kernel 6.8), GCC 13.3, CMake 3.28.3, Qt 6.4.2, GDAL 3.8.4, GLEW 2.2.0,
R/Rcpp/RInside from apt (`r-base-dev`, `r-cran-rcpp`, `r-cran-rinside`). Configure
and build succeeded with no source or CMake changes, so Qt 6.4 is sufficient even
though development used 6.11. `QT_QPA_PLATFORM=offscreen ctest` passed 4/4. CMake
logs a harmless "Could NOT find XKB" from Qt. **Not run on Linux:** `gl_probe`,
`native_check` and the interactive application, since the machine had no display.
Whether the legacy OpenGL 2.1 renderer behaves correctly under Mesa/X11/Wayland is
therefore still unknown.

## Limits and open work

Not validated: the Linux GUI (build and headless tests only), a distributable macOS bundle, physical mouse/trackpad
hardware (the native checks synthesize Qt events), and other display scales. The
numerical tests validate the stored cumulative-data representation, not the
underlying ecological models. The Golden Eagle auxiliary image
(`Golden_Eagle_AAB.jpg`) was absent from the original distribution.

Possible follow-ups, none scheduled:

1. Path-style includes (`"dataflow/Dataflow.hpp"`) instead of flat names.
2. CI for the headless tests (needs Qt, R/Rcpp/RInside and GDAL on the runner).
3. Run `gl_probe` and `native_check` on a Linux desktop (X11 and Wayland), and
   add a Linux CI job for the headless tests.
4. Replace the legacy OpenGL renderer (macOS has deprecated OpenGL).
5. Move R loading off the UI thread; reduce `using namespace std;` in headers and
   the shared global state behind `Global`.
6. Git LFS or an external download if the datasets grow (about 400 MB today).
