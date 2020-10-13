#include <iostream>
#include <pthread.h>
#include <ncurses.h>
#include <string>
#include <utility>
#include <unistd.h>
#include <cstdlib>
using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
string key;

class Song {
    string name;
    int duration; // in seconds
    int playback_time; // -1 means the song is not playing
public:
    Song(string song_name, int seconds) {
        name = std::move(song_name);
        duration = seconds;
        playback_time = -1;
    }

    string get_name() {
        return name;
    }

    int get_duration() {
        return duration;
    }

    string get_formated_duration() {
        int minutes = duration / 60;
        int seconds = duration % 60;
        return to_string(minutes) + ":" + to_string(seconds);
    }

    void set_playback_time(int seconds) {
        playback_time = seconds;
    }

    int get_playback_time() {
        return playback_time;
    }
};

void *keyboard(void *arg) {
    string input;
    while (true) {
        std::cin >> input;
        while (pthread_mutex_trylock(&mutex));
        key = input;
        pthread_mutex_unlock(&mutex);
        if (key == "q") {
            pthread_exit(nullptr);
        }
    }
}

int main() {
    bool exit = false;
    pthread_t ler_teclado;
    pthread_create(&ler_teclado, nullptr, &keyboard, nullptr);
    std::cout << "Welcome to musiC++!" << std::endl;
    std::cout << "type q to quit" << std::endl;
    while (!exit) {
        std::cout << "____________________" << std::endl;
        std::cout << "This is a menu loop!" << std::endl;
        std::cout << "____________________" << std::endl;
        sleep(2);
        while (pthread_mutex_trylock(&mutex));
        if (key == "q") {
            exit = true;
        }
        key = "nope";
        pthread_mutex_unlock(&mutex);
//        system("clear");  // uncomment when running in terminal
    }
    return 0;
}
