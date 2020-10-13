#include <iostream>
#include <pthread.h>
#include <ncurses.h>
#include <string>
#include <utility>
#include <unistd.h>
#include <cstdlib>
#include <list>
#include <iterator>
using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier;
string key;

class Song {
    string name;
    int duration; // in seconds
    int playback_time; // -1 means the song is not playing
public:
    Song() {
        name = "";
        duration = 0;
        playback_time = -1;
    }
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
        } else {
            pthread_barrier_wait(&barrier);
        }
    }
}

int main() {
    bool exit = false;
    pthread_t ler_teclado;
    pthread_create(&ler_teclado, nullptr, &keyboard, nullptr);
    pthread_barrier_init(&barrier, nullptr, 2);
    list<Song> songs;
    std::cout << "Welcome to musiC++!" << std::endl;
    std::cout << "type q to quit" << std::endl;
    sleep(5);
//    system("clear");  // uncomment when running in terminal
    while (!exit) {
        if (songs.empty()) {
            std::cout << R"(Your queue is empty, type "a" to add a song or "q" to quit)" << std::endl;
        }
        sleep(2);
        while (pthread_mutex_trylock(&mutex));
        if (key == "q") {
            exit = true;
        } else if (key == "a") {
            std::cout << "stuff being done" << std::endl;
            pthread_barrier_wait(&barrier);
        }
        key = "nope";
        pthread_mutex_unlock(&mutex);
//        system("clear");  // uncomment when running in terminal
    }
    return 0;
}
