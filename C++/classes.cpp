#include <iostream>
#include <vector>
#include <math.h>

class Point {
private:
    double X;
    double Y;

public:
    Point() { X = 0; Y = 0; }

    ~Point() = default;

    Point(const Point &toCopy) {
        this -> X = toCopy.X;
        this -> Y = toCopy.Y;
    }

    void setValues(int16_t XCoordinateInput, int16_t YCoordinateInput) {
        X = XCoordinateInput;
        Y = YCoordinateInput;
    }

    Point &operator=(const Point &other) = default;

    friend std::ostream &operator<<(std::ostream &stream, const Point &pointOutput);

    int16_t getX() const { return X; }

    int16_t getY() const { return Y; }
};

std::ostream &operator<<(std::ostream &stream, const Point &pointOutput) {
    stream << "(" << pointOutput.X << "; " << pointOutput.Y << ")" << std::endl;
    return stream;
}

typedef struct Line {
    const Point &initialPoint;
    const Point &end;
} TLine;

double getDistance(const Point &point1, const Point &point2) {
    int16_t XDifference, YDifference;
    XDifference = point2.getX() - point1.getX();
    YDifference = point2.getY() - point1.getY();
    return sqrt(pow(XDifference, 2) + pow(YDifference, 2));
}

bool isParallel(TLine &line1, TLine &line2) {
    int16_t X1, X2, Y1, Y2;
    double angularCoefficient1, angularCoefficient2;
    X1 = line1.initialPoint.getX();
    X2 = line1.end.getX();
    Y1 = line1.initialPoint.getY();
    Y2 = line1.end.getY();
    angularCoefficient1 = (double) ((double) (Y2 - Y1) / (double) (X2 - X1));
    X1 = line2.initialPoint.getX();
    X2 = line2.end.getX();
    Y1 = line2.initialPoint.getY();
    Y2 = line2.end.getY();
    angularCoefficient2 = (double) ((double) (Y2 - Y1) / (double) (X2 - X1));
    if (angularCoefficient1 == angularCoefficient2) return true;
    else return false;
}

class PolygonalChain {
protected:
    int16_t numOfVertices;
    std::vector<Point> vertices;

public:
    PolygonalChain() { numOfVertices = 0; }

    ~PolygonalChain() = default;

    PolygonalChain(const PolygonalChain &toCopy) {
        this -> numOfVertices = toCopy.numOfVertices;
        this -> vertices = toCopy.vertices;
    }

    PolygonalChain &operator=(const PolygonalChain &other) = default;

    virtual void setValues(int16_t amount, std::vector<Point> pointsInput) {
        numOfVertices = amount;
        vertices = pointsInput;
    }

    virtual double getPerimeter() const {
        double perimeter = 0, currentDistance = 0;
        for (int16_t i = 0; i < numOfVertices; i++) {
            if (i != numOfVertices - 1) {
                currentDistance = getDistance(vertices[i], vertices[i + 1]);
                perimeter += currentDistance;
            }
        }

        return perimeter;
    }
};

class ClosedPolygonalChain: public PolygonalChain {
public:
    ClosedPolygonalChain() { numOfVertices = 0; }

    ~ClosedPolygonalChain() = default;

    ClosedPolygonalChain(const ClosedPolygonalChain &toCopy): PolygonalChain(toCopy) {
        this -> numOfVertices = toCopy.numOfVertices;
        this -> vertices = toCopy.vertices;
    }

    ClosedPolygonalChain &operator=(const ClosedPolygonalChain &other) = default;

    void setValues(int16_t amount, std::vector<Point> pointsInput) override {
        numOfVertices = amount;
        vertices = pointsInput;
        if (pointsInput[0].getX() != pointsInput[numOfVertices - 1].getX() || pointsInput[0].getY() != pointsInput[numOfVertices - 1].getY()) {
            vertices.push_back(pointsInput[0]);
            ++numOfVertices;
        }
    }
};

class Polygon: public ClosedPolygonalChain {
public:
    Polygon() { numOfVertices = 0; }

    ~Polygon() = default;

    Polygon(const Polygon &toCopy): ClosedPolygonalChain(toCopy) {
        this -> numOfVertices = toCopy.numOfVertices;
        this -> vertices = toCopy.vertices;
    }

