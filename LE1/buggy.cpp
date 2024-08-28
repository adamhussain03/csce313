#include <iostream>
#include <cstring>

struct Point {
    int x, y;

    Point () : x(), y() {}
    Point (int _x, int _y) : x(_x), y(_y) {}
};

class Shape {
    int vertices;
    Point** points;

    public:
        Shape (int _vertices) {
            vertices = _vertices;
            points = new Point*[vertices+1];
        }

        ~Shape () {
            for(int i = 0;i<vertices;i++){
                delete points[i];
            }
            delete[] points;
        }

        void addPoints (Point* pts){
            for (int i = 0; i < vertices; i++) {
                points[i] = new Point();
                memcpy(points[i], &pts[i%vertices], sizeof(Point));
            }
        }

        double area () {
            int temp = 0;
            for (int i = 0; i < vertices-1; i++) {
                // FIXME: there are two methods to access members of pointers
                //        use one to fix lhs and the other to fix rhs
                int lhs = points[i]->x * points[i+1]->y;
                int rhs = (*points[i+1]).x * (*points[i]).y;
                temp += (lhs - rhs);
            }
            double area = abs(temp)/2.0;
            return area;
        }
};

int main () {
    // FIXME: create the following points using the three different methods
    //        of defining structs:
    //          tri1 = (0, 0)
    //          tri2 = (1, 2)
    //          tri3 = (2, 0)

    Point tri1 = {0,0};
    Point tri2;
    tri2.x = 1;
    tri2.y = 2;
    Point tri3(2,0);
    //std::cout<<"check 1"<<std::endl;



    // adding points to tri
    Point triPts[3] = {tri1, tri2, tri3};
    Shape* tri = new Shape(3);
    //std::cout<<"check 2"<<std::endl;
    (*tri).addPoints(triPts);
    //std::cout<<"check 3"<<std::endl;

    // FIXME: create the following points using your preferred struct
    //        definition:
    //          quad1 = (0, 0)
    //          quad2 = (0, 2)
    //          quad3 = (2, 2)
    //          quad4 = (2, 0)
    //std::cout<<"check 4"<<std::endl;
    Point quad1(0,0);
    Point quad2(0,2);
    Point quad3(2,2);
    Point quad4(2,0);


    // adding points to quad
    //std::cout<<"check 5"<<std::endl;
    Point quadPts[4] = {quad1, quad2, quad3, quad4};
    Shape* quad = new Shape(4);
    (*quad).addPoints(quadPts);

    //std::cout<<"check 6"<<std::endl;
    std::cout<<"Triangle area: "<<(*tri).area()<<std::endl;
    std::cout<<"Square area: "<<(*quad).area()<<std::endl;

    delete tri;
    delete quad;
}
