int main(){
    int a = 2;
    int b = 2;
    int c;
    if(a == b){
        c = 1;
        a = 1;
    }
    if(a + b == 3){
        c = 2;
        a = 10;
    }
    else{
        c = 0;
    }
    return c + a + b;
}