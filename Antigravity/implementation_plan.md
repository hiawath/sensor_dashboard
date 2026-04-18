# UI Revamp: Neo-Brutalist Dashboard

The goal is to update the current Qt project's user interface to match the provided image, which features a bold, high-contrast "Neo-Brutalist" design language (thick borders, monospace fonts, solid distinct colors).

## User Review Required
> [!IMPORTANT]
> **Abandoning `mainwindow.ui`**: Creating a UI of this complexity and styling using the XML-based Qt Designer (`.ui` file) is extremely rigid and error-prone for our workflow. I propose we build the UI **programmatically in C++** within `mainwindow.cpp` and apply a comprehensive Qt Style Sheet (QSS). This will give us pixel-perfect control over the styling and layouts. 
> *Are you okay with dropping the use of the Qt Designer `.ui` file for the main window?*

## Open Questions
> [!NOTE]
> 1. **Font**: The design relies heavily on a monospace or industrial font. I plan to use a widely available monospace font like `Courier New`, `Consolas`, or `Monospace`.
> 2. **Icons**: The design has several icons (Settings, Power, User, Temp, etc.). I will replace these with text symbols or basic colored shapes unless you have specific icon assets you want me to use.
> 3. **Dynamic Data**: The mockups show dynamic data (Temperature Chart, CPU Load). Should these be purely visual mockups for now, or should I attempt to animate them with dummy data?

## Proposed Changes

### UI Architecture (Code)
We will define custom layouts and widgets directly in `mainwindow.h` and `mainwindow.cpp`.

#### [MODIFY] `mainwindow.h`
- Remove `#include "ui_mainwindow.h"` and the `Ui::MainWindow *ui;` pointer.
- Declare the UI elements as member variables (e.g., `QWidget* topBar`, `QFrame* sideBar`, `QComboBox* portCombo`, `QPushButton* initBtn`, `QTextBrowser* terminalDisplay`, etc.).
- Declare functions for layout setup: `void setupUI();`, `void applyStyles();`.

#### [MODIFY] `mainwindow.cpp`
- Remove `ui(new Ui::MainWindow)` and `ui->setupUi(this)`.
- Implement `setupUI()` to construct the Top Bar, Left Sidebar, and Main Dashboard Area (Hardware Monitor, Temperature, Actuators, GPIOs, and the raw data stream).
- Implement `applyStyles()` to inject the Neo-Brutalist Qt Style Sheet (QSS), defining the heavy black borders, shadow effects on buttons, and background colors.
- Wire up the new "INITIALIZE" button to the serial connection logic.
- Connect the existing `readData()` slot to append to the new `terminalDisplay` with the dark styling.

#### [MODIFY] `CMakeLists.txt`
- Remove `mainwindow.ui` from the `PROJECT_SOURCES` to clean up the build process, as we will no longer use it.

## Verification Plan

### Automated Tests
- N/A for UI visual changes.

### Manual Verification
- Compile and run the application.
- Verify the layout structurally matches the 4-panel dashboard + sidebar + topbar layout.
- Verify the styling matches the "Brutalist" aesthetic (thick black borders, blue/red accents, monospace fonts).
- Verify the "INITIALIZE" button attempts to open the Serial Port and the output shows in the dark terminal section.
