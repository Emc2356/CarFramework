#include <Car/Car>

#include <exception>
#include <ostream>

int main() {
    Car::Application* app = Car::createApplication();

    // deleting application isnt 100% needed as the
    // OS will be able to delete and recoup the memory
    // much faster then what the program can delete it
    // but it is helpfull for detecting memory leaks

    try {
        app->run();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;

        delete app;

        return 1;
    }
    delete app;

    return 0;
}
