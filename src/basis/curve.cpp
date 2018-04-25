#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;
using namespace FW;

namespace {

// Approximately equal to.  We don't want to use == because of
// precision issues with floating point.
inline bool approx(const Vec3f& lhs, const Vec3f& rhs) {
	const float eps = 1e-8f;
	return (lhs - rhs).lenSqr() < eps;
}


// This is the core routine of the curve evaluation code. Unlike
// evalBezier, this is only designed to work on 4 control points.
// Furthermore, it requires you to specify an initial binormal
// Binit, which is iteratively propagated throughout the curve as
// the curvepoints are generated. Any other function that creates
// cubic splines can use this function by a corresponding change
// of basis.
Curve coreBezier(const Vec3f& p0,
				 const Vec3f& p1,
				 const Vec3f& p2,
				 const Vec3f& p3,
				 const Vec3f& Binit,
				 unsigned steps) {

	Curve R(steps + 1);

	// YOUR CODE HERE (R1): build the basis matrix and loop the given number of steps,
	// computing points on the spline

	Mat4f B;
	B.setCol(1, Vec4f(-3.0f, 3.0f, 0, 0));
	B.setCol(2, Vec4f(3.0f, -6.0f, 3.0f, 0));
	B.setCol(3, Vec4f(-1.0f, 3.0f, -3.0f, 1.0f));

	Mat4f dB;
	for (int k = 0; k < 4; k++) dB.setCol(k, B.getCol((k + 1) % 4) * ((k + 1) % 4));

	Mat4f G;
	G.setCol(0, Vec4f(p0, 0));
	G.setCol(1, Vec4f(p1, 0));
	G.setCol(2, Vec4f(p2, 0));
	G.setCol(3, Vec4f(p3, 0));

	for (unsigned i = 0; i <= steps; ++i) {
		float t = (float)i / steps;		
		Vec4f T(1.0f, t, t * t, t * t * t);

		Vec3f pos = (G * B * T).getXYZ();
		R[i].V = pos;

		Vec3f tan = (G * dB * T).getXYZ().normalized();
		R[i].T = tan;

		Vec3f bin = Binit;
		if (i == 0) {
			if (bin.cross(tan).isZero()) {
				float tmp = bin.y;
				bin.y = bin.x;
				bin.x = bin.z;
				bin.z = tmp;
			}
		}
		else bin = R[i - 1].B;

		Vec3f nor = bin.cross(tan).normalized();
		R[i].N = nor;

		R[i].B = tan.cross(nor).normalized();
	}

	return R;
}    

} // namespace

Curve coreBezier(const Vec3f& p0,
	const Vec3f& p1,
	const Vec3f& p2,
	const Vec3f& p3,
	const Vec3f& Binit,
	const float begin, const float end, const float errorbound, const float minstep) {

	// YOUR CODE HERE(EXTRA): Adaptive tessellation

	return Curve();
}
    
