#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

using namespace std;

#include "const.h"
#include "gioco.h"

gioco::gioco() {
 salto = 6;
 n=12;
 f=12;
 score = 0;
 level = 0;
 vite = 5;
 dist_proiettile = 12;
 start = 0;
 prec = false;
 passi_indietro = 0;
}

void gioco::setCursorPosition(int x, int y)//funzione che permette di prendere punti specifici della mappa
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, coord);
}

void gioco::run() { //Funzione principale del gioco che orchestra l'intero codice e che mette in moto tutto
  srand (time(NULL));
    lista head = NULL;
    lista scheletro = NULL;
    head = fill(head);
    scheletro = fill(scheletro);
    aggiorna_matrice(head, start);
    aggiorna_matrice_scheletro(scheletro, start);


    while (true) {

        contatto_bonus();
        collisione_nemici();
        draw_matrice();
        if(score < 0 || vite <1){
            system("cls");
            cout << "GAMEOVER, PUNTEGGIO: " + score;
            Sleep(5000);
            break;
        }

        while (c != 'a' && c != 'd' && c != 'w' && c != 's' && c != ' '){
            c = NULL;
            if (_kbhit())
                c = _getch();
            Sleep(0);
            sparo_nemici();
            gestione_proiettile();
            contatto_nemico();
            genera_nemico2();
            nemico2();
            collisione_mob_terra();
            aggiorna_matrice(head, start);
            aggiorna_matrice_scheletro(scheletro, start);
            system("cls");
            draw_matrice();
        }
        if(c == 'w' && f>1){
            salto_e_aderenza();
            if(score < 0 || vite <1){
            system("cls");
            break;
        }
        }
        if(c == 'd'){
            passo_destra(head, scheletro);
            aggiorna_matrice(head, start);
            aggiorna_matrice_scheletro(scheletro, start);
            bonus(head);
            nemico(head);
            if(score < 0 || vite <1){
            system("cls");
            break;
        }
        }
        if(c == 'a'){
            passo_sinistra();
            aggiorna_matrice(head, start);
            aggiorna_matrice_scheletro(scheletro, start);
            traiettoria_volante();
            if(score < 0 || vite <1){
            system("cls");
            break;
        }
        }

        if(c == 's'){
            scendi_e_aderenza();
            if(score < 0 || vite <1){
            system("cls");
            break;
        }
        }

        if(c == ' '){
            int n = sparo();
            vita_nemici(n);
            if(score < 0 || vite <1){
            system("cls");
            break;
        }
        }
        system("cls");
    }
cout << "GAMEOVER, PUNTEGGIO: " << score << endl;

}

lista gioco::fill(lista head) {//funzione che riempie la mappa nel suo stadio iniziale
    lista tail = head;
    for (int i = 0; i <= width; i++) {
        lista tmp = new matrix_col;
        tmp->col[0] = ' ';
        for (int j = 1; j <= height; j++) {
            int v1 = rand() % 73 + 72;
            if(j<13)
                tmp->col[j] = ' ';
            else if(j == 13){
                tmp->col[j] = '-';
            }
            else{
                if(v1 == 73){
                    tmp->col[j] = ';';
                }
                else{
                    tmp->col[j] = ':';
                }
            }
        }

        if (tail != NULL) {
            tail->next = tmp;
        }
        else {
            head = tmp;
        }
        tmp->next = NULL;
        tail = tmp;
    }

    return head;
}

int gioco::piattaforma_gia_generata(int riga){//ritorna la lunghezza di una determinata piattaforma in una determinata riga
    int count1 = 0;
    int decremento = 1;
    while(matrix_map[70-decremento]->col[riga] != ' '){
            count1++;
            decremento++;
    }
    return count1;
}

bool gioco::bonus_gia_generato(int riga){//funzione che controlla lo spawn di bonus sulla stessa piattaforma
    int count2 = 0;
    int dec = 1;
    while(matrix_map[70-dec]->col[riga+1] == '~'){//calcolo distanze per il controllo
        count2++;
        dec++;
    }
    while(count2>0){//controllo
        for(int j = 1; j < 26 ; j ++){
            if(matrix_map[70-count2]->col[riga] == 65 + j){
                return true;}
            }
            count2--;
        }

    return false;

}

