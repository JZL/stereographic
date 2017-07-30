/* Added to make it compile */
typedef double Point[3];
typedef double Vector[3];
struct Ray {
    Point O; //Origin
    Point D; //Is the Slope
    Point P; //Dest

} ;
struct Polygon {
        int n;
        bool interpolate;
        Point *V;

};

struct coord2D {
        double X;
        double Y;
};

bool intersect(struct Polygon *poly, struct Ray *ray, float t, int i1, int i2, int v);
