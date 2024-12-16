#define rep(i, N) for(i = 0; i < N; i++)
#define ll long long

/*重みに使用する型(ll or mint)*/
using type = ll;

class dsup {
   private:
   ll N;
   vector<ll> P;   /*親ノード*/
   vector<ll> S;   /*連結成分のサイズ*/
   vector<type> W; /*重み*/
   pair<ll, type> root(ll now) {
      if(now != P[now]) {
         pair<ll, type> ret = root(P[now]);
         P[now] = ret.first;
         W[now] += ret.second;
      }
      return {P[now], W[now]};
   }

   public:
   dsup(ll n) {
      N = n;
      P = vector<ll>(N + 1);
      for(ll i = 0; i <= N; i++) P[i] = i;
      S = vector<ll>(N + 1, 1);
      W = vector<type>(N + 1, 0);
   }
   /*a+w=bとして連結する*/
   bool merge(ll a, ll b, type w) {
      root(a);
      root(b);
      w += W[a] - W[b];
      a = P[a];
      b = P[b];
      if(a == b) return w == 0;
      if(S[a] <= S[b]) {
         P[b] = a;
         S[a] += S[b];
         W[b] = w;
      } else {
         P[a] = b;
         S[b] += S[a];
         W[a] -= w;
      }
      return 1;
   }
   /*a,bの連結判定*/
   bool same(ll a, ll b) {
      a = root(a).first;
      b = root(b).first;
      return a == b;
   }
   /*a連結成分のsizeを返す*/
   ll size(ll a) { return S[root(a).first]; }
   /*全ての連結成分を列挙(1-indexed)*/
   vector<vector<ll>> groups(void) {
      map<ll, vector<ll>> m;
      vector<vector<ll>> ret;
      for(ll i = 1; i <= N; i++) m[root(i).first].push_back(i);
      for(auto e : m) ret.push_back(e.second);
      return ret;
   }
   /*idのpotentialを求める*/
   type get_p(ll id) { return root(id).second; }
};