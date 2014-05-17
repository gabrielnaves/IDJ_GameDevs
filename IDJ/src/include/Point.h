/*
 * Point.h
 *
 *  Created on: Mar 28, 2014
 *      Author: Gabriel Naves
 */

#ifndef POINT_H_
#define POINT_H_

#include <cmath>

class Point
{
  public:
	Point();
	Point(float x, float y);

	void SetPoint(float x, float y);
	void SetPoint(const Point& point);

	float GetX() const;
	float GetY() const;

	void MovePointWithAngle(float distance, float angle);
	void MovePoint(float x, float y);
	void Multiply(float c);

	float FindAngle(const Point& point);
	bool Equals(const Point& point);
	float Distance(const Point& point);

	Point operator+(const Point& rhs) const;
    Point operator-(const Point& rhs) const;
    Point operator*(const float rhs) const;

  private:
	float x, y;
};

#endif /* POINT_H_ */