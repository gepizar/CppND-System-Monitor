#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    string HH, MM, SS; 
    int H, M, S; 
    H = seconds / 3600; 
    M = (seconds % 3600) / 60; 
    S = seconds % 60; 

    HH = std::to_string(H); 
    MM = std::to_string(M); 
    SS = std::to_string(S); 

    if (S < 10) {
        SS = "0" + SS; 
    }

    if (M < 10) {
        MM = "0" + MM; 
    }

    if (H < 10) {
        HH = "0" + HH; 
    }

    return HH + ":" + MM + ":" + SS; 
}