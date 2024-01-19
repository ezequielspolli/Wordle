#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define FILENAME "Palabras.txt"
#define MAX_WORDS_SAVED 30
#define NEW_LINE printf("\n")

#define ROWS 13
#define COLS 5
#define INITIAL_POSITION 0

#define MAX_GAMES 8
#define MAX_TRY 6
#define MAX_LETTERS 5
#define SAVED_WORD 0

#define INITIAL_SCORE 5000
#define MAX_SCORE 10000
#define WIN_SCORE 2000
#define NEW_TRY_SCORE 500
#define NEWCORRECTLETTER_SCORE 100
#define NEWINCORRECTLETTER_SCORE 50
#define PROMEDIO(X,Y) (X/Y)

#define CONTINUE 1
#define WIN 1
#define LOSE 0

void initialize_array (int array [MAX_GAMES]);
int ask_for_games ();
void get_saved_words (char matriz [MAX_GAMES][ROWS][COLS],int games);
void getWordInLine(char *fileName, int lineNumber, char *p);
void play_match(char matriz [MAX_GAMES][ROWS][COLS],int try_count [MAX_GAMES], int score [MAX_GAMES],int match,int line, int games);
void ask_for_try_word (char matriz [MAX_GAMES][ROWS][COLS], int match,int line);
int check_word (char matriz [MAX_GAMES][ROWS][COLS], int match,int line);
void compare_try_word (char matriz [MAX_GAMES][ROWS][COLS],int score [MAX_GAMES],char correct [MAX_LETTERS],char incorrect [MAX_LETTERS],int match,int line,int position);
int check_win (char matriz [MAX_GAMES][ROWS][COLS], int match, int line);
void score_win (int score [MAX_GAMES],int match,int try_count [MAX_GAMES]);
void print_results_statistics (char matriz[MAX_GAMES][ROWS][COLS], int score [MAX_GAMES], int match);
void statistics (int score [MAX_GAMES], int match);
void see_match (char matriz [MAX_GAMES][ROWS][COLS], int try_count [MAX_GAMES], int score [MAX_GAMES], int match);
void print_line (char matriz [MAX_GAMES][ROWS][COLS],int match, int line);
void print_match (char matriz [MAX_GAMES][ROWS][COLS],int match, int try_count [MAX_GAMES]);
int exit_game (int match, int games);

int main(){

    int games;
    int match_count;
    int row_number;
    int try_count [MAX_GAMES];
    int score [MAX_GAMES];
    char match [MAX_GAMES][ROWS][COLS];
    srand (getpid());

    printf("\n Bienvenide a WORDLE!!!\n");

    do{
        match_count=INITIAL_POSITION;
        initialize_array (try_count);
        initialize_array (score);
        games= ask_for_games ();
        get_saved_words (match,games);
        printf("\n Por favor, ingresa las palabras en LETRAS MAYÚSCULAS\n");
        do{
            printf(" Partida nro %d de %d:\n", match_count+1, games);
            play_match(match,try_count,score,match_count,row_number, games);
            match_count++;
        }while (match_count<games && exit_game(match_count, games)==CONTINUE);
        print_results_statistics  (match, score, match_count);
        see_match (match ,try_count ,score, match_count);
    } while (exit_game(match_count, games-1)==CONTINUE);
    return EXIT_SUCCESS;
}

// FUNCTIONS
void initialize_array (int array [MAX_GAMES]){
    for (int i=0;i<MAX_GAMES;i++){
        array[i]=INITIAL_POSITION;
    }
}

int ask_for_games (){
    int games;
    printf(" Por favor, ingresa el número de partidas que deseas jugar (Máximo 8 partidas)\n");
    do{
        printf(" >  ");
        scanf("%d", &games);
    } while (games<1||games>MAX_GAMES);
    return games;
}

void get_saved_words (char matriz [MAX_GAMES][ROWS][COLS],int games){
    int word_line [MAX_GAMES];
    for (int i=0; i<games;i++){
        word_line [i]= 1+rand()%MAX_WORDS_SAVED;
        for (int j=i-1; j>=INITIAL_POSITION;j--){
            if (word_line [i]==word_line [j]){
                i--;
                break;
            }
        }
    }
    for (int i=0; i<games;i++){
    getWordInLine(FILENAME, word_line [i], matriz [i][SAVED_WORD]);
    }
}

void getWordInLine(char *fileName, int lineNumber, char *p) {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(fileName, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    int i=1;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (i==lineNumber) {
           strcpy(p, line);
           return;
        }
        i++;
    }
}

