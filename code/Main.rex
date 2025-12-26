
u16 main(){
    u16[10] a;
    
    for u16 i = 0,i<10,i+=1 {
        a[i] = i;
    }

    u16 sum = 0;
    for u16 i = 0,i<10,i+=1 {
        sum += a[i];
    }
    
    return sum;
}