int main() {
    int a[3] = {1, 2, 3};
    int i = 0;

    while (i < 3) {
        a[i] = a[i] * 2; 
        i++;
    }

    return a[0] + a[1] + a[2]; 
}
