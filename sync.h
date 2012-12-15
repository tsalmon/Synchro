#ifndef _SYNC_H_
#define _SYNC_H_

#include <sys/stat.h>
#include <unistd.h>

#define OPT_R(n) ( n        >= 1000)
#define OPT_N(n) ( n % 1000 >= 100 )
#define OPT_I(n) ( n % 100  >= 10  )
#define OPT_S(n) ( n % 10   >= 1   )

struct stat dest_stat;

int lstat(const char  *path, struct stat *buf);
ssize_t readlink(const char *patch, char *buf, size_t bufsize);
int symlink(const char* path1, const char* patch2); 

void option_n(struct stat buf[], struct dirent **fichier, char* dest, char *src, int options, char **way);
void option_r(char **way, char *src, char *dest, struct dirent **fichier, int options);

int synchro(char *src, char *dest, int options);
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
