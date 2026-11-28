//github link: https://github.com/usausamikann/usausamikann-library
//sweep_2d:**静的な**2次元平面上の点（重み）追加・矩形領域内の総重量取得
//verified with: https://judge.yosupo.jp/submission/420130, https://atcoder.jp/contests/abc405/submissions/81112449
struct sweep_2d{
    private:
    using ll=long long;
    //座圧用
    //mylb:lower_bound
    template <typename Container, typename T> int mylb(const Container& A, const T& key){
        auto it=lower_bound(A.begin(),A.end(),key); return distance(begin(A),it);
    }
    //st_uni:sort+重複削除
    template <typename T> void st_uni(vector<T> &v){ sort(v.begin(),v.end()); v.erase(unique(v.begin(),v.end()),v.end()); }
    //永続セグ木
    struct pseg_for_sweep_2d{
        private:
        struct node{
            ll val; int left_id,right_id;
            node() : val(0LL),left_id(-1),right_id(-1){}
        };
        vector<node> pool;
        int make_node(ll val, int l_id, int r_id){
            node new_node=node();
            new_node.val=val; new_node.left_id=l_id; new_node.right_id=r_id;
            pool.push_back(new_node); return (int)pool.size()-1;
        }
        int dfs_add(int now_id, ll l, ll r, ll i, ll x){
            int res=make_node(0,-1,-1);
            if(now_id>=0) pool[res]=pool[now_id];
            pool[res].val+=x;
            if(r-l==1) return res;
            ll m=(l+r)/2;
            if(i<m) pool[res].left_id=dfs_add(pool[res].left_id,l,m,i,x);
            else pool[res].right_id=dfs_add(pool[res].right_id,m,r,i,x);
            return res;
        }
        ll dfs_prod(int now_id, ll l, ll r, ll a, ll b){
            if(now_id==-1) return 0;
            if(r<=a || l>=b) return 0;
            if(a<=l && r<=b) return pool[now_id].val;
            ll m=(l+r)/2;
            return dfs_prod(pool[now_id].left_id,l,m,a,b)+dfs_prod(pool[now_id].right_id,m,r,a,b);
        }
        ll n;
        public:
        pseg_for_sweep_2d(){}
        void get_size(ll _n){ n=_n; }
        int ini_build(){ return make_node(0,-1,-1); }
        int add(int root_id, ll i, ll x){
            assert(0<=i && i<n);
            return dfs_add(root_id,0,n,i,x);
        }
        ll prod(int root_id, ll l, ll r){
            assert(0<=l && l<=r && r<=n);
            if(l==r) return 0;
            return dfs_prod(root_id,0,n,l,r);
        }
    };
    //クエリ処理
    int n; //setされた点の個数
    vector<ll> xcd,ycd,weight; //x座標,y座標,重み
    vector<ll> xnums,ynums; //xcd,ycdにst_uniを施したもの
    pseg_for_sweep_2d seg;
    vector<int> x_to_table; //永続セグ木の根を格納　座圧後のx座標と根が対応
    int build_cnt;
    public:
    //コンストラクタ:代入不要
    sweep_2d(){
        build_cnt=0;
    }
    //set_pnt:(x,y)に重みwの点を追加
    void set_pnt(long long x, long long y, long long w){
        assert(build_cnt==0 && "build()後にset_pntを呼び出しています");
        xcd.push_back(x); ycd.push_back(y); weight.push_back(w);
    }
    //build:1回のみ呼び出し可
    void build(){
        assert(build_cnt==0 && "buildを複数回呼び出しています");
        build_cnt++;
        n=xcd.size(); //n==0のとき、prodで個別処理
        if(n==0) return;
        //x,y座標座圧
        xnums=xcd; st_uni(xnums);
        ynums=ycd; st_uni(ynums);
        //永続セグ木呼び出し
        seg.get_size((int)ynums.size());
        int now=seg.ini_build();
        //x座標昇順に見て、永続セグ木更新->根をx_to_tableに保存
        vector<int> ord(n); iota(ord.begin(),ord.end(),0);
        sort(ord.begin(),ord.end(),[&](int l, int r){ return xcd[l]<xcd[r]; }); //x座標昇順に見ていく
        ll pre_x=0; //初期値はどうでもよく、一旦0で仮置きしている
        for(int id : ord){
            now=seg.add(now,mylb(ynums,ycd[id]),weight[id]);
            if(x_to_table.empty() || xcd[id]!=pre_x){
                x_to_table.push_back(now);
            }
            else{
                x_to_table.back()=now;
            }
            pre_x=xcd[id];
        }
    }
    //prod:矩形領域[xl,xr)[yl,yr)内の総重量を返す
    long long prod(long long xl, long long xr, long long yl, long long yr){
        assert(build_cnt>0 && "buildを呼び出していません");
        assert(xl<=xr && yl<=yr);
        //返り値が必ず0になる場合
        if(n==0) return 0;
        if(xl==xr || yl==yr) return 0;
        if(xr<=xnums[0] || xnums.back()<xl) return 0;
        if(yr<=ynums[0] || ynums.back()<yl) return 0;
        //上記以外のクエリ処理
        int id_xl=mylb(xnums,xl)-1; int id_xr=mylb(xnums,xr)-1;
        int id_yl=mylb(ynums,yl); int id_yr=mylb(ynums,yr);
        long long right=seg.prod(x_to_table[id_xr],id_yl,id_yr);
        long long left=0;
        if(id_xl>=0){
            left=seg.prod(x_to_table[id_xl],id_yl,id_yr);
        }
        return right-left;
    }
};