void gioco::bonus(lista head) {//funzione addetta alla generazione dei bonus
    lista current = head;
    while(current->next != NULL){
        current = current->next;
    }
    int v = (rand() % 3+1);

    if(v == 2){//funzione che con l'ausilio di bonus_gia_generato() controlla lo spawn dei bonus
        int v1 =(rand() % 11+2);
        bool bg = false;
        if(v1%2 == 0){
            bg = bonus_gia_generato(v1);
        }
        if(v1%2 == 0 && matrix_map[70]->col[v1+1] == '~' && bg == false){//controlla che il bonus non sia generato in aria
            int v2 = (rand() % 24+1);
            current->col[v1] = 65+v2;}
        }
    }

void gioco::collisione_nemici(){// gestisce l'evenienza di contatto con i nemici e modifica di conseguenza vita e punti
    for(int i = 1; i<10; i++){
        if(matrix_map[n]->col[f] == 48+i){
            score = score-50;
            vite = vite-1;
            level = score/500;
        }
    }
}

int gioco::conteggio_piattaforma_versodestra(int j, int i){//dato un punto ritorna la lunghezza della piattaforma sottostante da quel punto in avanti
    int counter = 0;
    int g = j;
    while(matrix_map[g]->col[i+1] == '~' && g<70){
            counter++;
            g++;
    }
    return counter;
}

int gioco::conteggio_piattaforma_versosinistra(int j, int i){//dato un punto ritorna la lunghezza della piattaforma sottostante da quel punto indietro
    int counter = 0;
    int g = j;
    while(matrix_map[g]->col[i+1] == '~'){
            counter++;
            g--;
    }
    return counter;
}

bool gioco::nemico_gia_generato(int riga){ //funzione che regola e controlla la generazione dei nemici
    int count2 = piattaforma_gia_generata(riga+1);
    for(int i = 70; i > count2; i--){
        for(int j = 1; j < 10 ; j ++)
        if(matrix_map[i]->col[riga] == 48 + j)
            return true;
    }
}

bool gioco::bonus_12esima(int riga){//funzione che controlla lo spawn dei bonus a portata dei proiettili
    for(int i = 0; i < 26; i++){
        if(matrix_map[70-dist_proiettile]->col[riga] == 65+i){
            return true;
        }
    }
    return false;
}

bool gioco::bonus_prec(int riga){//funzione che controlla se un bonus è già stato generato nelle vicinanze
        for(int j = 0; j < 26 ; j ++){
            if(matrix_map[67]->col[riga] == 65 + j || matrix_map[68]->col[riga] == 65 + j || matrix_map[69]->col[riga] == 65 + j || matrix_map[70]->col[riga] == 65 + j){
                return true;
            }
        }
        return false;
}

bool gioco::distanza_tra_nemici(int riga){// indica se nelle vicinanze è già presente
        for(int j = 0; j < 10; j++){
            for(int z = 0; z <= dist_proiettile; z++){
                if(matrix_map[70-z]->col[riga] == 48+j){
                    return true;
                }
            }
        }
        return false;
}

void gioco::nemico(lista head) {//funzione di generazione dei nemici e scaling della difficoltà dei nemici
    lista current = head;
    while(current->next != NULL){
        current = current->next;
    }
        int v1 =(rand() % 11+1);
        if(v1%2 == 0 && matrix_map[70]->col[v1+1] == '~'){
            bool esistente = nemico_gia_generato(v1);
            bool esistente1 = bonus_12esima(v1);
            bool esistente2 = bonus_prec(v1);
            bool esistente3 = distanza_tra_nemici(v1);
            if(!esistente && !esistente1 && !esistente2 && !esistente3){
                int x = conteggio_piattaforma_versosinistra(70, v1);
                if(x>3){
                int v2 =(rand() % 2+1);
                int v3 =(rand() % 4+3);
                int v4 =(rand() % 4+6);

                if(level<=1){
                    matrix_map[70]->col[v1] = 48+v2;
                    matrix_map_scheletro[71-x]->col[v1] = '!';
                }
                else if(level >= 25 && level <=50){
                    matrix_map[70]->col[v1] = 48+v3;
                    matrix_map_scheletro[71-x]->col[v1] = '!';
                }
                else{
                    matrix_map[70]->col[v1] = 48+v4;
                    matrix_map_scheletro[71-x]->col[v1] = '!';
                }

            }
        }
    }
}

