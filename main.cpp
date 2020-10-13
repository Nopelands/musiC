#include <iostream>
#include <pthread.h>
#include <ncurses.h>
#include <string>
#include <utility>
#include <unistd.h>
#include <cstdlib>
#include <list>
#include <iterator>
#include <limits>
using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier;
string key;

class Song { // TODO comment
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

void *keyboard(void *arg) { // TODO comment
    string input;
    while (true) {
        std::cin >> input;
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
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

int main() { // TODO comment and add removal feature
    bool exit = false;
    pthread_t ler_teclado;
    pthread_create(&ler_teclado, nullptr, &keyboard, nullptr);
    pthread_barrier_init(&barrier, nullptr, 2);
    list<Song> songs;
    std::cout << "Welcome to musiC++!" << std::endl;
    sleep(5);
//    system("clear");  // uncomment when running in terminal
    while (!exit) {
        if (songs.empty()) {
            std::cout << R"(Your queue is empty, type "a" to add a song or "q" to quit)" << std::endl;
        } else {
            std::cout << "Now Playing:" << std::endl;
            std::cout << songs.front().get_name() << std::endl;
            std::cout << "0:00 [----------] " + songs.front().get_formated_duration() << std::endl;
            if (songs.size() > 1) {
                std::cout << "Playing Next:" << std::endl;
                auto it = std::begin(songs);
                ++it;
                for (auto end=std::end(songs); it!=end; ++it) {
                    std::cout << it->get_name() + " " + it->get_formated_duration() << std::endl;
                }
            }
            std::cout << R"(Type "q" to quit, "a" to add a song, and "r" to remove a song)" << std::endl;
        }
        sleep(2);
        while (pthread_mutex_trylock(&mutex));
        if (key == "q") {
            exit = true;
        } else if (key == "a") {
            string song_name;
            int song_duration;
            std::cout << "Type the name of the song" << std::endl;
            getline(cin, song_name);
            std::cout << "Type the duration of the song (in seconds)" << std::endl;
            std::cin >> song_duration;
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            songs.push_back(Song(song_name, song_duration));
            pthread_barrier_wait(&barrier);
        }
        key = "nope";
        pthread_mutex_unlock(&mutex);
//        system("clear");  // uncomment when running in terminal
    }
    return 0;
}
