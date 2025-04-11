int main(){
    int a[] = {1,2,3};
    a[2] = 12 * 48 + a[1];
    return a[2] + a[1] + a[0]; 
}