void play_match(char matriz [MAX_GAMES][ROWS][COLS],int try_count [MAX_GAMES], int score [MAX_GAMES],int match,int line, int games){
    int position=INITIAL_POSITION;
    char correct [MAX_LETTERS]={INITIAL_POSITION};
    char incorrect [MAX_LETTERS]={INITIAL_POSITION};
    line=1;
    score [match]=INITIAL_SCORE;
    do{
        if (try_count[match]!=0){
            line+=2;
            score [match]-=NEW_TRY_SCORE;
        }
        try_count [match]++;
        ask_for_try_word (matriz,match,line);
        compare_try_word (matriz,score,correct,incorrect,match,line,position);
        NEW_LINE;                                                               //Print_try_word_&_result
        print_line (matriz, match, line);
        print_line (matriz, match, line+1);
        NEW_LINE;
        printf("Intento: %d \n",try_count [match]);
    }while (try_count [match]<MAX_TRY & check_win (matriz, match, line)== LOSE);
    score_win (score,match, try_count);
    printf("\nPartida nro: %d de %d - Palabra secreta: ", match+1, games);
    print_line (matriz, match, SAVED_WORD);
    NEW_LINE;
    print_match (matriz, match, try_count);
    NEW_LINE;
    printf("Puntaje: %d\n",score [match]);
    printf("Intentos: %d\n",try_count[match]);
}

void ask_for_try_word (char matriz [MAX_GAMES][ROWS][COLS], int match,int line){
    printf("\n Por favor, ingresa una palabra de 5 letras (NO repetir letras)\n");
    do{
        printf(" >  ");
        scanf("%s", matriz [match][line]);
    }while (check_word (matriz,match,line)!=EXIT_SUCCESS);
}

int check_word (char matriz [MAX_GAMES][ROWS][COLS], int match,int line){
    int cont=INITIAL_POSITION;
    if(strlen(matriz [match][line])!=MAX_LETTERS){                                 //Lenght_try_Word
        printf("\n La palabra no cuenta con la cantidad de letras requerida\n");
        cont++;
    }
    if (cont==0){                                                                  //Repeated_Letter_try_word
        for (int i=0; i<COLS-1;i++){
            for (int j=i+1;j<COLS;j++){
                if (matriz [match][line][i] == matriz [match][line][j]){
                    printf("\n La palabra ingresada repite letras\n");
                    cont++;
                }
            }
        }

    }
    return cont;
}

void compare_try_word (char matriz [MAX_GAMES][ROWS][COLS],int score [MAX_GAMES],char correct [MAX_LETTERS],
                       char incorrect [MAX_LETTERS],int match,int line,int position){
    int cont_rep;
    char letter;
    char letter_result;
    for(int i=0;i<COLS;i++){
        letter = matriz [match][line][i];
        letter_result = matriz [match][line+1][i];
        for(int j=0;j<COLS;j++){
            if(letter != matriz [match][SAVED_WORD][j]){               //Letter_IS_NOT_in_Saved_word
                letter_result = '_';
            }else{                                                          //Letter_IS_in_Saved_Word
                cont_rep=0;
                for(int k=0;k<COLS;k++){                                        //CHECK_REPEATED_LETTER
                    if(letter != incorrect [k]){
                        cont_rep++;
                    }
                }

                if(i==j){                                                       //LETTER_CORRECT_POSITION
                    letter_result = letter;
                    if(cont_rep==MAX_LETTERS){                                      //New_letter
                        correct [i]= letter;
                        incorrect [position]= letter;
                        position++;
                        score [match]+=NEWCORRECTLETTER_SCORE;                          //SCORE_New_letter_correct_position
                    }else {
                        if(letter != correct [i]){                                  //Used_letter_incorrect_position
                            correct [i]= letter;
                            score [match]+=NEWCORRECTLETTER_SCORE;                      //SCORE_New_letter_correct_position
                        }
                    }
                }else{                                                          //LETTER_INCORRECT_POSITION
                    letter_result ='*';
                    if(cont_rep==MAX_LETTERS){                                      //New_letter
                        incorrect [position]= letter;
                        position++;
                        score [match] += NEWINCORRECTLETTER_SCORE;                      //SCORE_New_letter_incorrect_position
                    }
                }
                break;                                                          //COMPARE_NEXT_LETTER
            }
        }
        matriz [match][line][i] = letter;
        matriz [match][line+1][i] = letter_result;
    }
}

