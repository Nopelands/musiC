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

class Song {
    string name;
    int duration; // em segundos
    int playback_time; // -1 significa que a música não está tocando
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

    void set_playback_time(int seconds) { // será usado em versões futuras
        playback_time = seconds;
    }

    int get_playback_time() {
        return playback_time;
    }
};

void *keyboard(void *arg) { // Thread que lê o teclado no menu principal
    string input;
    while (true) {
        std::cin >> input;
        cin.ignore(numeric_limits<streamsize>::max(),'\n'); // remove newlines presentes na stream
        while (pthread_mutex_trylock(&mutex)); // mutex para acessar a região crítica
        key = input; // escreve o input na variável global key
        pthread_mutex_unlock(&mutex);
        if (input == "q") {
            pthread_exit(nullptr);
        } else if (input == "a" or input == "r"){
            pthread_barrier_wait(&barrier); // impede que o thread use o teclado enquanto o main thread está processando adição ou remoção de músicas
        }
    }
}

int main() { // TODO comment
    bool exit = false;
    bool screen_needs_refresh = true;
    vector<Song> songs; // vetor fila de reprodução

    // cria e inicia o thread do teclado
    pthread_t ler_teclado;
    pthread_create(&ler_teclado, nullptr, &keyboard, nullptr);

    // inicia a barreira do teclado
    pthread_barrier_init(&barrier, nullptr, 2);

    // splash screen
    std::cout << "Welcome to musiC++!" << std::endl;
    sleep(5);
    system("clear");

    // main loop
    while (!exit) {
        // imprime o menu principal se necessário
        if (songs.empty() and screen_needs_refresh) {
            std::cout << R"(Your queue is empty, type "a" to add a song or "q" to quit)" << std::endl;
            screen_needs_refresh = false;
        } else if (screen_needs_refresh) { // imprime o "player" de música
            screen_needs_refresh = false;
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

        while (pthread_mutex_trylock(&mutex)); // adquire a tranca para acessar e modificar key
        if (key == "q") {
            exit = true;
        } else if (key == "a") {
            screen_needs_refresh = true;
            string song_name;
            string song_duration;
            system("clear");

            std::cout << "Type the name of the song" << std::endl;
            getline(cin, song_name);
            system("clear");

            std::cout << "Type the duration of the song (in seconds)" << std::endl;
            std::cin >> song_duration;
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            system("clear");

            if (song_duration.find_first_not_of( "0123456789" ) == std::string::npos) { // verifica se a duração é um inteiro positivo
                stringstream stream_song_duration(song_duration);
                int int_song_duration;
                stream_song_duration >> int_song_duration;
                songs.push_back(Song(song_name, int_song_duration)); // cria a música e a coloca no vetor songs
            } else {
                std::cout << "Input is not a number" << std::endl; // cancela a adição e retorna ao menu
                sleep(2);
                system("clear");
            }
            pthread_barrier_wait(&barrier); // libera a barreira do thread do teclado
        } else if (key == "r") {
            screen_needs_refresh = true;
            system("clear");
            std::cout << "Type the number of the song you wish to remove:" << std::endl;
            for (int i = 0; i < songs.size(); ++i) { // printa a fila de reprodução para escolha do index
                std::cout << to_string(i+1) + " " + songs[i].get_name() << std::endl;
            }
            int to_remove;
            std::cin >> to_remove;
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            to_remove = to_remove - 1;
            if (to_remove < 0 or to_remove >= songs.size()) { // verifica se a música a ser removida existe
                std::cout << "Song not found" << std::endl;
                sleep(2);
            } else {
                songs.erase(songs.begin() + to_remove);
            }
            system("clear");
            pthread_barrier_wait(&barrier); // libera a barreira da thread do teclado
        }
        key = "nope"; // reseta key
        pthread_mutex_unlock(&mutex); // sai da região crítica
    }
    return 0;
}
