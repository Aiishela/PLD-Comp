int main() {
    int x = 5;
    int y = 1;

    int z = x < 0 || y++ > 0;

    return y; // Doit retourner 2
}