void gioco::genera_nemico2(){//funzione che genera un nemico in muovimento nella parte inferiore della mappa
    bool presente = false;
    for(int i=0; i<=width; i++){
        for(int j=0; j<=height; j++){
            if(matrix_map[i]->col[j] == 50+10){
                presente = true;
            }
        }
    }
    if(!presente && level>=1){
        matrix_map[69]->col[12] = 50+10;
    }
}

void gioco::nemico2(){//funzione che gestisce il movimento di nemico2
    for(int i=0; i<=width; i++){
        for(int j=0; j<=height; j++){
            if(matrix_map[i]->col[j] == 50+10){
                if(i == 0){
                    matrix_map[i]->col[j] = ' ';
                }
                else{
                    matrix_map[i]->col[j] = ' ';
                    matrix_map[i-1]->col[12] = 50+10;
                }
            }
        }
    }
}

void gioco::collisione_mob_terra(){//funzione che gestisce punti e vita relativi a nemico 2
    if(matrix_map[n]->col[f] == 50+10){
        vite = vite-1;
        score = score-50;
        level = score/500;
    }
}

void gioco::contatto_bonus(){// funzione che gestisce punti e vita quando a contatto con un bonus nonchè la scomparsa del bonus
    for(int i = 65; i<91 ; i++){
    if(matrix_map[n]->col[f] == 0+i){
        matrix_map[n]->col[f] = ' ';
        score = score+i*2;
        level = score/500;
    }
    }
}

void gioco::add_col(lista head) {//funzione per la generazione delle nuove colonne in coda
    lista current = head;
    while(current->next != NULL){
        current = current->next;
    }

    lista tmp = new matrix_col;
    tmp->next = NULL;
    current->next = tmp;
    current = current->next;
    for (int i = 0; i <= width; i++) {
        lista tmp = new matrix_col;
        current->col[0] = ' ';
        for (int j = 1; j <= height; j++) {
            if(j<13)
                current->col[j] = ' ';
            else if(j == 13){
                current->col[j] = '-';
            }
            else{current->col[j] = ':';}
            }
        }
    }

void gioco::piattaforme(lista head) {//funzione per la generazione di piattaforme di dimensione randomica maggiore di uno su determinate righe
    lista current = head;
    while(current->next != NULL){
        current = current->next;
    }

    int v3 =(rand() % 5) +1;

        int controllo = piattaforma_gia_generata(3);
        if(controllo<20){
            if(v3 == 1 && controllo > 3){current->col[3] = ' ';}
            else{current->col[3] = '~';}
        }

        controllo = piattaforma_gia_generata(5);
        if(controllo<20){
            if(v3 == 2 && controllo > 3){current->col[5] = ' ';}
            else{current->col[5] = '~';}
            }

        controllo = piattaforma_gia_generata(7);
        if(controllo<20){
            if(v3 == 3 && controllo > 3){current->col[7] = ' ';}
            else{current->col[7] = '~';}
        }

        controllo = piattaforma_gia_generata(9);
        if(controllo<20){
            if(v3 == 4 && controllo > 3){current->col[9] = ' ';}
            else{current->col[9] = '~';}
            }

        controllo = piattaforma_gia_generata(11);
        if(controllo<20){
            if(v3 == 5 && controllo > 3){current->col[11] = ' ';}
            else{current->col[11] = '~';}
            }
}

