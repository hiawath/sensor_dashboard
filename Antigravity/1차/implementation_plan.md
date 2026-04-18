# UI Revamp: Hybrid Approach (UI File + QSS)

Based on your feedback, we will roll back to using the Qt Designer `mainwindow.ui` file combined with C++ QSS styling to achieve the Neo-Brutalist layout from the image.

## User Review Required
> [!IMPORTANT]
> Writing complex `mainwindow.ui` XML manually (without Qt Designer) is error-prone. To implement the "hybrid" approach, I will create a structured `.ui` file defining the main regions (Top Bar, Side Bar, Dashboard Panels) and basic widgets (Labels, Buttons). I will then use C++ (`mainwindow.cpp`) to inject the QSS styling, dynamic chart elements, and logic.
> *Are you okay with me over-writing the current `mainwindow.ui` with a new XML structure that matches the image's layout?*

## Proposed Changes

### 1. `mainwindow.ui`
- Replace the current basic XML with a complex layout XML.
- Add nested layouts:
  - `mainLayout` (Vertical)
    - `topBar` (QFrame, Horizontal)
    - `bodyWidget` (QWidget, Horizontal)
      - `sideBar` (QFrame, Vertical)
      - `dashArea` (QWidget, Vertical)
        - Header Area
        - `row1Layout` (Temperature Panel, Actuators Panel)
        - `row2Layout` (GPIO Panel, Terminal Panel)
- Assign `objectName`s to all widgets (e.g., `initBtn`, `terminalDisplay`, `portCombo`, `panelBox`) so they can be targeted by QSS and accessed via the `ui->` pointer in C++.

### 2. `mainwindow.h`
- Revert back to using `Ui::MainWindow *ui`.
- Declare slots and member variables needed for the logic (e.g., serial port instance).

### 3. `mainwindow.cpp`
- Use `ui->setupUi(this)` to load the XML layout.
- Inject the comprehensive Neo-Brutalist QSS string (`applyStyles()`) to style the UI elements loaded from the `.ui` file based on their `objectName`.
- Populate dynamic content (like the 15 bars for the temperature chart) programmatically into the layout defined in the `.ui` file (`ui->chartLayout`).
- Connect `ui->initBtn` and the serial port logic.

## Verification Plan
- Build the project using CMake.
- Verify `uic` correctly parses the new `mainwindow.ui`.
- Run the executable and ensure the layout and styling match the target image.
