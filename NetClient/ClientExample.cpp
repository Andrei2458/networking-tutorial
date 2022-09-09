// #include "../NetCommon/olc_net.hpp" 
// #include <iostream>

// enum class CustomMsgTypes : uint32_t {
//     FireBullet,
//     MovePlayer
// };


// int main() {
//     olc::net::message<CustomMsgTypes> msg;
//     // ids can only be from the CustomMsgTypes enum class
//     // error: 
//     // msg.header.id = 8;
//     msg.header.id = CustomMsgTypes::FireBullet;

//     int a = 1;
//     bool b = true;
//     float c = 3.14159f;

//     std::cout << "a:" << a << " b:" << b << " c:" << c << "\n";

//     // put everything in our message

//     msg << a << b << c;

//     // overwrites the initial values
//     a = 99;
//     b = false;
//     c = 99.0f;

//     std::cout << "a:" << a << " b:" << b << " c:" << c << "\n";

//     msg >> c >> b >> a;

//     std::cout << "a: " << a << " b: " << b << " c: " << c << "\n";

//     // std::cout << msg;

//     return 0;
// }
