#ifndef _SYNC_H_
#define _SYNC_H_

#include <sys/stat.h>
#include <unistd.h>

#define OPT_R(n, d) (( n        >= 1000) && S_ISDIR(d.st_mode))
#define OPT_N(n) (n % 1000 >= 100 )
#define OPT_I(n)    ( n % 100  >= 10 )
#define OPT_S(n, l) (( n % 10   >= 1 ) && S_ISLNK(buf[0].st_mode))

typedef struct dirent dirent;

struct stat dest_stat;
int options;
struct stat buf[2];
int lstat(const char  *path, struct stat *buf);
ssize_t readlink(const char *patch, char *buf, size_t bufsize);
int symlink(const char* path1, const char* patch2); 

void option_n(char *way[], dirent *fichier[],char *dest, char *src);
void option(int no_exist, char **way, char *src, char *dest, dirent *fichiers[]);

char *realpath(const char *file_name, char *resolved_name);

void synchro(char *src, char *dest);
int synchro_droits(char *src, struct stat *droits, int droits_init);
void synchro_open(int desc[], DIR *dir[], char *src, char *dest);
int synchro_recherche(char **w, char *s, char *d, dirent *f[]);
void option_s(char *src, char *dest, char **way, dirent *fichier[]);
void option_r(char **way, char *dest, char *src, dirent *fichier[]);

char *addstr(char *str, char *str2, char sep);
int est_sousrep(char *str);
int getstat(char *str, struct stat *buffer);
int modifier_fichier(char **way);
int modifier_binaire_binaire(char **way);
char *saisie();
int reponse_positive(char *str);
void modifier(char **way, int options);
void ajouter(char *dest, char *src ,char *fichier);
#endif
