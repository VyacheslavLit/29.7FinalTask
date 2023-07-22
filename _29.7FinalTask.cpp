#include "Queue.h"

using namespace std;

int main() {
    FineGrainedQueue FGQ;
    std::thread FGQ1([&]() {FGQ.push_back(15); });
    std::thread FGQ2([&]() {FGQ.push_back(18); });
    std::thread FGQ3([&]() {FGQ.push_back(5); });
    std::thread FGQ4([&]() {FGQ.push_back(12); });
    FGQ.push_back(25);
    if (FGQ1.joinable())FGQ1.join();
    if (FGQ2.joinable())FGQ2.join();
    if (FGQ3.joinable())FGQ3.join();
    if (FGQ4.joinable())FGQ4.join();
    FGQ.show();

    std::thread FGQi1([&]() {FGQ.insertIntoMiddle(0, 85); });
    std::thread FGQi2([&]() {FGQ.insertIntoMiddle(2, 16); });
    std::thread FGQi3([&]() {FGQ.insertIntoMiddle(4, 10); });
    FGQ.insertIntoMiddle(1, 100);
    if (FGQi1.joinable())FGQi1.join();
    if (FGQi2.joinable())FGQi2.join();
    if (FGQi3.joinable())FGQi3.join();
    FGQ.show();

    std::thread FGQr1([&]() {FGQ.remove(25); });
    std::thread FGQr2([&]() {FGQ.remove(16); });
    std::thread FGQr3([&]() {FGQ.remove(12); });
    FGQ.remove(18);
    if (FGQr1.joinable())FGQr1.join();
    if (FGQr2.joinable())FGQr2.join();
    if (FGQr3.joinable())FGQr3.join();
    FGQ.show();

    return 0;
}