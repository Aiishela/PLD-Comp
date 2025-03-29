int main() {
    int a = 154;
    int b = 20;
    a %= b + 5 * 14;
    b /= b % 4;
    return a ;
}