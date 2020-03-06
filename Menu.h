// EasiBuild Mk 2 Firmware: menu framework
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>

#ifndef _MENU_H_
#define _MENU_H_

// Menu types
class Menu {
public:
    void refreshDisplay();
    char *name;
};

class ExecuteMenu: Menu {
public:
    char *actionName; // reset, edit, tune
};

template <class T> class ValueMenu: Menu {
public:
    T getStoredValue();
    void storeUpdatedValue(T value);
    void executeChoice(T value);
};

class NumberedList: ValueMenu<uint8_t> {
public:
    uint8_t choiceCount;
    char *choices[];
};

// A byte-numbered list of choices. Used for the top-level menus, filter choice, key/paddle choice, paddle reversed
// Double range with optional end stops lo-stop, [lo..hi], hi-stop. (CW delay) with unit
// Int range [lo..hi] (CW filter frequency) with unit
// Macro editing
// Execute (press button to send macros, show firmware version, reset after confirmation, tune)

#endif

