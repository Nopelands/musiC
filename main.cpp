#include <iostream>
#include <pthread.h>
#include <ncurses.h>
#include <string>
#include <utility>
using namespace std;

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

int main() {
    bool exit = false;
    string key;
    while (!exit) {
        std::cout << "Welcome to musiC++!" << std::endl;
        std::cout << "type q to quit" << std::endl;
        std::cin >> key;
        if (key == "q") {
            exit = true;
        }
    }
//    string a;
//    getline(cin, a);
//    int b;
//    std::cin >> b;
//    Song song(a, b);
//    std::cout << song.get_playback_time() << std::endl;
//    std::cout << song.get_formated_duration() << std::endl;
    return 0;
}
