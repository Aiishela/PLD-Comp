int main() {
    int a = {1, 2, 3};

    if (a[0] < a[1]) {
        a[0] = a[1] + a[2];
    }

    return a[0] + a[1] + a[2]; 
}