#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <random>
#include <string>
#include <cstdlib>
#include <ctime>

class TicTacToe {
private:
    std::array<std::array<char, 3>, 3> board; // Tabuleiro do jogo
    std::mutex board_mutex; // Mutex para controle de acesso ao tabuleiro
    std::condition_variable turn_cv; // Variável de condição para alternância de turnos
    char current_player; // Jogador atual ('X' ou 'O')
    bool game_over; // Estado do jogo
    char winner; // Vencedor do jogo

public:
    TicTacToe() : current_player('X'), game_over(false), winner(' ') { //Inicializa com valores padrão
        for (auto& row : board) {
            row.fill(' ');  // Preenche o array com caracteres vazios
        }
    }

    void display_board() {
{ 
    std::cout << "-------------\n"; 
    for (int i = 0; i < 3; i++) { 
        std::cout << "| "; 
        for (int j = 0; j < 3; j++) { 
            std::cout << this->board[i][j] << " | "; 
        } 
        std::cout << "\n-------------\n"; 
        std::cout << std::endl;
    }
}
    }

bool make_move(char player, int row, int col) {

        if (player == current_player) {
        std::unique_lock<std::mutex> lock(board_mutex);
        turn_cv.wait(lock);
    }
        if(this->game_over){
            return true;
        }
    
    board_mutex.lock();
    if (this->board[row][col] == ' ') {
        current_player = player;
        this->board[row][col] = player;

        if (check_win()) {
            game_over = true;
            winner = get_winner();
        } else if (check_draw()) {
            game_over = true;
            winner = 'D';  // D for Draw
        }
        this->display_board();
        board_mutex.unlock();
        turn_cv.notify_one();
        return true;
    }
        this->display_board();
        board_mutex.unlock();
        turn_cv.notify_one();
    return false;
}

bool check_draw() {
    if(check_win()){
	return false;
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == ' ') {
                return false;  
            }
        }
    }
    return true;  
}

    bool check_win() {
        for (int i = 0; i < 3; i++) { 
          if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) 
            return true; 
          if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) 
            return true; 
    } 
        if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) 
            return true; 
        if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) 
            return true; 
    return false; 
}
 
    bool check_empty_cell(int linha, int coluna){
        if (this->board[linha][coluna] == ' ') {
		return true;
	}
        return false;

    }

    bool is_game_over() {
        if(check_draw() || check_win()){
            return true;
        }
        return false;
    }

    char get_winner() {
        if(check_draw()){
            return 'D';
        }
        else
        return this->current_player;
    }
};

class Player {
private:
    TicTacToe& game; // Referência para o jogo
    char symbol; // Símbolo do jogador ('X' ou 'O')
    std::string strategy; // Estratégia do jogador

public:
    Player(TicTacToe& g, char s, const std::string& strat) 
        : game(g), symbol(s), strategy(strat) {
    }
 
    void play() {
        while(!this->game.is_game_over()){
        if (this -> strategy == "random") {
            play_random();
        } else if (this -> strategy == "sequential") {
            play_sequential();
        }
    }
        return;
    }

private:
    void play_sequential() {
     for (int lin = 0; lin < 3; lin++) {
           for (int col = 0; col < 3; col++) {
               if (game.check_empty_cell(lin, col)) {
                 game.make_move(this->symbol, lin, col);
                 return;
             }
            }
      }
    }

    void play_random() {
      int lin, col;
     do {
            lin = std::rand() % 3;
         col = std::rand() % 3;
     } while (!game.check_empty_cell(lin, col));
     game.make_move(this->symbol, lin, col);
    }

};

int main() {
    std::srand(static_cast<unsigned int>(std::time(0))); 

    TicTacToe jogo;
    Player playerX(jogo, 'X', "random");
    Player playerO(jogo, 'O', "sequential");

    std::thread t1(&Player::play, &playerX);
    std::thread t2(&Player::play, &playerO);

    t1.join();
    t2.join();

    jogo.display_board();
    std::cout<<"O jogador vencedor foi: " << jogo.get_winner() << std::endl;
    return 0;
}