    Polygon &operator=(const Polygon &other) = default;

    virtual double getArea() const {
        double result = 0;
        for (int16_t i = 0; i < numOfVertices; i++) {
            if (i != numOfVertices - 1) result += vertices[i].getX() * vertices[i + 1].getY();
        }

        for (int16_t j = 0; j < numOfVertices; j++) {
            if (j != numOfVertices - 1) result -= vertices[j].getY() * vertices[j + 1].getX();
        }

        return std::abs(result) / 2;
    }
};

class RegularPolygon: public Polygon {
private:
    void isRegular() {
        double currentSideLength, correctSideLength;
        correctSideLength = getDistance(vertices[0], vertices[1]);
        for (int16_t i = 0; i < numOfVertices; i++) {
            if (i != numOfVertices - 1) {
                currentSideLength = getDistance(vertices[i], vertices[i + 1]);
                if (currentSideLength != correctSideLength) {
                    std::cout << "Error :: the polygon is not regular." << std::endl;
                    exit(0);
                }
            }
        }
    }

public:
    RegularPolygon() { numOfVertices = 0; }

    ~RegularPolygon() = default;

    RegularPolygon(const RegularPolygon &toCopy): Polygon(toCopy) {
        this -> numOfVertices = toCopy.numOfVertices;
        this -> vertices = toCopy.vertices;
        isRegular();
    }

    RegularPolygon &operator=(const RegularPolygon &other) = default;

    void setValues(int16_t amount, std::vector<Point> pointsInput) override {
        numOfVertices = amount;
        vertices = pointsInput;
        if (pointsInput[0].getX() != pointsInput[numOfVertices - 1].getX() || pointsInput[0].getY() != pointsInput[numOfVertices - 1].getY()) {
            vertices.push_back(pointsInput[0]);
            ++numOfVertices;
        }

        isRegular();
    }
};

class Triangle: public Polygon {
private:
    void isCorrect() {
        double length1, length2, length3;
        length1 = getDistance(vertices[0], vertices[1]);
        length2 = getDistance(vertices[1], vertices[2]);
        length3 = getDistance(vertices[2], vertices[0]);
        if (length1 >= length2 + length3 || length2 >= length1 + length3 || length3 >= length1 + length2 || vertices.size() > 4) {
            std::cout << "Error :: the triangle is not correct." << std::endl;
            exit(0);
        }
    }

public:
    Triangle() { numOfVertices = 0; }

    ~Triangle() = default;

    Triangle(const Triangle &toCopy): Polygon(toCopy) {
        this -> numOfVertices = toCopy.numOfVertices;
        this -> vertices = toCopy.vertices;
        isCorrect();
    }

    Triangle &operator=(const Triangle &other) = default;

    void setValues(std::vector<Point> pointsInput) {
        numOfVertices = 3;
        vertices = pointsInput;
        if (pointsInput[0].getX() != pointsInput[numOfVertices - 1].getX() || pointsInput[0].getY() != pointsInput[numOfVertices - 1].getY()) {
            vertices.push_back(pointsInput[0]);
            ++numOfVertices;
        }

        isCorrect();
    }
};

class Trapezoid: public Polygon {
private:
    void isCorrect() {
        TLine base1 = {vertices[0], vertices[1]}, base2 = {vertices[2], vertices[3]};
        TLine side1 = {vertices[3], vertices[0]}, side2 = {vertices[1], vertices[2]};
        if (!(isParallel(base1, base2)) || isParallel(side1, side2) || vertices.size() > 5) {
            std::cout << "Error :: the trapezoid is not correct." << std::endl;
            exit(0);
        }
    }

public:
    Trapezoid() { numOfVertices = 0; }

    ~Trapezoid() = default;

    Trapezoid(const Trapezoid &toCopy): Polygon(toCopy) {
        this -> numOfVertices = toCopy.numOfVertices;
        this -> vertices = toCopy.vertices;
        isCorrect();
    }

    Trapezoid &operator=(const Trapezoid &other) = default;

