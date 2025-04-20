# PLD-Comp

Hexanome : H2442  
Membres :
- BARATOVA Malika - Cheffe de projet
- SAADALLA Nadine
- COCHET Mélisse
- VARJABEDIAN Noémie
- CHARLOTTE Matéo
- DOAN Adrien
- HUE Simon

## Lancement du code

> Depuis le dossier compiler.  

Afin de compiler le code source en l'exécutable ifcc, faites : 
```
make
```

Ensuite, vous pouvez compiler un fichier test avec :
```
./ifcc ../testfiles/02_Variables/2_1.c
```

Cette dernière commande redirige la sortie (code assembleur) vers asm-ifcc.s qui sera utilisé par notre script shell pour executer le code assembleur généré : 
```
./s_exec.sh
```

Pour éxécuter tous les tests, exécuter la commande :
```
../ifcc-test.py ../testfiles/
```
