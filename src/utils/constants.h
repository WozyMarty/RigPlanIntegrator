#pragma once

#include <cstdint>

namespace Constants {

// Physical constants
constexpr double GRAVITY         = 9.80665;     // m/s²
constexpr double AIR_DENSITY     = 1.225;       // kg/m³
constexpr double PI              = 3.14159265358979323846;
constexpr double DEG_TO_RAD      = PI / 180.0;
constexpr double RAD_TO_DEG      = 180.0 / PI;

// Temperature conversion
constexpr double CELSIUS_TO_F_FACTOR = 9.0 / 5.0;
constexpr double CELSIUS_TO_F_OFFSET = 32.0;

// Application defaults
constexpr int DEFAULT_CALC_BLOCKS    = 16;
constexpr int MAX_PRESET_COUNT       = 50;
constexpr int PREVIEW_UPDATE_DELAY   = 300; // ms debounce

// Validation limits
constexpr double MIN_VALID_VALUE     = -1.0e15;
constexpr double MAX_VALID_VALUE     =  1.0e15;
constexpr double EPSILON             =  1.0e-10;

// UI dimensions
constexpr int HEADER_HEIGHT          = 65;
constexpr int CALC_PANEL_MIN_WIDTH   = 340;
constexpr int CALC_PANEL_MAX_WIDTH   = 500;
constexpr int CALC_BLOCK_MIN_HEIGHT  = 60;
constexpr int PRESET_CARD_WIDTH      = 200;
constexpr int PRESET_CARD_HEIGHT     = 260;

// Export
constexpr int PDF_DPI                = 300;

// Colors (as hex integer for programmatic use)
constexpr uint32_t COLOR_HEADER_START   = 0xB71C1C;
constexpr uint32_t COLOR_HEADER_END     = 0xE53935;
constexpr uint32_t COLOR_PANEL_START    = 0xE65100;
constexpr uint32_t COLOR_PANEL_END      = 0xFF8F00;
constexpr uint32_t COLOR_ACCENT_BLUE    = 0x1F6FEB;
constexpr uint32_t COLOR_SUCCESS_GREEN  = 0x3FB950;
constexpr uint32_t COLOR_WARNING_YELLOW = 0xD29922;
constexpr uint32_t COLOR_ERROR_RED      = 0xF85149;
constexpr uint32_t COLOR_BG_DARK        = 0x0D1117;
constexpr uint32_t COLOR_SURFACE        = 0x161B22;
constexpr uint32_t COLOR_CARD           = 0x1C2333;

} // namespace Constants
