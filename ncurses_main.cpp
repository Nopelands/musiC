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

int main() {
    // TODO implement main menu with ncurses
    // TODO implement keyboard reading thread
    // TODO implement quit feature
    // TODO implement playlist vector
    // TODO implement fake folder for adding songs from
    // TODO implement adding songs
    // TODO implement removing songs
    // TODO implement progress bar thread
    // TODO implement visual progress bar
    // TODO implement play feature
    // TODO implement pause feature
    // TODO implement skip song feature
    return 0;
}