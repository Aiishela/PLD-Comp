int main(){
    int a[] = {0,1,2};
    a[a[1]] += a[1] + 45;
    a[a[2]] *= a[0] ;

    return a[0] + a[1] + a[2]; 
}