#ifndef _SUDOKU_UTILITY_INL_
#define _SUDOKU_UTILITY_INL_

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>

//not real random,return number between [begin,end]
inline unsigned int random(int begin, int end)
{
    std::random_device rd;
    std::mt19937 g(rd());
    return std::uniform_int_distribution<unsigned int>(begin, end)(g);
}

inline std::vector<int> get_unit(){
    return std::vector<int> {1, 2, 3, 4, 5, 6, 7, 8, 9};
}

inline std::vector<int> shuffle_unit(){
    std::vector<int> unit = get_unit();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(unit.begin(), unit.end(), g);
    return unit;
}

inline void message(const char* msg = "", bool lf = true)
{
    std::cout << msg;
    if (lf) std::cout << std::endl;
}

inline void message(const std::string& msg, bool lf = true) {
  message(msg.c_str(), lf);
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

inline void cls(void)
{
    // Clear screen using ANSI escape sequence; faster and less flicker than system("cls")
    std::cout << "\033[H\033[J";
}

#endif
