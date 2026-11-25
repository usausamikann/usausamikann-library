
template<typename T>
struct trie{
    private:
    struct node_info{
        T val{};
        map<char,int> cld;
    };
    vector<node_info> node={node_info()};
    const int root=0;
    public:
    //get_root: 根のindexを得る
    int get_root()const{ return root; }
    //insert: 文字列を挿入し、挿入時に訪問したnodeのindexを根から順に返す
    vector<int> insert(const string& s){
        vector<int> res={root};
        int now=root;
        for(int i=0; i<(int)s.size(); i++){
            int nxt=node[now].cld[s[i]];
            if(nxt==0){
                node.push_back(node_info());
                nxt=(int)node.size()-1;
                node[now].cld[s[i]]=nxt;
            }
            now=nxt;
            res.push_back(now);
        }
        return res;
    }
    //operator[id]: indexがidのnodeが持っている値を返す
    T& operator[](int id){
        assert(0<=id && id<(int)node.size());
        return node[id].val;
    }
    const T& operator[](int id) const{
        assert(0<=id && id<(int)node.size());
        return node[id].val;
    }
};
