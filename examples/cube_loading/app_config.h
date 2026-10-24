// Application constants, no demo test selectors.
APP_ROTATION => 0;
APP_HOLD_CYCLES => 200000; // Per 5% step, including full/empty; not calibrated.

// Derived application layout; preserve all four compile-time rotations.
#IF APP_ROTATION > 2
APP_PORTRAIT => 1;
#ELSE
#IF APP_ROTATION > 1
APP_PORTRAIT => 0;
#ELSE
#IF APP_ROTATION > 0
APP_PORTRAIT => 1;
#ELSE
APP_PORTRAIT => 0;
#ENDIF
#ENDIF
#ENDIF
