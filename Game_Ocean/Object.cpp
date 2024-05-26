#include <Object.h>

void Stone::update() override {
    if (to_reef == 0) {
        symbol = "&";
    }
    else
        to_reef--;
}
std::string Stone::getSymbol(symbol) { return symbol; }