# LVGL Dual Dashboard Simulator

A PC/simulator project built on [LVGL](https://github.com/lvgl/lvgl) + SDL2 that renders a **dual-dial automotive instrument cluster**:

| Left dial — Dashboard | Right dial — Infotainment |
|---|---|
| Battery arc & percentage | Album art & music metadata |
| Estimated range (km) | Incoming-call overlay |
| Drive-mode icon & label | Time & date |
| Lighting indicator | — |
| Left / Right turn-blinkers | — |

Both dials are 420 × 420 px circles rendered side-by-side in a 960 × 480 SDL window.

---

## Project structure

```
LVGL-Dual-Dash/
├── CMakeLists.txt          # CMake build configuration
├── lv_conf.h               # LVGL configuration
├── config/
│   └── FreeRTOSConfig.h    # FreeRTOS heap / tick settings
├── FreeRTOS/               # FreeRTOS kernel (optional)
├── lvgl/                   # LVGL library submodule
└── src/
    ├── main.c              # Entry point – dual-dial UI + simulation loop
    ├── freertos_main.c     # Alternative entry point with FreeRTOS tasks
    ├── mouse_cursor_icon.c # Custom cursor image asset
    ├── album_cover.c       # Album art image asset (LVGL C array)
    ├── adaptive.c          # "Driver assistance" icon asset
    ├── autonomous.c        # "Full autonomous" icon asset
    ├── charging.c          # Charging / low-battery icon asset
    ├── front_fog_light.c   # Front fog-light icon asset
    ├── low_beam.c          # Low-beam icon asset
    ├── parking_light.c     # Parking-light icon asset
    ├── rear_fog_light.c    # Rear fog-light icon asset
    ├── steering.c          # Steering / manual-drive icon asset
    ├── hal/
    │   ├── hal.h
    │   └── hal.c           # SDL HAL – display + mouse initialisation
    └── freertos/
        └── freertos_posix_port.c  # POSIX event helpers for FreeRTOS
```

---

## Features simulated in the main loop

### Left dial – Dashboard
| Feature | Detail |
|---|---|
| **Battery arc** | 270 ° arc drains from 30 % → 0 %; colour interpolates green → yellow → red |
| **Range label** | Calculated as `battery_pct × 1.6` km |
| **Drive-mode icon** | Switches automatically with battery level: Full Autonomous → Driver assistance → Manual Drive → LOW BATTERY |
| **Lighting indicator** | Cycles through: Parking light → Low beam (green) → High beam (blue) → Front fog → Rear fog |
| **Turn blinkers** | Alternates Left / Right / Hazard at ~1 Hz; arrows illuminate green when active |

### Right dial – Infotainment
| Feature | Detail |
|---|---|
| **Album art** | Static image rendered at reduced scale in the centre of the dial |
| **Music metadata** | Title and artist labels below the artwork |
| **Incoming-call overlay** | Toggles every ~10 s; hides music UI and shows caller name + number |
| **Time & date** | Static label at the bottom of the dial |

---

## Prerequisites

| Tool | macOS (Homebrew) | Debian/Ubuntu |
|---|---|---|
| SDL2 | `brew install sdl2` | `sudo apt install libsdl2-dev` |
| CMake | `brew install cmake` | `sudo apt install cmake` |
| C compiler | Xcode CLT or `brew install llvm` | `sudo apt install build-essential` |

---

## Build & run

```bash
# Clone with submodules
git clone --recursive <repo-url>
cd LVGL-Dual-Dash

# Configure and build (standard mode)
cmake -B build
cmake --build build -j$(nproc)

# Run
./bin/main
```

### Debug in VS Code

1. Open `simulator.code-workspace`.
2. Install the recommended extensions (CMake Tools, C/C++).
3. Open **Run and Debug** → select **Debug LVGL demo with gdb**.
4. Press **F5**.

### macOS – use Homebrew clang (avoids `-fsanitize=leak` issues)

```bash
brew install llvm
# In VS Code: Cmd+Shift+P → "CMake: Select a Kit" → pick the Homebrew clang entry
# Then: Cmd+Shift+P → "CMake: Configure"
```

---

## Optional: enable FreeRTOS

The project ships with a ready-made FreeRTOS entry point (`freertos_main.c`) that spawns an `lvgl_task` and a second background task.

1. In `lv_conf.h` change:
   ```c
   #define LV_USE_OS   LV_OS_FREERTOS
   ```
2. Build with:
   ```bash
   cmake -B build -DUSE_FREERTOS=ON
   cmake --build build -j$(nproc)
   ```

> **Heap note:** the FreeRTOS config allocates **512 MB** of heap (`configTOTAL_HEAP_SIZE`) to ensure the SDL window initialises promptly. Reducing this value may cause visible delays or prevent the window from appearing.

---

## Adding / replacing image assets

All icons and images are stored as LVGL C-array files (generated via the [LVGL image converter](https://lvgl.io/tools/imageconverter)). To replace an asset:

1. Convert your PNG/JPG to a C array with the LVGL online tool (format: **CF_TRUE_COLOR_ALPHA**).
2. Replace the corresponding `.c` file in `src/`.
3. Keep the existing `LV_IMG_DECLARE(…)` name in `main.c`, or update both the declaration and the `lv_image_set_src()` call.

---

## Licence

MIT — see [`licence.txt`](licence.txt).