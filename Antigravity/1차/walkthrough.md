# Hybrid Qt Dashboard UI

We successfully implemented the "Hybrid" approach, utilizing both the `mainwindow.ui` file for structure and C++ QSS for the styling.

## Key Changes Made

### 1. `mainwindow.ui` Layout Structure
- **Restored and Overhauled**: We kept the `mainwindow.ui` file but completely replaced its contents with a complex, nested layout architecture that matches the provided mockup.
- The XML now defines the `topBar`, `sideBar`, and the 4 main dashboard quadrants (`tempPanel`, `actPanel`, `gpioPanel`, `termPanel`) along with their respective labels, buttons, and text fields.
- By defining this in XML, the layout can be previewed or tweaked inside **Qt Designer** if needed.

### 2. C++ Dynamic UI Population
- Repetitive or dynamic elements (like the 15 bars in the Temperature Chart and the 4 GPIO toggle boxes) are generated programmatically inside `mainwindow.cpp` via the `setupDynamicUI()` function and injected into the layouts defined in the `.ui` file (`ui->chartLayout`, `ui->gpioGrid`).

### 3. Neo-Brutalist QSS Styling
- A comprehensive Qt Style Sheet (QSS) is still applied in `mainwindow.cpp` (`applyStyles()`).
- The QSS targets the specific `objectName`s defined in the `mainwindow.ui` file (e.g., `#initBtn`, `#termPanel`, `#panelBox`) to apply the Monospace font, thick borders, and heavy drop shadows.

## Verification
- The project successfully compiled using `cmake` and the `uic` correctly generated the `ui_mainwindow.h` header from our custom XML layout.
- You can run the application by executing `./build/QT_Dashboard`.

> [!TIP]
> You can now open `mainwindow.ui` in Qt Creator/Designer to visually inspect the widget tree and adjust basic layouts, while the colors and borders will still be enforced by the QSS in `mainwindow.cpp`.
