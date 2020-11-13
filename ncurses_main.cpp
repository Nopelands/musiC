#include <iostream>
#include <pthread.h>
#include <ncurses.h>
#include <string>
#include <vector>
#include <utility>
#include <chrono>
#include <time.h>

using namespace std;

class Song {
    string name;
    int duration; // em segundos
public:
    Song() {
        name = "";
        duration = 0;
    }
    Song(string song_name, int seconds) {
        name = std::move(song_name);
        duration = seconds;
    }

    // Here be getters
    string get_name() {
        return name;
    }
    int get_duration() {
        return duration;
    }
    string get_formated_duration() { // recebe tempo em segundos e retorna no formato mm:ss
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
};

std::chrono::system_clock::rep time_since_epoch(){ // Retorna o tempo em segundos desde a epoch
    static_assert(
            std::is_integral<std::chrono::system_clock::rep>::value,
            "Representation of ticks isn't an integral value."
    );
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(now).count();
}

pthread_mutex_t player_mutex = PTHREAD_MUTEX_INITIALIZER; // mutex das variáveis do player

// variáveis globais do player
bool global_playing_song = false; // true se alguma música está tocando
bool player_reset = false; // true se uma nova música vai começar
int global_epoch_when_song_was_paused = 0;
int global_player_time; // tempo atual da música

void *player(void *arg) { // thread do player de música

    int song_start_time = 0;
    int playback_time_offset = 0;

    while (true) {
        while (pthread_mutex_trylock(&player_mutex)); // pede o mutex para acessar região crítica
        if (global_playing_song) { // verifica se alguma música está tocando
            if (player_reset) { // reseta o player
                song_start_time = time_since_epoch();
                global_player_time = 0;
                playback_time_offset = 0;
                player_reset = false;
            } else { // continua a música atual
                if (global_epoch_when_song_was_paused != 0) { // corrige o tempo de playback da música se ela foi pausada
                    song_start_time = time_since_epoch();
                    playback_time_offset = global_player_time;
                    global_player_time = playback_time_offset + time_since_epoch() - song_start_time;
                    global_epoch_when_song_was_paused = 0;
                } else { // atualiza o tempo de playback da música se ela não foi pausada
                    global_player_time = playback_time_offset + time_since_epoch() - song_start_time;
                }
            }
        }
        pthread_mutex_unlock(&player_mutex); // solta o mutex
    }
}

vector<Song> fake_folder_init() { // inicializa o vector que representa um diretório fictício
    vector<Song> library;
    library.push_back(Song("Every Day Is Night.mp3", 221)); // Time to mix drinks and change lives
    library.push_back(Song("Throw Away Your Mask.mp3", 242)); // A reality where... no one suffers...
    library.push_back(Song("Gentle Madman.mp3", 184)); // I will never let a tragedy like that happen, ever again.
    library.push_back(Song("The Hunter.mp3", 204)); // Beasts all over the shop. You'll be one of them... Sooner or later...
    library.push_back(Song("Nameless King.mp3", 266)); // Nothing will remain once the bell has tolled
    library.push_back(Song("Song of The Ancients.mp3", 300)); // Those two have watched the world wither for time immemorial. The cruelness of such a fate is difficult to imagine.
    library.push_back(Song("Decretum.mp3", 108)); // I was stupid... So stupid...
    library.push_back(Song("Gate of Steiner.mp3", 227)); // Shed a tear and leap to a new world
    library.push_back(Song("Battle Against a True Hero.mp3", 156)); // The wind is howling...
    return library;
}

int main() {
    srand (time(NULL)); // seed para reprodução aleatória
    vector<Song> biblioteca; // representa um diretório com músicas
    biblioteca = fake_folder_init();
    vector<Song> playlist; // a playlist atual
    int keypress;

    //cria o thread do player
    pthread_t player_thread;
    pthread_create(&player_thread, nullptr, &player, nullptr);

    // ncurses setup
    initscr();
    noecho();
    curs_set(0); // esconde o cursor
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    halfdelay(10); // getch retorna ERR depois de 1 segundo
    keypad(stdscr, true);
    int max_screen_size_y;
    int max_screen_size_x;
    getmaxyx(stdscr, max_screen_size_y, max_screen_size_x);

    WINDOW *library_window = newwin(max_screen_size_y, max_screen_size_x/3, 0, 0); // cria a janela do diretório
    WINDOW *playlist_window = newwin(3*(max_screen_size_y/4), 2*(max_screen_size_x/3), 0, max_screen_size_x/3); // cria a janela da playlist
    WINDOW *control_help_window = newwin(1, max_screen_size_x - max_screen_size_x/3, max_screen_size_y-1, max_screen_size_x/3); // cria a janela que mostra os controles
    WINDOW *player_window = newwin((max_screen_size_y - 1) - (3*(max_screen_size_y/4)), 2*(max_screen_size_x/3), 3*(max_screen_size_y/4), max_screen_size_x/3); // cria a janela do player
    refresh();

    //setup da janela do diretório
    box(library_window, 0, 0);
    wmove(library_window, 1, 1);
    wprintw(library_window, "/home/user/Music");
    for (int i = 0; i < biblioteca.size(); ++i) {
        wmove(library_window, i + 2, 1);
        wprintw(library_window, (to_string(i+1) + " " + biblioteca[i].get_name()).data());
    }
    wrefresh(library_window);

    //setup inicial da janela da playlist
    box(playlist_window, 0, 0);
    wmove(playlist_window, 1, 1);
    wprintw(playlist_window, "Loading...");
    wrefresh(playlist_window);

    //setup inicial da janela de controles
//    wbkgd(control_help_window, COLOR_PAIR(1)); // currently bugged
    wprintw(control_help_window, " (add mode enabled) | [r] remove mode | [q] quit | [right arrow key] skip song | [Spacebar] pause/unpause");
    wrefresh(control_help_window);

    //setup inicial da janela do player
    box(player_window, 0, 0);
    wrefresh(player_window);

    // window variables

    bool playlist_window_needs_refresh = true;
    bool playlist_add_mode = true;
    bool random_mode = false;
    int player_window_y_center = ((max_screen_size_y - 1) - (3*(max_screen_size_y/4))) / 2;

    // main loop

    while ((keypress = getch()) != 'q') { // bloqueia por 1 segundo
        switch (keypress) {
            case 32: // SPACEBAR - função de pause/unpause
                while (pthread_mutex_trylock(&player_mutex));
                if (global_playing_song) {
                    global_playing_song = false;
                    global_epoch_when_song_was_paused = time_since_epoch();
                } else {
                    global_playing_song = true;
                }
                pthread_mutex_unlock(&player_mutex);
                break;
            case KEY_RIGHT: // função de skip
                if (not playlist.empty()) {
                    playlist.erase(playlist.begin());
                    playlist_window_needs_refresh = true;
                    if (playlist.empty()) {
                        while (pthread_mutex_trylock(&player_mutex));
                        global_playing_song = false;
                        pthread_mutex_unlock(&player_mutex);
                    } else if (random_mode and playlist.size() != 1) { // se random_mode, escolhe uma música aleatória e a coloca na frente da playlist
                        int next_song = rand() % (playlist.size()-1);
                        playlist.insert(playlist.begin(), playlist[next_song]);
                        playlist.erase(playlist.begin() + next_song + 1);
                    }
                    while (pthread_mutex_trylock(&player_mutex));
                    player_reset = true;
                    pthread_mutex_unlock(&player_mutex);
                }
                break;
            case 115: // s - alterna entre o modo sequencial e o modo aleatório
                if (random_mode) {
                    random_mode = false;
                } else {
                    random_mode = true;
                }
                break;
            case 97: // a - ativa o modo de adição de músicas
                playlist_add_mode = true;
                wmove(control_help_window, 0, 0);
                wprintw(control_help_window, " (add mode enabled) | [r] remove mode | [q] quit | [right arrow key] skip song | [Spacebar] pause/unpause");
                wrefresh(control_help_window);
                break;
            case 114: // r - ativa o modo de remoção de músicas
                playlist_add_mode = false;
                wmove(control_help_window, 0, 0);
                wprintw(control_help_window, " (remove mode enabled) | [a] add mode | [q] quit | [right arrow key] skip song | [Spacebar] pause/unpause");
                wrefresh(control_help_window);
                break;
            case 49 ... 57: // 1-9 - adiciona ou remove a música de número especificado
                keypress = keypress - 49; // transforma 1 em 0, 2 em 1 e assim por diante
                if (playlist_add_mode) {
                    if (playlist.empty()) {
                        while (pthread_mutex_trylock(&player_mutex));
                        global_playing_song = true;
                        player_reset = true;
                        pthread_mutex_unlock(&player_mutex);
                    }
                    if (playlist.size() < 9) {
                        playlist.push_back(biblioteca[keypress]);
                        playlist_window_needs_refresh = true;
                    } else {
                        wmove(playlist_window, 11, 1);
                        wprintw(playlist_window, "Playlist limit reached");
                        wrefresh(playlist_window);
                    }
                } else {
                    if (not playlist.empty() and keypress < playlist.size()) {
                        playlist.erase(playlist.begin() + keypress);
                        playlist_window_needs_refresh = true;
                        if (playlist.empty()) {
                            while (pthread_mutex_trylock(&player_mutex));
                            global_playing_song = false;
                            pthread_mutex_unlock(&player_mutex);
                        } else if (keypress == 0) {
                            while (pthread_mutex_trylock(&player_mutex));
                            player_reset = true;
                            pthread_mutex_unlock(&player_mutex);
                        }
                        if (not playlist.empty() and random_mode and playlist.size() != 1) { // se random_mode, escolhe uma música aleatória e a coloca na frente da playlist
                            int next_song = rand() % (playlist.size()-1);
                            playlist.insert(playlist.begin(), playlist[next_song]);
                            playlist.erase(playlist.begin() + next_song + 1);
                        }
                    }
                }
                break;
            case ERR: // timeout do getch, permite que o main dê refresh na tela
                break;
        }

        // playlist_window update
        if (playlist.empty() and playlist_window_needs_refresh) {
            wclear(playlist_window);
            box(playlist_window, 0, 0);
            wmove(playlist_window, 1, 1);
            wprintw(playlist_window, "Your playlist is empty.");
            wrefresh(playlist_window);
            playlist_window_needs_refresh = false;
        } else if (playlist_window_needs_refresh) {
            wclear(playlist_window);
            box(playlist_window, 0, 0);
            wmove(playlist_window, 1, 1);
            wprintw(playlist_window, ("1 > " + playlist[0].get_name() + " " + playlist[0].get_formated_duration() + " <").data());
            for (int i = 1; i < playlist.size(); ++i) {
                wmove(playlist_window, i + 1, 1);
                wprintw(playlist_window, (to_string(i + 1) + "   " + playlist[i].get_name() + " " + playlist[i].get_formated_duration()).data());
            }
            wrefresh(playlist_window);
            playlist_window_needs_refresh = false;
        }

        // player window update

        if (!playlist.empty()) { // passa para a próxima música se a atual chegou ao fim
            while (pthread_mutex_trylock(&player_mutex));
            if (global_player_time >= playlist[0].get_duration()) {
                playlist.erase(playlist.begin());
                playlist_window_needs_refresh = true;
                if (playlist.empty()) {
                    global_playing_song = false;
                } else {
                    player_reset = true;
                }
                if (not playlist.empty() and random_mode and playlist.size() != 1) { // se random_mode, escolhe uma música aleatória e a coloca na frente da playlist
                    int next_song = rand() % (playlist.size()-1);
                    playlist.insert(playlist.begin(), playlist[next_song]);
                    playlist.erase(playlist.begin() + next_song + 1);
                }
            }
            pthread_mutex_unlock(&player_mutex);
        }

        if (playlist.empty()) {
            wclear(player_window);
            box(player_window, 0, 0);
            wmove(player_window, player_window_y_center, ((2*(max_screen_size_x/3)) - 19) / 2);
            wprintw(player_window, "Nothing is playing");
            wrefresh(player_window);
        } else { // imprime o player se algo está tocando
            wclear(player_window);
            box(player_window, 0, 0);
            string current_song_name = playlist[0].get_name();
            wmove(player_window, player_window_y_center - 1, ((2*(max_screen_size_x/3)) - current_song_name.size()) / 2);
            wprintw(player_window, current_song_name.data());
            wmove(player_window, player_window_y_center, ((2 * (max_screen_size_x / 3)) - 24) / 2);
            // pega o tempo atual formatado do playback e a razão entre o tempo total e o atual.
            while (pthread_mutex_trylock(&player_mutex));
            Song why_am_I_doing_this = Song("I swear this is a temporary fix", global_player_time);
            string formated_playback_time = why_am_I_doing_this.get_formated_duration();
            float playback_ratio = ((float)global_player_time) / (float)playlist[0].get_duration();
            pthread_mutex_unlock(&player_mutex);

            string song_progress; // decide que barra de progresso utilizar
            if (playback_ratio < 0.1) {
                song_progress = "----------";
            } else if (playback_ratio >= 0.1 and playback_ratio < 0.2) {
                song_progress = "#---------";
            } else if (playback_ratio >= 0.2 and playback_ratio < 0.3) {
                song_progress = "##--------";
            } else if (playback_ratio >= 0.3 and playback_ratio < 0.4) {
                song_progress = "###-------";
            } else if (playback_ratio >= 0.4 and playback_ratio < 0.5) {
                song_progress = "####------";
            } else if (playback_ratio >= 0.5 and playback_ratio < 0.6) {
                song_progress = "#####-----";
            } else if (playback_ratio >= 0.6 and playback_ratio < 0.7) {
                song_progress = "######----";
            } else if (playback_ratio >= 0.7 and playback_ratio < 0.8) {
                song_progress = "#######---";
            } else if (playback_ratio >= 0.8 and playback_ratio < 0.9) {
                song_progress = "########--";
            } else if (playback_ratio >= 0.9 and playback_ratio < 1.0) {
                song_progress = "#########-";
            } else {
                song_progress = "##########";
            }
            wprintw(player_window, (formated_playback_time + " [" + song_progress + "] " + playlist[0].get_formated_duration()).data());
            // avisa se a música atual está pausada
            while (pthread_mutex_trylock(&player_mutex));
            if (!global_playing_song) {
                wmove(player_window, player_window_y_center + 1, ((2*(max_screen_size_x/3)) - 6) / 2);
                wprintw(player_window, "Paused");
            }
            pthread_mutex_unlock(&player_mutex);
            if (random_mode) {
                wmove(player_window, player_window_y_center + 2, ((2*(max_screen_size_x/3)) - 19) / 2);
                wprintw(player_window, "Random mode enabled");
            }
            wmove(player_window, player_window_y_center + 3, ((2*(max_screen_size_x/3)) - 22) / 2);
            wprintw(player_window, "[s] toggle random mode");
            wrefresh(player_window);
        }
    }

    endwin();
    return 0;
}