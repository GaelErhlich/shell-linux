void testsStrtokToFullStringArg() {
    char cmd[128] = "Hier j'ai voulu manger une pomme de type \"pomme golden\" et, c'est de mon point de vue exquis.";

    char* args[64];
    int nbArgs;
    splitStringCmd(cmd, args, &nbArgs);

    if (strcmp(args[0], "Hier")) { printf("Erreur : splitStringCmd aurait du renvoyer 'Hier'. (%s)\n", args[0]); }
    if (strcmp(args[8], "pomme golden")) { printf("Erreur : splitStringCmd aurait du renvoyer 'pomme golden'. (%s)\n", args[8]); }
    if (strcmp(args[9], "et,")) { printf("Erreur : splitStringCmd aurait du renvoyer 'et,'. (%s)\n", args[9]); }
    if (strcmp(args[16], "exquis.")) { printf("Erreur : splitStringCmd aurait du renvoyer 'exquis.' (%s)\n", args[16]); }
    if (nbArgs != 17) { printf("Erreur : splitStringCmd aurait du renvoyer une longueur de 17. (%d).\n", nbArgs); }

}