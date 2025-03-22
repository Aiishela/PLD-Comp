int main() {
    int a = 30;
    int* ptr = &a;
    int** ptr2 = &ptr;  // Pointeur sur pointeur
    **ptr2 = 40;        // Modification de a via le double pointeur
    return a;           // Doit retourner 40
}