// // #include <iostream>
// // #include <stdio.h>

// // int main() {
// //     std::cout << "Press any key to continue...\n";

// //     // Set terminal to raw mode 
// //     system("stty raw");

// //     // Wait for a single character
// //     char input = getchar();

// //     // Echo input
// //     std::cout << "--" << input << "--\n";

// //     // Reset terminal to moral "cooked" mode
// //     system("stty cooked");

// //     // And out...
// //     return 0;
// // }

// #include <unistd.h>
// #include <termios.h>
// #include <stdio.h>
// #include <iostream>

// char getch() {
//         char buf = 0;
//         struct termios old = {0};
//         if (tcgetattr(0, &old) < 0) {
//             perror("tcsetattr()");
//             }
//         old.c_lflag &= ~ICANON;
//         old.c_lflag &= ~ECHO;
//         old.c_cc[VMIN] = 1;
//         old.c_cc[VTIME] = 0;
//         if (tcsetattr(0, TCSANOW, &old) < 0)
//                 perror("tcsetattr ICANON");
//         if (read(0, &buf, 1) < 0)
//                 perror ("read()");
//         old.c_lflag |= ICANON;
//         old.c_lflag |= ECHO;
//         if (tcsetattr(0, TCSADRAIN, &old) < 0)
//                 perror ("tcsetattr ~ICANON");
//         return (buf);
// }

// int main() {
//     std::cout << "Press any key to continue...\n";
//     auto input = getch();
//     std::cout << "you pressed: " << input << "\n";

// }
