/*
 * Rect.h
 *
 *  Created on: Mar 28, 2014
 *      Author: Gabriel Naves
 */

#ifndef RECT_H_
#define RECT_H_

#include "Point.h"

class Rect
{
  public:
	Rect();
	Rect(Point point, float w, float h);
	Rect(float x, float y, float w, float h);
	void SetRect(float x, float y, float w, float h);
	void SetRect(Point point, float w, float h);
	void SetPoint(Point point);
	void SetPoint(float x, float y);

	Point GetCenter() const;
	Point GetPoint() const;
	float GetX() const;
	float GetY() const;
	float GetW() const;
	float GetH() const;

	bool IsInside(float x, float y);
	bool IsInside(Point point);

	void MoveRect(float x, float y);
	void MoveRectWithAngle(float distance, float angle);

  private:
	Point p;
	float w, h;
};

#endif /* RECT_H_ */
