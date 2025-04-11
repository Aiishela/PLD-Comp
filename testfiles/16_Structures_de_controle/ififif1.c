int main (){
    int var1=0;
    int a = 0;
    int b = 0;
    if (var1==0){
        if (a!=0) {
            if (b==0){
                return 0;
            }  
        }
        else
        {
            if(b==0)
            {
                return 1; // output attendu
            }
            else 
            {
                return 2;
            }
        }
    }
    else 
    {
        return 3;
    }
}