// the P argument holds the control points and steps gives the amount of uniform tessellation.
// the rest of the arguments are for the adaptive tessellation extra.
Curve evalBezier(const vector<Vec3f>& P, unsigned steps, bool adaptive, float errorbound, float minstep) {
    // Check
    if (P.size() < 4 || P.size() % 3 != 1) {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
		_CrtDbgBreak();
		exit(0);
	}

    // YOUR CODE HERE (R1):
    // You should implement this function so that it returns a Curve
    // (e.g., a vector<CurvePoint>).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

	int num_pieces = P.size() / 3;
	Curve curve(num_pieces * (steps + 1));

	Mat4f B;	
	B.setCol(1, Vec4f(-3.0f, 3.0f, 0, 0));
	B.setCol(2, Vec4f(3.0f, -6.0f, 3.0f, 0));
	B.setCol(3, Vec4f(-1.0f, 3.0f, -3.0f, 1.0f));

	Mat4f dB;
	for (int k = 0; k < 4; k++) dB.setCol(k, B.getCol((k + 1) % 4) * ((k + 1) % 4));


	for (unsigned int cur_piece = 0; cur_piece < num_pieces; cur_piece++) {
		for (unsigned int step = 0; step <= steps; step++) {
			float t = (float)step / steps;
			int cur_index = (steps + 1) * cur_piece + step;
			int p1_index = 3 * cur_piece;

			Mat4f G;			
			for (int i = 0; i < 4; i++)	G.setCol(i, Vec4f(P[p1_index + i], 0));
			Vec4f T(1.0f, t, t * t, t * t * t);
			
			Vec3f pos = (G * B * T).getXYZ();
			curve[cur_index].V = pos;

			Vec3f tan = (G * dB * T).getXYZ().normalized();
			curve[cur_index].T = tan;

			Vec3f bin(0, 0, 1.0f);
			if (cur_piece == 0 && step == 0) {
				if (bin.cross(tan).isZero()) {
					bin.y = 1.0f;
					bin.z = 0;
				}
			}
			else bin = curve[cur_index - 1].B;

			Vec3f nor = bin.cross(tan).normalized();
			curve[cur_index].N = nor;

			curve[cur_index].B = tan.cross(nor).normalized();
		}
	}

	// EXTRA CREDIT NOTE:
    // Also compute the other Vec3fs for each CurvePoint: T, N, B.
    // A matrix [N, B, T] should be unit and orthogonal.
    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity. The T, N and B vectors will not
	// have to be defined at points where this does not hold.

    cerr << "\t>>> evalBezier has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector<Vec3f>): "<< endl;
    for (unsigned i = 0; i < P.size(); ++i) {
        cerr << "\t>>> "; printTranspose(P[i]); cerr << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    
    return curve;
}

// the P argument holds the control points and steps gives the amount of uniform tessellation.
// the rest of the arguments are for the adaptive tessellation extra.
Curve evalBspline(const vector<Vec3f>& P, unsigned steps, bool adaptive, float errorbound, float minstep) {
    // Check
    if (P.size() < 4) {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit(0);
    }

    // YOUR CODE HERE (R2):
    // We suggest you implement this function via a change of basis from
	// B-spline to Bezier.  That way, you can just call your evalBezier function.
	// Didn't do that because of increasing error in float multiplication

	int num_pieces = P.size() - 3;
	bool closed = P[0] == P[P.size() - 3] && P[1] == P[P.size() - 2] && P[2] == P[P.size() - 1];
	Curve curve(num_pieces * steps + closed);

	Mat4f B;
	B.setCol(0, (1.0f / 6.0f) * Vec4f(1.0f, 4.0f, 1.0f, 0));
	B.setCol(1, (1.0f / 6.0f) * Vec4f(-3.0f, 0, 3.0f, 0));
	B.setCol(2, (1.0f / 6.0f) * Vec4f(3.0f, -6.0f, 3.0f, 0));
	B.setCol(3, (1.0f / 6.0f) * Vec4f(-1.0f, 3.0f, -3.0f, 1.0f));

	Mat4f dB;
	for (int k = 0; k < 4; k++) dB.setCol(k, B.getCol((k + 1) % 4) * ((k + 1) % 4));

	for (unsigned int cur_piece = 0; cur_piece < num_pieces; cur_piece++) {
		Mat4f G;
		for (int i = 0; i < 4; i++)	G.setCol(i, Vec4f(P[cur_piece + i], 0));

		for (unsigned int step = 0; step < steps; step++) {
			float t = (float)step / steps;
			int cur_index = steps * cur_piece + step;
			
			Vec4f T(1.0f, t, t * t, t * t * t);

			Vec3f pos = (G * B * T).getXYZ();
			curve[cur_index].V = pos;

			Vec3f tan = (G * dB * T).getXYZ().normalized();
			curve[cur_index].T = tan;

			Vec3f bin(0, 0, 1.0f);
			if (cur_piece == 0 && step == 0) {
				if (bin.cross(tan).isZero()) {
					bin.y = 1.0f;
					bin.z = 0;
				}
			}
			else bin = curve[cur_index - 1].B;

			Vec3f nor = bin.cross(tan).normalized();
			curve[cur_index].N = nor;

			curve[cur_index].B = tan.cross(nor).normalized();
		}
	}
	if (closed) {
		curve.back().V = curve.front().V;
		curve.back().T = curve.front().T;
		curve.back().N = curve.front().N;
		curve.back().B = curve.front().B;
	}

    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vec3f >): "<< endl;
    for (unsigned i = 0; i < P.size(); ++i) {
        cerr << "\t>>> "; printTranspose(P[i]); cerr << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    
    return curve;
}

Curve evalCircle(float radius, unsigned steps) {
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector<CurvePoint>).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R(steps+1);

    // Fill it in counterclockwise
    for (unsigned i = 0; i <= steps; ++i) {
        // step from 0 to 2pi
        float t = 2.0f * (float)M_PI * float(i) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vec3f(FW::cos(t), FW::sin(t), 0);
        
        // Tangent vector is first derivative
        R[i].T = Vec3f(-FW::sin(t), FW::cos(t), 0);
        
        // Normal vector is second derivative
        R[i].N = Vec3f(-FW::cos(t), -FW::sin(t), 0);

        // Finally, binormal is facing up.
        R[i].B = Vec3f(0, 0, 1);
    }

    return R;
}

void drawCurve(const Curve& curve, float framesize) {
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // Setup for line drawing
    glDisable(GL_LIGHTING); 
    glColor4f(1, 1, 1, 1);
    glLineWidth(1);
    
	if (framesize >= 0) {
		// Draw curve
		glBegin(GL_LINE_STRIP);
		for (unsigned i = 0; i < curve.size(); ++i) {
			glVertex(curve[i].V);
		}
		glEnd();
	}

    glLineWidth(1);

    // Draw coordinate frames if framesize nonzero
    if (framesize != 0.0f) {
		framesize = FW::abs(framesize);
        Mat4f M;

        for (unsigned i = 0; i < curve.size(); ++i) {
            M.setCol( 0, Vec4f( curve[i].N, 0 ) );
            M.setCol( 1, Vec4f( curve[i].B, 0 ) );
            M.setCol( 2, Vec4f( curve[i].T, 0 ) );
            M.setCol( 3, Vec4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf(M.getPtr());
            glScaled(framesize, framesize, framesize);
            glBegin(GL_LINES);
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

