/* Added to make it compile */
typedef double Point[3];
typedef double Vector[3];
struct Ray {
    Point O;
    Point D;
    Point P;
    Vector normal;

} ;
struct Polygon {
        int n;
        bool interpolate;
        Point *V;

};

bool intersect(struct Polygon poly, struct Ray ray, float t, int i1, int i2);
