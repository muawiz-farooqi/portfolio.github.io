int climbStairs(int n){
    int l = 0, r = 1, tmp;
    for (int i = 1; i <= n; i++){
        tmp = r;
        r += l;
        l = tmp;
    }
    return r;
}