//github link: https://github.com/usausamikann/usausamikann-library
//dual_segtree:区間変更・一点取得に対応
//verified with: https://atcoder.jp/contests/abc477/submissions/79599130
template<typename S, typename F, auto mapping, auto composition, auto id>
struct dual_seg{
    private:
    //debug
    static_assert(is_invocable_r_v<S,decltype(mapping),F,S>, "mapping must work as (F,S)->S");
    static_assert(is_invocable_r_v<F,decltype(composition),F,F>, "composition must work as (F,F)->F");
    static_assert(is_invocable_r_v<F,decltype(id)>, "id must work as ()->F");
    //初期テーブル
    int n; //要素数
    int level; //セグ木の高さ
    vector<S> val; //値
    vector<F> lazy; //遅延
    //各種内部関数
    void update(int idx, const F& f){ //lazy[idx]に作用fを加える
        lazy[idx]=composition(f,lazy[idx]);
    }
    void push(int idx){ //lazy[idx]の遅延値を子に押し付ける
        assert(idx<n);
        update(2*idx,lazy[idx]); update(2*idx+1,lazy[idx]);
        lazy[idx]=id();
    }

    public:
    //_n個の要素のテーブルをeで埋める
    dual_seg(int _n, S e){
        n=1;
        level=1;
        while(n<_n){
            n*=2;
            level++;
        }
        val.resize(n,e);
        lazy.resize(2*n,id());
    }
    //初期要素で埋まったテーブルを渡す
    dual_seg(const vector<S>& table){
        n=1;
        level=1;
        while(n<(int)table.size()){
            n*=2;
            level++;
        }
        val.resize(n,S());
        for(int i=0; i<(int)table.size(); i++){ val[i]=table[i]; }
        lazy.resize(2*n,id());
    }
    //区間[l,r)にfを作用
    void apply(int l, int r, const F& f){
        if(l==r){ return; }
        l+=n;
        r+=n;
        //両端について、祖先から遅延作用をおろす
        for(int lv=level-1; lv>=1; lv--){
            //最下段よりlv段上のnodeから子へと遅延作用をpush
            push(l>>lv); push((r-1)>>lv);
        }
        //遅延作用を更新(ここは可換なときの双対セグ木とほぼ同じ)
        while(l<r){
            if(l&1){
                update(l,f);
                l++;
            }
            if(r&1){
                r--;
                update(r,f);
            }
            l>>=1;
            r>>=1;
        }
    }
    //全区間にfを作用
    void all_apply(const F& f){
        update(1,f);
    }
    //一点代入: val[idx]=xとする
    void set(int idx, const S& x){
        idx+=n;
        //祖先のlazyを全部pushしてからvalを更新
        for(int lv=level-1; lv>=1; lv--){
            push(idx>>lv);
        }
        val[idx-n]=x;
        lazy[idx]=id();
    }
    //一点取得
    S get(int idx){
        idx+=n;
        //祖先のlazyｗ全部push
        for(int lv=level-1; lv>=1; lv--){
            push(idx>>lv);
        }
        return mapping(lazy[idx],val[idx-n]);
    }
};
