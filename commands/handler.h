/*
        Fait le necessaire etant donne une commande
*/
int handleCommand(char *cmd);

/*	Envoie l'argument (potentiellement compose de plusieurs mots dans des
   guillemets suivants) Renvoie -1 en cas de syntaxe invalide ou de fin de liste
*/
int strtokToFullSingleArg(char *cmd, char *motPtr, char *arg, int *i);

/*
        Divise une commande (cmd) en un tableau d'arguments (args, de taille
   definie a l'avance)
*/
int splitStringCmd(char *cmd, char **args, int *len);

int maincp(int argc, char *argv[]);