int calc(int a, int b) {
    int res = 0;

    // Arithmétique de base
    res = a + b;
    res -= a;
    res *= 2;
    res /= (b + 1); 
    res %= 5;

    // Opérateurs unaires
    res = -res;

    // Bit à bit
    res = res | 2;
    res = res & 7;
    res = res ~ 3;

    // Comparaisons
    int c1 = (a == b);
    int c2 = (a != b);
    int c3 = (a < b);
    int c4 = (a > b);

    // Logiques paresseux
    int logic = (c1 && c2) || (c3 && c4);

    // Affectations composées
    res += logic;
    res -= 1;
    res *= 3;
    res /= 2;
    res %= 4;

    // Incrémentation / décrémentation
    a++;
    b--;
    ++res;
    --res;

    return res;
}

int main() {
    int x = 5;
    int y = 3;
    int result = calc(x, y);
    return result;
}
