#include <iostream>
#include <pthread.h>
#include <ncurses.h>
#include <string>
#include <vector>
#include <utility> // Not sure if everything here will still be necessary
#include <unistd.h>
#include <limits>
#include <sstream>

using namespace std;

class Song { // Same implementation as previous version
    string name;
    int duration; // em segundos
    int playback_time; // -1 significa que a música não está tocando
public:
    // Here be constructors
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

    // Here be getters
    string get_name() {
        return name;
    }
    int get_duration() {
        return duration;
    }
    int get_playback_time() {
        return playback_time;
    }
    string get_formated_duration() { // More of a formating function than a getter but anyway
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
    // Here be setters
    void set_playback_time(int seconds) { // será usado em versões futuras << haha yes lets goooooo
        playback_time = seconds;
    }
};

vector<Song> fake_folder_init() { // TODO music this up
    vector<Song> library;
    library.push_back(Song("test1", 31));
    library.push_back(Song("test2", 32));
    library.push_back(Song("test3", 33));
    library.push_back(Song("test4", 34));
    library.push_back(Song("test5", 35));
    return library;
}

int main() {
    vector<Song> biblioteca;
    biblioteca = fake_folder_init(); // initializes the fake folder the songs will come from
    // TODO implement main menu with ncurses
        //TODO implement library window
        // TODO implement playlist window
            // TODO implement visual progress bar
        // TODO implement control help window
        // TODO implement player window
    // TODO implement keyboard reading thread
    // TODO implement quit feature
    // TODO implement playlist vector
    // TODO implement adding songs
    // TODO implement removing songs
    // TODO implement progress bar thread
    // TODO implement play feature
    // TODO implement pause feature
    // TODO implement skip song feature
    return 0;
}