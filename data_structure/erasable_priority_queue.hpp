//以下、昇順管理・降順管理の削除可能priority_queueを載せています　用途ごとにお使いください

//github link: https://github.com/usausamikann/usausamikann-library
//erasable_maxpq:降順管理priority_queue+削除操作
//verified with: https://judge.yosupo.jp/submission/413694
template<typename T>
struct erasable_maxpq{
    private:
    using maxpq=priority_queue<T,vector<T>,less<T>>;
    maxpq que,del;
    //削除操作
    void private_erase(){
        while(!que.empty() && !del.empty() && que.top()==del.top()){
            que.pop();
            del.pop();
        }
    }
    public:
    void push(const T& key){
        que.push(key);
    }
    void erase(const T& key){
        del.push(key);
    }
    T top(){
        private_erase();
        assert(!que.empty() && "空のキューに対してtop()を呼び出しています");
        return que.top();
    }
    void pop(){
        private_erase();
        assert(!que.empty() && "空のキューに対してpop()を呼び出しています");
        que.pop();
    }
    bool empty(){
        private_erase();
        return que.empty();
    }
};

//github link: https://github.com/usausamikann/usausamikann-library
//erasable_minpq:昇順管理priority_queue+削除操作
//verified with: https://judge.yosupo.jp/submission/413694
template<typename T>
struct erasable_minpq{
    private:
    using minpq=priority_queue<T,vector<T>,greater<T>>;
    minpq que,del;
    //削除操作
    void private_erase(){
        while(!que.empty() && !del.empty() && que.top()==del.top()){
            que.pop();
            del.pop();
        }
    }
    public:
    void push(const T& key){
        que.push(key);
    }
    void erase(const T& key){
        del.push(key);
    }
    T top(){
        private_erase();
        assert(!que.empty() && "空のキューに対してtop()を呼び出しています");
        return que.top();
    }
    void pop(){
        private_erase();
        assert(!que.empty() && "空のキューに対してpop()を呼び出しています");
        que.pop();
    }
    bool empty(){
        private_erase();
        return que.empty();
    }
};
