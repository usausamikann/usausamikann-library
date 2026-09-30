//github link: https://github.com/usausamikann/usausamikann-library
//mymultiset:std::multisetをsplaytreeで再実装
//verified with: https://atcoder.jp/contests/abc475/submissions/79283254
template<typename K>
struct mymultiset{
    private:
    //0. node関連
    struct info{ //node情報
        K key;
        int pnt,cnt,sz;
        array<int,2> cld;
        info() : key(K{}),pnt(0),cnt(0),sz(0),cld(array<int,2>{0,0}){}
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
        node[res].cnt=1;
        node[res].sz=1;
        return res;
    }
    //1. splay関連
    int root; //初期値は0
    void update(int id){ //node[id]のcnt再計算
        if(!id){ return; }
        node[id].sz=node[id].cnt;
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
                node[root].cnt++;
                node[root].sz++;
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
    // ↓idのノードそのものを、cntによらず木から削除する
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

    mymultiset(){
        node.push_back(info());
        root=0;
    }

    //1. イテレーター関連 -> 今回は実装せず
    //イテレーターを実装していないためlower boundなどの返り値の型に注意

    //2. 要素の検索・挿入・削除

    //exist:keyが存在するか
    bool exist(const K& key){
        return count(key)!=0;
    }
    //count:keyの個数
    int count(const K& key){
        int res=private_search(key,1);
        if(!res || node[res].key!=key){ return 0; }
        else{ return node[res].cnt; }
    }
    //lower_bound:{key以上の値が存在するか, 存在する場合その値}を返す
    pair<bool,K> lower_bound(const K& key){
        int res=private_search(key,1);
        if(!res){ return {false,K{}}; }
        else return {true,node[res].key};
    }
    //upper_bound:{keyより大きい値が存在するか, 存在する場合その値}を返す
    pair<bool,K> upper_bound(const K& key){
        int res=private_search(key,0);
        if(!res){ return {false,K{}}; }
        else return {true,node[res].key};
    }
    void insert(const K& key){
        private_insert(key);
    }
    //erase_one:keyを1つだけ削除
    void erase_one(const K& key){
        int res=private_search(key,1);
        if(!res || node[res].key!=key){ return; } //そもそもノードが存在しない
        node[res].cnt--;
        node[res].sz--;
        if(node[res].cnt==0){ //要素が0個になった
            private_erase(res);
        }
        return;
    }
    //erase_all:keyを全て削除
    void erase_all(const K& key){
        int res=private_search(key,1);
        if(!res || node[res].key!=key){ return; } //そもそもノードが存在しない
        private_erase(res);
        return;
    }

    //3. sz関連

    int size(){ return node[root].sz; }
    bool empty(){ return root==0; }
    //cnt_from:key以上の要素の個数を返す
    int cnt_from(const K& key){
        int res=private_search(key,1);
        if(!res){ return 0; }
        return node[root].sz-node[node[root].cld[0]].sz;
    }
    //cnt_over:keyより大きい要素の個数を返す
    int cnt_over(const K& key){
        int res=private_search(key,0);
        if(!res){ return 0; }
        return node[root].sz-node[node[root].cld[0]].sz;
    }
    //find_by_order:昇順id番目(0-index)の要素を返す
    K find_by_order(int id){
        assert(id>=0 && id<size());
        int cur=root;
        while(cur){
            int left_sz=node[node[cur].cld[0]].sz;
            if(id<left_sz){
                cur=node[cur].cld[0];
            }
            else if(id<left_sz+node[cur].cnt){
                splay(cur);
                return node[cur].key;
            }
            else{
                id-=left_sz+node[cur].cnt;
                cur=node[cur].cld[1];
            }
        }
        return K{}; //一応
    }
};
