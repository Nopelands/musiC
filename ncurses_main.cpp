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
    library.push_back(Song("test1.mp3", 31));
    library.push_back(Song("test2.mp3", 32));
    library.push_back(Song("test3.mp3", 33));
    library.push_back(Song("test4.mp3", 34));
    library.push_back(Song("test5.mp3", 35));
    return library;
}

int main() {
    vector<Song> biblioteca;
    biblioteca = fake_folder_init(); // initializes the fake folder the songs will come from
    vector<Song> playlist;

    initscr();
    noecho();
    int max_screen_size_y;
    int max_screen_size_x;
    getmaxyx(stdscr, max_screen_size_y, max_screen_size_x);

    WINDOW *library_window = newwin(max_screen_size_y, max_screen_size_x/3, 0, 0);
    WINDOW *playlist_window = newwin(3*(max_screen_size_y/4), 2*(max_screen_size_x/3), 0, max_screen_size_x/3);
    WINDOW *control_help_window = newwin(1, max_screen_size_x - max_screen_size_x/3, max_screen_size_y-1, max_screen_size_x/3);
    WINDOW *player_window = newwin((max_screen_size_y - 1) - (3*(max_screen_size_y/4)), 2*(max_screen_size_x/3), 3*(max_screen_size_y/4), max_screen_size_x/3);
    refresh();

    box(library_window, '|', '-');
    wmove(library_window, 1, 1);
    wprintw(library_window, "/home/user/Music");
    for (int i = 0; i < biblioteca.size(); ++i) {
        wmove(library_window, i + 2, 1);
        wprintw(library_window, (to_string(i+1) + " " + biblioteca[i].get_name()).data());
    }
    wrefresh(library_window);

    box(playlist_window, '|', '-');
    wrefresh(playlist_window);

    wprintw(control_help_window, " LOREM IPSUM DOLOR SIT AMET");
    wrefresh(control_help_window);

    box(player_window, '|', '-');
    wrefresh(player_window);

    // window variables

    bool playlist_window_needs_refresh = true;

    // main loop

    playlist.push_back(biblioteca[0]);
    playlist.push_back(biblioteca[2]);

    while (true) {
        if (playlist.empty() and playlist_window_needs_refresh) {
            wclear(playlist_window);
            box(playlist_window, '|', '-');
            wmove(playlist_window, 1, 1);
            wprintw(playlist_window, "Your playlist is empty, press a to add a song");
            wrefresh(playlist_window);
            getch();
        } else if (playlist_window_needs_refresh) {
            wclear(playlist_window);
            box(playlist_window, '|', '-');
            wmove(playlist_window, 1, 1);
            wprintw(playlist_window, ("> " + playlist[0].get_name() + " " + playlist[0].get_formated_duration() + " <").data());
            for (int i = 1; i < playlist.size(); ++i) {
                wmove(playlist_window, i + 1, 3);
                wprintw(playlist_window, (playlist[i].get_name() + " " + playlist[i].get_formated_duration()).data());
            }
            wrefresh(playlist_window);
            getch();
        }
        break;
    }

    getch();
    endwin();
    // TODO implement main menu with ncurses
        // TODO implement control help window
        // TODO implement player window
            // TODO implement visual progress bar
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