// using article: http://www.drdobbs.com/database/ternary-search-trees/184410528?pgno=1


struct TernarySearchTree {

    struct Node;

    using UN = unique_ptr<Node>;

    struct Node {
        char ch;
        UN loKid;
        UN eqKid;
        UN hiKid;
    };


    static bool search(const UN& n, char* s) const {
        if (!n) return false; 
        if (*s == 0) return true;
        
        if (*s < n->ch) return search(n->loKid, s);
        if (*s > n->ch) return search(n->hiKid, s);
        // eq
        return search(n->eqKid, ++s);
    }

    /*
    bool search(const UN& n, char* s) {
        auto p = n.get();
        
        while (p) {
            if (*s < p->ch) {
                p = p->loKid.get();
            } else if (*s == p->ch) {
                if (*(s+1) == 0) return true;
                p = p->eqKid;
            } else {
                p = p->hiKid;
            }
        }
        return false;
    }
    */

    static void insert(UN& n, char* s) {
        if (!n) {
            n.reset(new Node(*s));
        }
        
        if (*s < n->ch) {
            insert(n->loKid, s); 
        } else if (*s == n->ch) {
            if (*s != 0) insert(n->eqKid, ++s);
        } else {
            insert(n->hiKid, s);
        }
    }

    
    bool search(char* s) const {
        return search(root, s);
    }
    
    void insert(char* s) {
        insert(root, s);
    }
    
private:
    UN root;
    
};

/*
Tptr insert(Tptr p, char *s)
 { if (p == 0) { 
        p = (Tptr) malloc(sizeof(Tnode)); 
        p->splitchar = *s; 
        p->lokid = p->eqkid = p->hikid = 0; 
   } 
   if (*s < p->splitchar) 
        p->lokid = insert(p->lokid, s); 
   else if (*s == p->splitchar) { 
        if (*s != 0) 
            p->eqkid = insert(p->eqkid, ++s); 
   } else
        p->hikid = insert(p->hikid, s); 
   return p; 
} 


if (*s == 0) 
    p->eqkid = (Tptr) insertstr; 
else
    p->eqkid = insert(p->eqkid, ++s);


void traverse(Tptr p) 
{    if (!p) return; 
     traverse(p->lokid); 
     if (p->splitchar) 
         traverse(p->eqkid); 
     else
         printf("%s/n", (char *) p->eqkid); 
     traverse(p->hikid); 
} 




srchtop = 0; 
pmsearch(root, ".a.a.a"); 

void pmsearch(Tptr p, char *s) 
{    if  (!p) return; 
     nodecnt++; 
     if (*s == '.' || *s < p->splitchar) 
        pmsearch(p->lokid, s); 
     if (*s == '.' || *s == p->splitchar) 
        if (p->splitchar && *s) 
            pmsearch(p->eqkid, s+1); 
     if (*s == 0 && p->splitchar == 0) 
            srcharr[srchtop++] = 
                (char *) p->eqkid; 
     if (*s == '.' || *s > p->splitchar) 
            pmsearch(p->hikid, s); 
}


void nearsearch(Tptr p, char *s, int d) 
{   if (!p || d < 0) return; 
    if (d > 0 || *s < p->splitchar) 
        nearsearch(p->lokid, s, d); 
    if (p->splitchar == 0) { 
       if ((int) strlen(s) <= d) 
          srcharr[srchtop++] = (char *) p->eqkid; 
    } else
       nearsearch(p->eqkid, *s ? s+1:s, 
          (*s == p->splitchar) ? d:d-1); 
    if (d > 0 || *s > p->splitchar) 
        nearsearch(p->hikid, s, d); 
} 

*/
