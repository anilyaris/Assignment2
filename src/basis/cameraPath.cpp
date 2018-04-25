#include "cameraPath.h"
#include <iostream>

using namespace FW;



Mat4f FW::cameraPath::GetOrientation(float t)
{
	if (orientationMode)
	{
		// fetch the original control points and do some trickery to come up with new ones that make bezier interpolation continuous
		int count = orientationPoints.size();
		int start =  t * count;

		Mat4f controlPoints;
		auto clampIdx = [&](int index)->int { if (index < 0)index += count; if (index >= count)index -= count; return index; };

		controlPoints.setCol(0, orientationPoints[start].normalized());
		controlPoints.setCol(1, (orientationPoints[start]+ .4f * (orientationPoints[clampIdx(start+1)]- orientationPoints[clampIdx(start-1)])).normalized());
		controlPoints.setCol(2, (orientationPoints[clampIdx(start+1)] - .4f * (orientationPoints[clampIdx(start + 2)] - orientationPoints[start])).normalized());
		controlPoints.setCol(3, orientationPoints[clampIdx(start+1)].normalized());

		// YOUR CODE HERE (extra)
		// Use De Casteljau with spherical interpolation (slerp) to interpolate between the orientation control point
		// quaternions in the controlPoints matrix, and convert the interpolated quaternion to an orientation matrix
		return Mat4f();
	}
	else
	{
		Mat4f orientation;
		int i = t * positionPath.size();

		orientation.setCol(0, -Vec4f(positionPath[i].B, 0));
		orientation.setCol(1, -Vec4f(positionPath[i].N, 0));
		orientation.setCol(2, -Vec4f(positionPath[i].T, .0f));
		return orientation.transposed();
	}
}

Mat4f FW::cameraPath::GetTranslation(float t)
{
	int i = t * positionPath.size();

	Vec3f pos = positionPath[i].V;
	return Mat4f::translate(-pos);
}

Mat4f FW::cameraPath::GetWorldToCam(float t)
{
	return GetOrientation(t) * GetTranslation(t);
}

void FW::cameraPath::Draw(float t, GLContext* gl, Mat4f projection)
{
	mesh->draw(gl, GetWorldToCam(t), projection);
	glUseProgram(0);
}
