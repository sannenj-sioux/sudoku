#ifndef _SUDOKU_UTILITY_INL_
#define _SUDOKU_UTILITY_INL_

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

// Return a random number in [begin, end].
inline unsigned int RandomInt(int begin, int end)
{
    std::random_device random_device;
    std::mt19937 generator(random_device());
    return std::uniform_int_distribution<unsigned int>(begin, end)(generator);
}

inline std::vector<int> GetUnit() {
    std::vector<int> unit;
    unit.reserve(9);
    for (int value = 1; value <= 9; ++value) {
        unit.push_back(value);
    }
    return unit;
}

inline std::vector<int> ShuffleUnit() {
    std::vector<int> unit = GetUnit();
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::shuffle(unit.begin(), unit.end(), generator);
    return unit;
}

inline void Message(const char* msg = "", bool add_line_feed = true)
{
    std::cout << msg;
    if (add_line_feed) {
        std::cout << '\n';
    }
}

inline void Message(const std::string& msg, bool add_line_feed = true) {
  Message(msg.c_str(), add_line_feed);
}

#ifdef _WIN32
#include <conio.h>
#else
    #ifdef __linux__
    #include <termio.h>
    #include <cstdio>
    #include <fcntl.h>
    #elif __APPLE__
    #include <termios.h>
    #include <fcntl.h>
    #endif
    inline char getch(void)
    {
        struct termios tmtemp, tm;
        int c;
        int fd = 0;
        if (tcgetattr(fd, &tm) != 0)
        { /*Get current terminal attributes and store them in `tm`*/
            return -1;
        }
        tmtemp = tm;
        cfmakeraw(&tmtemp); /*Initialize `tmtemp` with raw terminal mode settings*/
        if (tcsetattr(fd, TCSANOW, &tmtemp) != 0)
        { /*Apply raw mode settings to terminal*/
            return -1;
        }
        c = getchar();
        if (c == 27)  /* ESC is 27; arrow keys are (27,91,xx), which conflicts with ESC */
        {
            int back_flags = fcntl(fd, F_GETFL);
            /*Set fd to non-blocking so it returns immediately when there is no input*/
            fcntl(fd, F_SETFL, back_flags | O_NONBLOCK);
            c = getchar();
            if (c == EOF)
            {
                c = 27; 
            }
            else
            {
                while ( (c = getchar()) != EOF) 
                {
                }
            }
            fcntl(fd, F_SETFL, back_flags);
        }
        if (tcsetattr(fd, TCSANOW, &tm) != 0)
        { /*Restore terminal attributes after reading a character*/
            return 0;
        }
        return static_cast<char>(c);
    }

    inline char _getch() { return getch(); }
#endif

inline void ClearScreen(void)
{
    // Clear screen using ANSI escape sequence; faster and less flicker than system("cls")
    std::cout << "\033[H\033[J";
}

#endif