void gioco::salto_e_aderenza(){//funzione che si occupa di gestire il salto e il rapporto giocatore piattaforme
            bool esiste = false;
            if(f>=3){
                for(int i = 0; i<5; i++){
                    if(f-1>=0){
                    setCursorPosition(n, f);
                    cout << ' ';
                    Sleep(5);
                    setCursorPosition(n, f-1);
                    cout << "K";
                    }
                    f = f-1;
                    if (matrix_map[n]->col[f] == '~') {
                        f = f-1;
                        esiste = true;
                        break;
                    }
                    Sleep(5);
                }

                if(!esiste){
                    for(int i = 0; i<5; i++){
                        setCursorPosition(n, f);
                        cout << " ";
                        Sleep(5);
                        setCursorPosition(n, f+1);
                        cout << "K";
                        f = f+1;
                        Sleep(35);
                    }
                }
            }
            c = NULL;
        }

void gioco::scendi_e_aderenza(){//funzione che si occupa della discesa statica da una piattaforma al livello inferiore e del rapporto con piattaforme inferiori o eventuali vuoti
            if(f<12){
                    f = f+1;
                    if (matrix_map[n]->col[f] == '~') {
                        f = f+1;
                        if(f+1<13 && f+1 == '~'){
                        setCursorPosition(n, f);
                        cout << ' ';
                        Sleep(0);
                        setCursorPosition(n, f+1);
                        cout << "K";}
                    }
                }
                        while(matrix_map[n]->col[f+1] != '~' && f<12  ){
                        if(f+1<13){
                        setCursorPosition(n, f);
                        cout << ' ';
                        Sleep(0);
                        setCursorPosition(n, f+1);
                        cout << "K";
                        f = f+1;

                        }
                        }
            c = NULL;
        }

void gioco::caduta_avanti(){//funzione che si occupa della gestione della caduta quando si supera il bordo di una piattaforma
        while(f<12){
            if (matrix_map[n+1]->col[f+1] != '~') {
                    f = f+1;
            }
            else{break;}
        }
}

void gioco::caduta_dietro(){//funzione che si occupa della gestione della caduta quando si supera il bordo di una piattaforma procedendo verso sinistra
        while(f<12){
            if (matrix_map[n-1]->col[f+1] != '~') {
                    f = f+1;
            }
            else{break;}
        }
}

void gioco::passo_destra(lista head, lista scheletro){//genera mappa a seconda del movimento verso destra del player in caso non fosse già generata
    if(prec == false && passi_indietro == 0){
        add_col(head);
        add_col(scheletro);
        piattaforme(head);
        c = NULL;
        start++;
    }
    else{
        c = NULL;
        start ++;
    }
    prec = false;
    if(passi_indietro>0){passi_indietro--;}
    else{passi_indietro=0;}

    caduta_avanti();
    }

void gioco::passo_sinistra(){//funzione che scorre la mappa verso sinistra con l'indietreggiare del personaggio
    if(start == 0){
        c = NULL;
        prec = false;}
    else{
        start--;
        c = NULL;
        prec = true;
        passi_indietro++;}

    caduta_dietro();
}

int gioco::sparo(){//funzione che si occupa di gestire l?azione di sparo del player ed annesso proiettile

    for(int i = 1; i <=10; i++){
        for(int j = 1; j < 10; j++){
            if (matrix_map[12 + i]->col[f] == 48+j) {
                setCursorPosition(12+i,f);
                cout << "°";
                c = NULL;
                return i;
            }
        }

    setCursorPosition(12 + i ,f);
    cout << "*";
    Sleep(25);
    setCursorPosition(12 + i,f);
    cout << " ";
    Sleep(35);
        }
    c = NULL;
    return 0;

}

void gioco::vita_nemici(int o){//funzione che gestise la vita dei nemici nonchè l'eliminazione
    if(o<11 && o>0){
        if(matrix_map[n+o]->col[f]>49){
            matrix_map[n+o]->col[f]--;
            }
        else{matrix_map[n+o]->col[f] = ' '; score = score + 150; }
    }
}

void gioco::aggiorna_matrice(lista head, int start) {//effettua un refresh della matrice
    lista current = head;
    int counter = start;
    while(counter>0){
        current = current->next;
        counter--;
    }
    int i = 0;
    while (current != NULL && i <= width) {
        matrix_map_prev[i] = matrix_map[i];
        matrix_map[i] = current;
        current = current->next;
        i++;
    }
}

