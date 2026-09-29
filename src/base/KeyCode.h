//
// Created by csis on 2026/9/29.
//

#pragma once

/* Printable keys */
#define AZ_KEY_SPACE              32
#define AZ_KEY_APOSTROPHE         39  /* ' */
#define AZ_KEY_COMMA              44  /* , */
#define AZ_KEY_MINUS              45  /* - */
#define AZ_KEY_PERIOD             46  /* . */
#define AZ_KEY_SLASH              47  /* / */
#define AZ_KEY_0                  48
#define AZ_KEY_1                  49
#define AZ_KEY_2                  50
#define AZ_KEY_3                  51
#define AZ_KEY_4                  52
#define AZ_KEY_5                  53
#define AZ_KEY_6                  54
#define AZ_KEY_7                  55
#define AZ_KEY_8                  56
#define AZ_KEY_9                  57
#define AZ_KEY_SEMICOLON          59  /* ; */
#define AZ_KEY_EQUAL              61  /* = */
#define AZ_KEY_A                  65
#define AZ_KEY_B                  66
#define AZ_KEY_C                  67
#define AZ_KEY_D                  68
#define AZ_KEY_E                  69
#define AZ_KEY_F                  70
#define AZ_KEY_G                  71
#define AZ_KEY_H                  72
#define AZ_KEY_I                  73
#define AZ_KEY_J                  74
#define AZ_KEY_K                  75
#define AZ_KEY_L                  76
#define AZ_KEY_M                  77
#define AZ_KEY_N                  78
#define AZ_KEY_O                  79
#define AZ_KEY_P                  80
#define AZ_KEY_Q                  81
#define AZ_KEY_R                  82
#define AZ_KEY_S                  83
#define AZ_KEY_T                  84
#define AZ_KEY_U                  85
#define AZ_KEY_V                  86
#define AZ_KEY_W                  87
#define AZ_KEY_X                  88
#define AZ_KEY_Y                  89
#define AZ_KEY_Z                  90
#define AZ_KEY_LEFT_BRACKET       91  /* [ */
#define AZ_KEY_BACKSLASH          92  /* \ */
#define AZ_KEY_RIGHT_BRACKET      93  /* ] */
#define AZ_KEY_GRAVE_ACCENT       96  /* ` */
#define AZ_KEY_WORLD_1            161 /* non-US #1 */
#define AZ_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define AZ_KEY_ESCAPE             256
#define AZ_KEY_ENTER              257
#define AZ_KEY_TAB                258
#define AZ_KEY_BACKSPACE          259
#define AZ_KEY_INSERT             260
#define AZ_KEY_DELETE             261
#define AZ_KEY_RIGHT              262
#define AZ_KEY_LEFT               263
#define AZ_KEY_DOWN               264
#define AZ_KEY_UP                 265
#define AZ_KEY_PAGE_UP            266
#define AZ_KEY_PAGE_DOWN          267
#define AZ_KEY_HOME               268
#define AZ_KEY_END                269
#define AZ_KEY_CAPS_LOCK          280
#define AZ_KEY_SCROLL_LOCK        281
#define AZ_KEY_NUM_LOCK           282
#define AZ_KEY_PRINT_SCREEN       283
#define AZ_KEY_PAUSE              284
#define AZ_KEY_F1                 290
#define AZ_KEY_F2                 291
#define AZ_KEY_F3                 292
#define AZ_KEY_F4                 293
#define AZ_KEY_F5                 294
#define AZ_KEY_F6                 295
#define AZ_KEY_F7                 296
#define AZ_KEY_F8                 297
#define AZ_KEY_F9                 298
#define AZ_KEY_F10                299
#define AZ_KEY_F11                300
#define AZ_KEY_F12                301
#define AZ_KEY_F13                302
#define AZ_KEY_F14                303
#define AZ_KEY_F15                304
#define AZ_KEY_F16                305
#define AZ_KEY_F17                306
#define AZ_KEY_F18                307
#define AZ_KEY_F19                308
#define AZ_KEY_F20                309
#define AZ_KEY_F21                310
#define AZ_KEY_F22                311
#define AZ_KEY_F23                312
#define AZ_KEY_F24                313
#define AZ_KEY_F25                314
#define AZ_KEY_KP_0               320
#define AZ_KEY_KP_1               321
#define AZ_KEY_KP_2               322
#define AZ_KEY_KP_3               323
#define AZ_KEY_KP_4               324
#define AZ_KEY_KP_5               325
#define AZ_KEY_KP_6               326
#define AZ_KEY_KP_7               327
#define AZ_KEY_KP_8               328
#define AZ_KEY_KP_9               329
#define AZ_KEY_KP_DECIMAL         330
#define AZ_KEY_KP_DIVIDE          331
#define AZ_KEY_KP_MULTIPLY        332
#define AZ_KEY_KP_SUBTRACT        333
#define AZ_KEY_KP_ADD             334
#define AZ_KEY_KP_ENTER           335
#define AZ_KEY_KP_EQUAL           336
#define AZ_KEY_LEFT_SHIFT         340
#define AZ_KEY_LEFT_CONTROL       341
#define AZ_KEY_LEFT_ALT           342
#define AZ_KEY_LEFT_SUPER         343
#define AZ_KEY_RIGHT_SHIFT        344
#define AZ_KEY_RIGHT_CONTROL      345
#define AZ_KEY_RIGHT_ALT          346
#define AZ_KEY_RIGHT_SUPER        347
#define AZ_KEY_MENU               348

#define AZ_KEY_LAST               AZ_KEY_MENU

/*! @} */

/*! @defgroup mods Modifier key flags
 *  @brief Modifier key flags.
 *
 *  See [key input](@ref input_key) for how these are used.
 *
 *  @ingroup input
 *  @{ */

/*! @brief If this bit is set one or more Shift keys were held down.
 *
 *  If this bit is set one or more Shift keys were held down.
 */
#define AZ_MOD_SHIFT           0x0001
/*! @brief If this bit is set one or more Control keys were held down.
 *
 *  If this bit is set one or more Control keys were held down.
 */
#define AZ_MOD_CONTROL         0x0002
/*! @brief If this bit is set one or more Alt keys were held down.
 *
 *  If this bit is set one or more Alt keys were held down.
 */
#define AZ_MOD_ALT             0x0004
/*! @brief If this bit is set one or more Super keys were held down.
 *
 *  If this bit is set one or more Super keys were held down.
 */
#define AZ_MOD_SUPER           0x0008
/*! @brief If this bit is set the Caps Lock key is enabled.
 *
 *  If this bit is set the Caps Lock key is enabled and the @ref
 *  AZ_LOCK_KEY_MODS input mode is set.
 */
#define AZ_MOD_CAPS_LOCK       0x0010
/*! @brief If this bit is set the Num Lock key is enabled.
 *
 *  If this bit is set the Num Lock key is enabled and the @ref
 *  AZ_LOCK_KEY_MODS input mode is set.
 */
#define AZ_MOD_NUM_LOCK        0x0020