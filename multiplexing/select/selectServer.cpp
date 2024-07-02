#include "Socket.hpp"
#include "Select.hpp"

int main() {
    Select server(8888);

    server.Loop();

    return 0;
}
