#include <httplib.h>
#include <iostream>

int main() {
    httplib::Server svr;
    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("ok", "text/plain");
    });
    std::cout << "http://127.0.0.1:8080/health\n";
    svr.listen("127.0.0.1", 8080);
}
