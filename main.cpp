#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>

class Curve3D {
public:
    virtual double getRadius() const = 0;
    virtual void evaluate(double t, double& x, double& y, double& z) const = 0;
    virtual void derivative(double t, double& dx, double& dy, double& dz) const = 0;
};

class Circle : public Curve3D {
private:
    double radius;

public:
    Circle(double r) : radius(r) {
        if (radius <= 0) {
            std::cerr << "Error: Circle radius must be positive." << std::endl;
            exit(1);
        }
    }

    double getRadius() const override {
        return radius;
    }

    void evaluate(double t, double& x, double& y, double& z) const override {
        x = radius * cos(t);
        y = radius * sin(t);
        z = 0.0;
    }

    void derivative(double t, double& dx, double& dy, double& dz) const override {
        dx = -radius * sin(t);
        dy = radius * cos(t);
        dz = 0.0;
    }
};

class Ellipse : public Curve3D {
private:
    double a, b;

public:
    Ellipse(double majorRadius, double minorRadius) : a(majorRadius), b(minorRadius) {
        if (a <= 0 || b <= 0) {
            std::cerr << "Error: Ellipse radii must be positive." << std::endl;
            exit(1);
        }
    }

    double getRadius() const override {
        return a;  
    }

    void evaluate(double t, double& x, double& y, double& z) const override {
        x = a * cos(t);
        y = b * sin(t);
        z = 0.0;
    }

    void derivative(double t, double& dx, double& dy, double& dz) const override {
        dx = -a * sin(t);
        dy = b * cos(t);
        dz = 0.0;
    }
};

class Helix : public Curve3D {
private:
    double radius, step;

public:
    Helix(double r, double s) : radius(r), step(s) {
        if (radius <= 0 || step <= 0) {
            std::cerr << "Error: Helix radius and step must be positive." << std::endl;
            exit(1);
        }
    }

    double getRadius() const override {
        return radius;
    }

    double getStep() const {
        return step;  
    }

    void evaluate(double t, double& x, double& y, double& z) const override {
        x = radius * cos(t);
        y = radius * sin(t);
        z = step * t / (2 * M_PI);
    }

    void derivative(double t, double& dx, double& dy, double& dz) const override {
        dx = -radius * sin(t);
        dy = radius * cos(t);
        dz = step / (2 * M_PI);
    }
};


int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::vector<Curve3D*> curves;
    for (int i = 0; i < 5; ++i) {
        double randomRadius = static_cast<double>(std::rand()) / RAND_MAX * 10.0 + 1.0; 
        double randomStep = static_cast<double>(std::rand()) / RAND_MAX * 5.0 + 1.0;     
        int curveType = std::rand() % 3; 

        Curve3D* curve = nullptr;
        switch (curveType) {
            case 0:
                curve = new Circle(randomRadius);
                break;
            case 1:
                curve = new Ellipse(randomRadius, randomRadius * 0.5); 
                break;
            case 2:
                curve = new Helix(randomRadius, randomStep);
                break;
        }
        curves.push_back(curve);
    }

    double t = M_PI / 4;
    std::cout << "Coordinates and Derivatives at t=PI/4:" << std::endl;
    for (const Curve3D* curve : curves) {
        double x, y, z;
        double dx, dy, dz;
        curve->evaluate(t, x, y, z);
        curve->derivative(t, dx, dy, dz);
        std::cout << "Curve Type: ";
        if (dynamic_cast<const Circle*>(curve)) {
            std::cout << "Circle, Radius: " << curve->getRadius();
        } else if (dynamic_cast<const Ellipse*>(curve)) {
            std::cout << "Ellipse, Major Radius: " << dynamic_cast<const Ellipse*>(curve)->getRadius();
        } else if (dynamic_cast<const Helix*>(curve)) {
            std::cout << "Helix, Radius: " << curve->getRadius() << ", Step: " << dynamic_cast<const Helix*>(curve)->getStep();
        }
        std::cout << "\nPoint (x, y, z): (" << x << ", " << y << ", " << z << ")\n";
        std::cout << "Derivative (dx, dy, dz): (" << dx << ", " << dy << ", " << dz << ")\n\n";
    }

    std::vector<const Circle*> circleContainer;
    for (Curve3D* curve : curves) {
        if (dynamic_cast<Circle*>(curve)) {
            circleContainer.push_back(static_cast<Circle*>(curve));
        }
    }

    std::sort(circleContainer.begin(), circleContainer.end(), [](const Circle* a, const Circle* b) {
        return a->getRadius() < b->getRadius();
    });

    double totalSumOfRadii = 0.0;
    #pragma omp parallel for reduction(+:totalSumOfRadii)
    for (int i = 0; i < circleContainer.size(); ++i) {
        totalSumOfRadii += circleContainer[i]->getRadius();
    }

    std::cout << "Sorted Circles by Radius:" << std::endl;
    for (const Circle* circle : circleContainer) {
        std::cout << "Circle, Radius: " << circle->getRadius() << std::endl;
    }
    std::cout << "Total Sum of Radii: " << totalSumOfRadii << std::endl;

    for (Curve3D* curve : curves) {
        delete curve;
    }

    return 0;
}