int check_win (char matriz [MAX_GAMES][ROWS][COLS], int match, int line){
    for(int col=0;col<COLS;col++){
        if (matriz [match][line][col]!= matriz [match][SAVED_WORD][col]){
            return LOSE;
        }
    }
    return WIN;
}

void score_win (int score [MAX_GAMES],int match,int try_count[MAX_GAMES]){
    if (try_count[match]>MAX_TRY){
        score [match]= LOSE;
    }else{
        if (try_count [match]==1){
            score [match]= MAX_SCORE;
        } else {
            score [match]+= WIN_SCORE;
        }
    }
}

void print_results_statistics (char matriz[MAX_GAMES][ROWS][COLS], int score [MAX_GAMES], int match){
    NEW_LINE;
    printf("Tu recorrido por el juego fue:\n\n");
    for (int nro_match=0; nro_match<match;nro_match++){
        printf("Partida nro %d - Puntaje: %d - Palabra secreta: ",nro_match+1,score [nro_match]);
        print_line (matriz, nro_match, SAVED_WORD);
    }
    NEW_LINE;
    if (match!=1){
    statistics (score, match);
    }
}

void statistics (int score [MAX_GAMES], int match){
    int max_score= score [INITIAL_POSITION];
    int min_score= score [INITIAL_POSITION];
    int win_total_score=INITIAL_POSITION;
    int win_cont=INITIAL_POSITION;

    for (int position=INITIAL_POSITION; position<match; position++){
        if(score [position]!=0){
            win_total_score+= score [position];
            win_cont++;
        }
    }
    for (int position=INITIAL_POSITION; position<match; position++){
        if(max_score<score [position]){
            max_score= score [position];
        }
        if (min_score>score [position]){
            min_score= score [position];
        }
    }
    if (min_score!=max_score){
        printf("El puntaje más alto fue: %d puntos en:\n",max_score);
        for (int position=INITIAL_POSITION; position<match; position++){
            if(score[position]== max_score){
                printf("Partida nro: %d\n",position+1);
            }
        }
        NEW_LINE;
        printf("El puntaje más bajo fue: %d puntos en:\n",min_score);
        for (int position=INITIAL_POSITION; position<match; position++){
            if(score[position]== min_score){
                printf("Partida nro: %d\n",position+1);
                }
        }
        NEW_LINE;
    }
    printf("El promedio de todas tus victorias fue: %d\n",PROMEDIO(win_total_score,win_cont));
}

void see_match (char matriz [MAX_GAMES][ROWS][COLS], int try_count [MAX_GAMES],int score [MAX_GAMES], int match){
    int menu_answer=INITIAL_POSITION;
    do{
        printf("\nDeseas ver alguna partida? \n\t1.SI \n\t2.NO\n\n");
        do{
            printf(" >  ");
            scanf("%d", &menu_answer);
        } while (menu_answer!=1 && menu_answer!=2);
        if (menu_answer==1){
            printf("\nQue numero de partida deseas ver?\n\n");
            do{
                printf(" >  ");
                scanf("%d", &menu_answer);
            } while (menu_answer<1 || menu_answer>match);
            printf("\nPartida nro %d de %d - Palabra secreta: ",menu_answer, match);
            print_line (matriz, menu_answer-1, SAVED_WORD);
            NEW_LINE;
            print_match (matriz, menu_answer-1, try_count);
            NEW_LINE;
            printf("Puntaje: %d\n",score [menu_answer-1]);
            printf("Intentos: %d\n",try_count[menu_answer-1]);
            menu_answer=INITIAL_POSITION;
        }
    }while (menu_answer!=2);
}

void print_line (char matriz [MAX_GAMES][ROWS][COLS],int match, int line){
    for (int col=0;col<COLS;col++){
        printf( "%c ", matriz [match][line][col]);
    }
    NEW_LINE;
}

void print_match (char matriz [MAX_GAMES][ROWS][COLS],int match, int try_count [MAX_GAMES]){
    print_line (matriz, match, SAVED_WORD);
    NEW_LINE;
    for(int line=1;line<try_count [match]*2+1;line++){
        print_line (matriz, match, line);
    }
}

int exit_game (int match, int games){
    int menu_answer;
    if (match!=games){
        printf("\nDeseas seguir jugando? \n\t1.SI \n\t2.NO\n\n");
        do{
            printf(" >  ");
            scanf("%d", &menu_answer);
        } while (menu_answer!=1&menu_answer!=2);
        if (menu_answer==1){
            NEW_LINE;
            return CONTINUE;
        }
    }
    return EXIT_SUCCESS;
}


