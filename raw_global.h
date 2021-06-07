#ifndef RAW_GLOBAL_H
#define RAW_GLOBAL_H

#define SET_LANG_RU
#define DEFAULT_URL "http://localhost:6677/"
#define AUTH_DELAY 2000

// DEBUG
#define DEBUG_TOKEN "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJ1c2VybmFtZSI6ImFkbWluIn0.e3UwvG12weaHaVWZ2u-vuH1SkOb6Ee0NFMVJGtTgwio"
#define DEBUG_AUTO_FILES

// Paths
#define AUTH_PATH "auth"
#define GEMM_PATH "gemm"
#define GEMV_PATH "gemv"

// Main Window
#define PREVIEW_DIM_ROWS 20
#define PREVIEW_DIM_COLS 20

// DEBUG HIDDEN
#ifdef DEBUG_AUTO_FILES
#define DEBUG_FILE_A "C:\\Users\\login\\Documents\\512x8.txt"
#define DEBUG_FILE_B "C:\\Users\\login\\Documents\\8x512.txt"
#define DEBUG_FILE_C "C:\\Users\\login\\Documents\\512x512.txt"
#define DEBUG_FILE_A2 "C:\\Users\\login\\Documents\\1024x512.txt"
#define DEBUG_FILE_x "C:\\Users\\login\\Documents\\1x512.txt"
#define DEBUG_FILE_y "C:\\Users\\login\\Documents\\1x1024.txt"
#else // DEBUG_NO_FILES
#define DEBUG_FILE_A nullptr
#define DEBUG_FILE_B nullptr
#define DEBUG_FILE_C nullptr
#define DEBUG_FILE_A2 nullptr
#define DEBUG_FILE_x nullptr
#define DEBUG_FILE_y nullptr
#endif // DEBUG_NO_FILES

#endif // RAW_GLOBAL_H
