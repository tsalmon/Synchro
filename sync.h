#ifndef _SYNC_H_
#define _SYNC_H_

#define OPT_R(n) ( n        >= 1000)
#define OPT_N(n) ( n % 1000 >= 100 )
#define OPT_I(n) ( n % 100  >= 10  )
#define OPT_S(n) ( n % 10   >= 1   )

struct stat dest_stat;

int sync(char *src, char *dest, int options);
int sync_rec(char *src, char *dest, int optopns);
char *addstr(char *str, char *str2, char sep);
int est_sousrep(char *str);
void getstat(char *str, struct stat *buffer);
int modifier_fichier(char **way);
int modifier_binaire_binaire(char **way);
char *saisie();
int reponse_positive(char *str);
void modifier(char **way, int options);
void ajouter(char *dest, char *src ,char *fichier);
#endif
