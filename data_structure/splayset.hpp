//github link: https://github.com/usausamikann/usausamikann-library
//splayset:std::setをsplay treeで実装し、順序統計機能を追加
//verified with: https://atcoder.jp/contests/awc0100/submissions/79254698
template<typename K>
struct splayset{
    private:
    //0. node関連
    struct info{ //node情報
        K key;
        int pnt;
        int sz;
        array<int,2> cld;
    };
    vector<info> node; //ノードプール
    vector<int> garbage_id; //eraseしたノードのid格納
    void dump(int id){
        node[id]=info();
        garbage_id.push_back(id);
    }
    int get_new_node(K key){
        int res=0;
        if(garbage_id.empty()){
            node.push_back(info());
            res=(int)node.size()-1;
        }
        else{
            res=garbage_id.back();
            garbage_id.pop_back();
        }
        node[res].key=key;
        node[res].sz=1;
        return res;
    }
    //1. splay関連
    int root; //初期値は0
    void update(int id){ //node[id]の部分木サイズ再計算
        if(!id){ return; }
        node[id].sz=1;
        for(int i=0; i<2; i++){
            node[id].sz+=node[node[id].cld[i]].sz;
        }
    }
    int get_dir(int id){ //node[id]が親に対してどちらの子か
        if(!id){ return -1; }
        if(!node[id].pnt){ return -1; }
        return node[node[id].pnt].cld[1]==id;
    }
    void set_edge(int pnt, int cld, int dir){ //node[pnt].cld[dir]=cld とする
        if(pnt){ node[pnt].cld[dir]=cld; }
        if(cld){ node[cld].pnt=pnt; }
    }
    void rotate(int id){ //node[id]が一段上がるように回転
        if(!id){ return; }
        int pid=node[id].pnt;
        if(!pid){ return; }
        int gid=node[pid].pnt;
        int dir=get_dir(id);
        int pdir=get_dir(pid);
        set_edge(pid,node[id].cld[1-dir],dir);
        set_edge(id,pid,1-dir);
        set_edge(gid,id,pdir);
        update(pid);
        update(id);
    }
    void splay(int id){ //node[id]を根になるように適切な回転を繰り返す
        if(id==0){ return; }
        if(root==id){ return; }
        while(node[id].pnt){
            int gid=node[node[id].pnt].pnt;
            if(!gid){
                rotate(id);
            }
            else if(get_dir(id)==get_dir(node[id].pnt)){
                rotate(node[id].pnt);
                rotate(id);
            }
            else{
                rotate(id);
                rotate(id);
            }
        }
        root=id;
    }
    //2. 検索・挿入・削除用の内部関数
    int private_search(const K& key, bool lower_fg){ //lower_fg=1のときlower_boundとして、そうでないときupper_boundとして振る舞う関数
        if(!root){ return 0; }
        int cur=root;
        int last=cur;
        int res=0;
        while(cur){
            last=cur;
            if((lower_fg && node[cur].key>=key) || (!lower_fg && node[cur].key>key)){
                res=cur;
                if(node[cur].key==key){ break; }
                cur=node[cur].cld[0];
            }
            else{
                cur=node[cur].cld[1];
            }
        }
        splay(last);
        if(res){ splay(res); }
        return res;
    }
    void private_insert(K key){
        if(!root){
            root=get_new_node(key);
            return;
        }
        int res=private_search(key,1);
        if(res){
            if(node[root].key==key){
                return;
            }
            else{
                int id=get_new_node(key);
                set_edge(id,node[root].cld[0],0);
                set_edge(root,0,0);
                set_edge(id,root,1);
                update(root);
                update(id);
                root=id;
            }
        }
        else{
            int id=get_new_node(key);
            set_edge(id,root,0);
            update(id);
            root=id;
        }
    }
    int private_erase(int id){ //node[id]を根にした上で削除し、node[id].keyより大でかつ最小のキー値を持つnodeのindexを返す
        if(!id){ return 0; }
        splay(id);
        int left=node[id].cld[0];
        int right=node[id].cld[1];
        //idの削除
        set_edge(0,left,0);
        set_edge(0,right,1);
        dump(id);
        //木の再構成
        //右の部分木がない場合
        if(!right){
            root=left;
            return 0;
        }
        //右の部分木がある場合
        int res=right;
        while(node[res].cld[0]){
            res=node[res].cld[0];
        }
        root=right; //孤立した右の部分木内でsplay
        splay(res); //右の部分木の根がresになり、定義よりnode[res].cld[0]=0
        set_edge(res,left,0);
        update(res);
        return res;
    }
    public:
    //0. コンストラクタ

    splayset(){
        node.push_back(info());
        root=0;
    }

    //1. イテレーター関連

