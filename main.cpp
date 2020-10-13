#include <iostream>
#include <pthread.h>
#include <ncurses.h>
#include <string>
#include <utility>
#include <unistd.h>
#include <vector>
#include <limits>
#include <sstream>
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
        string formated_minutes;
        string formated_seconds;
        if (minutes < 10) {
            formated_minutes = "0" + to_string(minutes);
        } else {
            formated_minutes = to_string(minutes);
        }
        if (seconds < 10) {
            formated_seconds = "0" + to_string(seconds);
        } else {
            formated_seconds = to_string(seconds);
        }
        return formated_minutes + ":" + formated_seconds;
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
        bool quit_signal = input == "q";
        pthread_mutex_unlock(&mutex);
        if (quit_signal) {
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
    vector<Song> songs;
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
                for (int i = 1; i < songs.size(); ++i) {
                    std::cout << songs[i].get_name() + " " + songs[i].get_formated_duration() << std::endl;
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
            string song_duration;
//            system("clear");  // uncomment when running in terminal
            std::cout << "Type the name of the song" << std::endl;
            getline(cin, song_name);
//            system("clear");  // uncomment when running in terminal
            std::cout << "Type the duration of the song (in seconds)" << std::endl;
            std::cin >> song_duration;
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            if (song_duration.find_first_not_of( "0123456789" ) == std::string::npos) {
                stringstream stream_song_duration(song_duration);
                int int_song_duration;
                stream_song_duration >> int_song_duration;
                songs.push_back(Song(song_name, int_song_duration));
            } else {
                std::cout << "Input is not a number" << std::endl;
            }
            pthread_barrier_wait(&barrier);
        } else if (key == "r") {
//            system("clear");  // uncomment when running in terminal
            std::cout << "Type the number of the song you wish to remove:" << std::endl;
            for (int i = 0; i < songs.size(); ++i) {
                std::cout << to_string(i+1) + " " + songs[i].get_name() << std::endl;
            }
            int to_remove;
            std::cin >> to_remove;
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            to_remove = to_remove - 1;
            if (to_remove < 0 or to_remove >= songs.size()) {
                std::cout << "Song not found" << std::endl;
                sleep(2);
//                system("clear"); // uncomment when running in terminal
            } else {
                songs.erase(songs.begin() + to_remove);
            }
            pthread_barrier_wait(&barrier);
        }
        key = "nope";
        pthread_mutex_unlock(&mutex);
//        system("clear");  // uncomment when running in terminal
    }
    return 0;
}