void gioco::aggiorna_matrice_scheletro(lista head, int start) {
    lista current = head;
    int counter = start;
    while(counter>0){
        current = current->next;
        counter--;
    }
    int i = 0;
    while (current != NULL && i <= width) {
        matrix_map_scheletro[i] = current;
        current = current->next;
        i++;
    }
}

void gioco::gestione_proiettile(){//funzione di supporto per la gestione del proiettile nemico
    for(int r = 0; r <= width; r++){
        for(int k = 0; k <= height; k++){

                if(matrix_map[r]->col[k] == '+' ){
                    bool no = false;

                    if(r==0){
                        int counter = 0;
                        while(matrix_map[r+counter]->col[k] == '.' || matrix_map[r+counter]->col[k] == '+' ){
                            matrix_map[r+counter]->col[k] = ' ';
                            counter++;
                        }
                    }

                    if(matrix_map[r+1]->col[k] == '.'){
                        int cc = 1;
                        while(matrix_map[r+cc]->col[k] == '.' || matrix_map[r+cc+2]->col[k] == '.' ) {
                            cc++;
                    }

                        if(cc > dist_proiettile){
                        for(int i = 0; i <= dist_proiettile; i++){
                            if(matrix_map[r+i]->col[k] == '.' || matrix_map[r+i]->col[k] == '+' ){
                                    matrix_map[r+i]->col[k] = ' ';
                                }
                            }
                            no = true;
                        }

                    }

                    for(int i = 0; i < 26; i++){
                    if(r>0 && (matrix_map[r-1]->col[k] == ' ' || matrix_map[r-1]->col[k] == 65+i)&& !no){
                        if(matrix_map[r-1]->col[k] == ' '){
                            matrix_map[r]->col[k] = '.';
                            matrix_map[r-1]->col[k] = '+';
                        }
                        else if(matrix_map[r-1]->col[k] == 65+i && r>1){
                            matrix_map[r]->col[k] = '.';
                            matrix_map[r-2]->col[k] = '+';
                        }
                    }
                    }
                }
        }
    }
}

void gioco::sparo_nemici(){//funzione che consente ai nemici di effettuare lo sparo
    for(int r = 4; r < width-2; r++){
        for(int k = 0; k <= height; k++){
            for(int j = 1; j<10; j++){
                if(matrix_map[r]->col[k] == 48+j){
                    if(matrix_map[r-1]->col[k] == ' '){
                        matrix_map[r-1]->col[k] = '+';
                    }
                }
            }
        }
    }
}

void gioco::traiettoria_volante(){//funzione che gestisce la traiettoria degli spari nemici
    for(int j=0; j<height; j++){
        if(matrix_map[69]->col[j] == '.'){
            int cnt = 0;
            while(matrix_map[69-cnt]->col[j] == '.' && cnt < 13){
                if(matrix_map[69-cnt]->col[j] == '.'){
                    matrix_map[69-cnt]->col[j] = ' ';
                    cnt++;
                }
                else{
                    matrix_map[69-cnt-2]->col[j] = ' ';
                    cnt = cnt+2;

                }
            }
        }
    }
}

void gioco::contatto_nemico(){//funzione che gestisce il contatto con un proiettile nemico

        if( matrix_map[n]->col[f] == '+'){
        score = score -50;
        vite = vite - 1;
    }
}

void gioco::draw_matrice() {//funzione che gestisce la grafica del menù di gioco
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
                cout << matrix_map[i]->col[j];
        }
        cout << "\n";
    }
        setCursorPosition(n, f);
        cout << "K";
        setCursorPosition(75, 2);
        cout << " SCORE: " << score;
        setCursorPosition(75, 3);
        cout << " LEVEL: " << level ;
        setCursorPosition(75, 4);
        cout << " VITE: " << vite ;
        setCursorPosition(75, 1);
        cout << "-----------";
        setCursorPosition(75, 5);
        cout << "-----------";
}
