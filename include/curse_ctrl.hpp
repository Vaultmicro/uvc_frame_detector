#ifndef INIT_SCR_HPP
#define INIT_SCR_HPP

#define WIN32_LEAN_AND_MEAN
#include <windows.h> // For Sleep function

#ifdef MOUSE_MOVED
#undef MOUSE_MOVED
#endif

#include "cursed_output.hpp"

// Extern declaration for the global running flag
extern bool running; 

// Function declarations
void handle_sigint(int sig);
void init_scr();
void initialize_windows();
void finish_windows();

#endif // INIT_SCR_HPP
