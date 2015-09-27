public class Treap
{
    public int x;
    public int y;
    
    public Treap Left;
    public Treap Right;    
    
    private Treap(int x, int y, Treap left = null, Treap right = null)
    {
    this.x = x;
    this.y = y;
    this.Left = left;
    this.Right = right;
    }
    
    public static Treap Merge(Treap L, Treap R)
    {
    if (L == null) return R;
    if (R == null) return L;
    
    if (L.y > R.y)
        {
        var newR = Merge(L.Right, R);
        return new Treap(L.x, L.y, L.Left, newR);
        }
    else
        {
        var newL = Merge(L, R.Left);
        return new Treap(R.x, R.y, newL, R.Right);
        }
    }
}
