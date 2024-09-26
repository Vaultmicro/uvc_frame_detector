#include "utils/verbose.hpp"
#include <iostream>
#include "utils/verbose.hpp"


// Define the global verbose level
int verbose_level = 0;

// Verbose level 1: Crucial output using cout
void v_cout_1(const std::string& message) {
    if (verbose_level >= 1) {
        std::cout << "CRUCIAL: " << message << std::endl;
    }
}

// Verbose level 1: Crucial error output using cerr
void v_cerr_1(const std::string& message) {
    if (verbose_level >= 1) {
        std::cerr << "CRUCIAL ERROR: " << message << std::endl;
    }
}

// Verbose level 2: Basic output using cout
void v_cout_2(const std::string& message) {
    if (verbose_level >= 2) {
        std::cout << "BASIC: " << message << std::endl;
    }
}

// Verbose level 2: Basic error output using cerr
void v_cerr_2(const std::string& message) {
    if (verbose_level >= 2) {
        std::cerr << "BASIC ERROR: " << message << std::endl;
    }
}

// Verbose level 3: Detailed output using cout
void v_cout_3(const std::string& message) {
    if (verbose_level >= 3) {
        std::cout << "DETAIL: " << message << std::endl;
    }
}

// Verbose level 3: Detailed error output using cerr
void v_cerr_3(const std::string& message) {
    if (verbose_level >= 3) {
        std::cerr << "DETAIL ERROR: " << message << std::endl;
    }
}

// Verbose level 4: Dev output using cout
void v_cout_4(const std::string& message) {
    if (verbose_level >= 4) {
        std::cout << "DEV: " << message << std::endl;
    }
}

// Verbose level 4: Dev error output using cerr
void v_cerr_4(const std::string& message) {
    if (verbose_level >= 4) {
        std::cerr << "DEV ERROR: " << message << std::endl;
    }
}

// Verbose level 5: All output using cout
void v_cout_5(const std::string& message) {
    if (verbose_level >= 5) {
        std::cout << "ALL: " << message << std::endl;
    }
}

// Verbose level 5: All error output using cerr
void v_cerr_5(const std::string& message) {
    if (verbose_level >= 5) {
        std::cerr << "ALL ERROR: " << message << std::endl;
    }
}