    void setValues(std::vector<Point> pointsInput) {
        numOfVertices = 4;
        vertices = pointsInput;
        if (pointsInput[0].getX() != pointsInput[numOfVertices - 1].getX() ||
            pointsInput[0].getY() != pointsInput[numOfVertices - 1].getY()) {
            vertices.push_back(pointsInput[0]);
            ++numOfVertices;
        }

        isCorrect();
    }
};

class Polynomial {
private:
    std::vector<double> coefficients;

public:
    Polynomial() { coefficients.resize(1); coefficients = {0}; }

    ~Polynomial() = default;

    Polynomial(const Polynomial &toCopy) {
        coefficients = toCopy.coefficients;
    }

    Polynomial &operator=(const Polynomial &other) = default;

    void setValues(std::vector<double> inputCoefficients) {
        coefficients = inputCoefficients;
    }

    friend bool operator==(const Polynomial &polynomial1, const Polynomial &polynomial2);

    friend bool operator!=(const Polynomial &polynomial1, const Polynomial &polynomial2);

    friend Polynomial operator+(const Polynomial &polynomial1, const Polynomial &polynomial2);

    friend Polynomial operator-(const Polynomial &polynomial1, const Polynomial &polynomial2);

    friend Polynomial operator/(const Polynomial &polynomial1, double &polynomial2);

    friend Polynomial operator*(const Polynomial &polynomial1, const Polynomial &polynomial2);

    friend std::ostream &operator<<(std::ostream &stream, const Polynomial &polynomialOutput);

    friend std::istream &operator>>(std::istream &stream, Polynomial &polynomialInput);

    Polynomial operator+=(const Polynomial &other) {
        *this = *this + other;
        return *this;
    }

    Polynomial operator-=(const Polynomial &other) {
        *this = *this - other;
        return *this;
    }

    Polynomial operator*=(const Polynomial &other) {
        *this = *this * other;
        return *this;
    }

    Polynomial operator/=(double valueInput) {
        *this = *this / valueInput;
        return *this;
    }

    Polynomial operator-() const {
        Polynomial result = *this;
        for (int16_t i = 0; i < coefficients.size(); i++) {
            result.coefficients[i] = -coefficients[i];
        }

        return result;
    }

    double operator[](int16_t degreeIndex) {
        if (degreeIndex > coefficients.size() || degreeIndex < 0) {
            std::cout << "Error :: invalid polynomial index." << std::endl;
            exit(0);
        }

        else return coefficients[degreeIndex];
    }
};

bool operator==(const Polynomial &polynomial1, const Polynomial &polynomial2) {
    if (polynomial1.coefficients.size() == polynomial2.coefficients.size() && polynomial1.coefficients == polynomial2.coefficients) return true;
    else return false;
}

bool operator!=(const Polynomial &polynomial1, const Polynomial &polynomial2) {
    if (polynomial1.coefficients.size() == polynomial2.coefficients.size() && polynomial1.coefficients == polynomial2.coefficients) return false;
    else return true;
}

Polynomial operator+(const Polynomial &polynomial1, const Polynomial &polynomial2) {
    Polynomial result = polynomial1;
    int16_t polynomialDegree1 = polynomial1.coefficients.size();
    int16_t polynomialDegree2 = polynomial2.coefficients.size();
    if (polynomialDegree1 > polynomialDegree2) {
        result.coefficients.resize(polynomialDegree1);
        for (int16_t i = 0; i < result.coefficients.size(); i++) {
            if (i >= polynomialDegree2) {
                result.coefficients[i] = polynomial1.coefficients[i];
            }

            else result.coefficients[i] = polynomial1.coefficients[i] + polynomial2.coefficients[i];
        }
    }

    else {
        result.coefficients.resize(polynomialDegree2);
        for (int16_t i = 0; i < result.coefficients.size(); i++) {
            if (i >= polynomialDegree1) {
                result.coefficients[i] = polynomial2.coefficients[i];
            }

            else result.coefficients[i] = polynomial1.coefficients[i] + polynomial2.coefficients[i];
        }
    }

    return result;
}

