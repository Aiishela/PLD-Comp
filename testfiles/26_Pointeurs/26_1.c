int main() {
    int x = 10;
    int* ptr = &x;  // Pointeur vers `x`
    *ptr = 20;  // Modification de la valeur de `x` via le pointeur
    return x;  // Retourne 20, car `x` a été modifié
}