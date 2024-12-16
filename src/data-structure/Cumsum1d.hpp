struct Cumsum1d {
   vector<int> sum;
   Cumsum1d(const vector<int>& a) {
      sum.resize(a.size() + 1);
      for(int i = 0; i < a.size(); i++) { sum[i + 1] = sum[i] + a[i]; }
   }
   // Returns the sum of the elements in the range [l, r).
   int query(int l, int r) { return sum[r] - sum[l]; }
};