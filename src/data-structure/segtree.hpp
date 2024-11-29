using namespace std;
#define rep(i,N) for(i=0;i<N;i++)
#define ll long long

/*type:セグ木で使う型*/
using type = ll;
/*op:セグ木に載せたい演算*/
type op(type a,type b){return min(a,b);}
/*e:opを用いてもデータを変化させない値*/
type e(void){return INT64_MAX;}

class segtree{
private:
    ll N;
    vector<type>tree;
public:
    /*0-indexed,1-indexed両対応*/
    segtree(ll tree_size){
        N=tree_size;
        tree.resize(2*(N+1),e());
    }
    /*全体にopを適用する*/
    type all_prod(void){return tree[1];}
    /*[l,r)の区間演算*/
    type prod(ll l,ll r){
        l+=N;r+=N;
        type res;
        type left=e();
        type right=e();
        while(l<r){
            if(l%2==1){left=op(left,tree[l]);l++;}
            if(r%2==1){r--;right=op(tree[r],right);}
            l/=2;r/=2;
        }
        res=op(left,right);
        return res;
    }
    /*idに値valを代入する*/
    void set(ll id,type val){
        id+=N;
        tree[id]=val;
        while(id>1){
            id/=2;
            tree[id]=op(tree[2*id],tree[2*id+1]);
        }
    }
    /*idの値を取得する*/
    type get(ll id){return tree[N+id];}
};