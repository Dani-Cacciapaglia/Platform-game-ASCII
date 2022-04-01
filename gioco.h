#include "const.h"

class gioco
{
protected:
    int salto;
    int n;
    int f;
    int score;
    int level;
    int dist_proiettile;
    int vite;
    lista matrix_map[width+1];
    lista matrix_map_prev[width+1];
    lista matrix_map_scheletro[width+1];
    int start;
    char c;
    bool prec;
    int passi_indietro;
public:
    gioco();
	void run();
	void setCursorPosition(int x, int y);
	lista fill(lista head);
	int piattaforma_gia_generata(int riga);
	bool bonus_gia_generato(int riga);
	void bonus(lista head);
	void collisione_nemici();
	int conteggio_piattaforma_versodestra(int j, int i);
    int conteggio_piattaforma_versosinistra(int j, int i);
    bool nemico_gia_generato(int riga);
    bool bonus_12esima(int riga);
    bool bonus_prec(int riga);
    bool distanza_tra_nemici(int riga);
    void nemico(lista head);
    void genera_nemico2();
    void nemico2();
    void collisione_mob_terra();
    void contatto_bonus();
    void add_col(lista head);
    void piattaforme(lista head);
    void salto_e_aderenza();
    void scendi_e_aderenza();
    void caduta_avanti();
    void caduta_dietro();
    void passo_destra(lista head, lista scheletro);
    void passo_sinistra();
    int sparo();
    void vita_nemici(int o);
    void aggiorna_matrice(lista head, int start);
    void aggiorna_matrice_scheletro(lista head, int start);
    void gestione_proiettile();
    void sparo_nemici();
    void traiettoria_volante();
    void contatto_nemico();
    void draw_matrice();
};
