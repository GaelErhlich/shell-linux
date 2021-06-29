

/*
	Divise une commande (cmd) en un tableau d'arguments (args, de taille definie a l'avance)
	Renvoie 0 si succes et -1 si syntaxe incorrecte
*/
int splitStringCmd(char* cmd, char** args, int* len);