Polynomial operator-(const Polynomial &polynomial1, const Polynomial &polynomial2) {
    Polynomial result = polynomial1;
    int16_t polynomialDegree1 = polynomial1.coefficients.size();
    int16_t polynomialDegree2 = polynomial2.coefficients.size();
    if (polynomialDegree1 > polynomialDegree2) {
        result.coefficients.resize(polynomialDegree1);
        for (int16_t i = 0; i < result.coefficients.size(); i++) {
            if (i >= polynomialDegree2) {
                result.coefficients[i] = polynomial1.coefficients[i];
            }

            else result.coefficients[i] = polynomial1.coefficients[i] - polynomial2.coefficients[i];
        }
    }

    else {
        result.coefficients.resize(polynomialDegree2);
        for (int16_t i = 0; i < result.coefficients.size(); i++) {
            if (i >= polynomialDegree1) {
                result.coefficients[i] = polynomial2.coefficients[i];
            }

            else result.coefficients[i] = polynomial1.coefficients[i] - polynomial2.coefficients[i];
        }
    }

    return result;
}

Polynomial operator*(const Polynomial &polynomial1, const Polynomial &polynomial2) {
    Polynomial result;
    result.coefficients.resize(polynomial1.coefficients.size() + polynomial2.coefficients.size() - 1);
    for (int16_t i = 0; i < polynomial1.coefficients.size(); i++) {
        for (int16_t j = 0; j < polynomial2.coefficients.size(); j++) {
            result.coefficients[i + j] += polynomial1.coefficients[i] * polynomial2.coefficients[j];
        }
    }

    return result;
}

Polynomial operator/(const Polynomial &polynomialInput, double &valueInput) {
    Polynomial result = polynomialInput;
    if (valueInput == 0) {
        std::cout << "Error :: invalid divisor." << std::endl;
        exit(0);
    }

    else {
        for (int16_t i = 0; i < result.coefficients.size(); i++) {
            result.coefficients[i] = polynomialInput.coefficients[i] / valueInput;
        }

        return result;
    }
}

std::ostream &operator<<(std::ostream &stream, const Polynomial &polynomialOutput) {
    for (int16_t i = polynomialOutput.coefficients.size() - 1; i >= 0; i--) {
        if (i == 0 && polynomialOutput.coefficients[i] > 0) stream << " + " << polynomialOutput.coefficients[i];
        if (i == 0 && polynomialOutput.coefficients[i] < 0) stream << " - " << std::abs(polynomialOutput.coefficients[i]);
        if (i != 0) {
            if (i == polynomialOutput.coefficients.size() - 1 && polynomialOutput.coefficients[i] != 0) {
                stream << polynomialOutput.coefficients[i] << "x^" << i;
            }

            else if (polynomialOutput.coefficients[i] > 0) {
                stream << " + " << polynomialOutput.coefficients[i] << "x^" << i;
            }

            else if (polynomialOutput.coefficients[i] < 0) {
                stream << " - " << std::abs(polynomialOutput.coefficients[i]) << "x^" << i;
            }
        }
    }

    return stream;
}

std::istream &operator>>(std::istream &stream, Polynomial &polynomialInput) {
    int16_t polynomialDegree; stream >> polynomialDegree;
    polynomialDegree = polynomialDegree + 1;
    polynomialInput.coefficients.resize(polynomialDegree);
    for (int16_t i = 0; i < polynomialDegree; i++) {
        double coefficientInput; stream >> coefficientInput;
        polynomialInput.coefficients[i] = coefficientInput;
    }

    return stream;
}

int main() {
    Point myPoint1;
    Point myPoint2;
    Point myPoint3;
    Point myPoint4;
    PolygonalChain myPolygonalChain;
    ClosedPolygonalChain myClosedPolygonalChain;
    Polygon myPolygon;
    RegularPolygon myRegularPolygon;
    Triangle myTriangle;
    Trapezoid myTrapezoid;
    Polynomial myPolynomial;
    Polynomial myPolynomial2;
    Polynomial myPolynomial3;
    myPoint1.setValues(0, 0);
    myPoint2.setValues(0, 0);
    myPoint3.setValues(0, 0);
    myPoint4.setValues(0, 0);
    std::vector<Point> myVector = {myPoint1, myPoint2, myPoint3, myPoint4};
    std::vector<double> polynomial1 = {1, 1, 1};
    std::vector<double> polynomial2 = {2, 2, 2};
    return 0;
}