    struct iterator{
        public: //erase()での都合上全てpublic
        splayset* tree;
        int id;
        int private_neighbor(bool fg) const{
            assert(tree && "無効なiteratorです"); //一応
            //fg==trueのとき、tree内でnode[id]の次に大きいキー値を持つnodeのindexを返す
            //要は fg:true->next, fg:false->prev として振る舞う
            if(fg){ //end()++は不可
                assert(id && "end()++は実行できません");
            }
            //end()に対する--
            if(!fg && !id){
                int res=tree->root;
                assert(res && "空のsplaysetに対しend()--は呼べません");
                while(tree->node[res].cld[1]){
                    res=tree->node[res].cld[1];
                }
                return res;
            }
            //通常の挙動
            if(tree->node[id].cld[fg]){
                int res=tree->node[id].cld[fg];
                while(tree->node[res].cld[1-fg]){
                    res=tree->node[res].cld[1-fg];
                }
                return res;
            }
            else{
                int res=id;
                while(res){
                    int next=tree->node[res].pnt; //親に戻る
                    if(!next || tree->node[next].cld[1-fg]==res){
                        if(!next){ //next==0が許されるのは最大要素++->返り値がend()の場合のみで、最小要素=begin()--は許されない
                            assert(fg && "begin()--は実行できません");
                        }
                        return next;
                    }
                    res=next;
                }
                return 0; //一応　この行が実行されることはない
            }
        }
        iterator() : tree(nullptr),id(0) {}
        iterator(splayset* _tree, int _id) : tree(_tree),id(_id) {}
        //各種演算子
        bool operator==(const iterator& other) const{
            return tree==other.tree && id==other.id;
        }
        bool operator!=(const iterator& other) const{
            return !(*this==other);
        }
        const K& operator*() const{
            assert(tree && id && "*end()は実行できません");
            return tree->node[id].key;
        }
        explicit operator bool() const{ //このiteratorの指すnodeがend()ではないか
            return tree && id;
        }
        //前置インクリメント:++it,--it
        iterator& operator++(){
            id=private_neighbor(true);
            return *this;
        }
        iterator& operator--(){
            id=private_neighbor(false);
            return *this;
        }
        //後置デクリメント:it++,it--
        iterator operator++(int) {
            iterator res = *this;
            ++(*this);
            return res;
        }
        iterator operator--(int) {
            iterator res = *this;
            --(*this);
            return res;
        }
    };
    iterator begin(){
        int cur=root;
        while(node[cur].cld[0]){
            cur=node[cur].cld[0];
        }
        return iterator(this,cur);
    }
    iterator end(){
        return iterator(this,0);
    }

    //2. 要素の検索・挿入・削除

    iterator find(const K& key){
        int res=private_search(key,1);
        if(!res || node[res].key!=key){ //ノードが存在しない
            return iterator(this,0);
        }
        else{ return iterator(this,res); }
    }
    iterator lower_bound(const K& key){
        int res=private_search(key,1);
        return iterator(this,res);
    }
    iterator upper_bound(const K& key){
        int res=private_search(key,0);
        return iterator(this,res);
    }
    void insert(K key){
        private_insert(key);
    }
    void erase(K key){
        int res=private_search(key,1);
        if(!res || node[res].key!=key){ return; } //そもそもノードが存在しない
        private_erase(res);
    }
    iterator erase(iterator it){
        assert(it.tree == this && "別のsplaysetのイテレーターが引数になっています");
        assert(it.id && "erase(end())は実行できません");
        return iterator(this,private_erase(it.id));
    }

    //3. sz関連

    int size(){ return node[root].sz; }
    bool empty(){ return root==0; }
    //order_of_key:keyが昇順何番目の要素か0-indexで返す(keyがそもそも含まれていない場合は-1を返す)
    int order_of_key(K key){
        if(!find(key)){ return -1; }
        //find(key)が呼ばれているので今node[root].key=key
        return node[node[root].cld[0]].sz;
    }
    //find_by_order:昇順id番目(0-index)の要素を返す(id>=size()の場合はend()を返す)
    iterator find_by_order(int id){
        if(id<0 || id>=size()){ return end(); }
        int cur=root;
        while(cur){ //curを根とする部分木を考える
            int left_sz=node[node[cur].cld[0]].sz;
            if(id==left_sz){ break; }
            if(id<left_sz){
                cur=node[cur].cld[0];
            }
            else{
                id-=left_sz+1;
                cur=node[cur].cld[1];
            }
        }
        splay(cur);
        return iterator(this,cur);
    }
    //cnt_from:key以上の要素の個数を返す
    int cnt_from(const K& key){
        int res=private_search(key,1);
        if(!res){ return 0; }
        else{ return 1+node[node[root].cld[1]].sz; }
    }
    //cnt_over:keyより大きい要素の個数を返す
    int cnt_over(const K& key){
        int res=private_search(key,0);
        if(!res){ return 0; }
        else{ return 1+node[node[root].cld[1]].sz; }
    }
};
