int start(char * filename, short unsigned * m, short* size_b, short* size_s, short rule_b[8], short rule_s[8]);
int  commento(FILE * f, short unsigned * m, short* size_b, short* size_s, short* rule_b[8], short* rule_s[8]);
int leggi_rule(FILE * f, short unsigned * m, short* size_b, short* size_s, short* rule_b[8], short* rule_s[8]);
void decodifica_rule(char * s,short* size_b, short* size_s, short* rule_b, short* rule_s);
int prepara_matrice(FILE * f, short unsigned * m,int n_x, int n_y);




