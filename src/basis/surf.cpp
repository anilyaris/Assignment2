#include "surf.h"
#include "extra.h"

using namespace std;
using namespace FW;

namespace
{
    // This is a generic function that generates a set of triangle
    // faces for a sweeping a profile curve along "something".  For
    // instance, say you want to sweep the profile curve [01234]:
    //
    //   4     9     10
    //    3     8     11
    //    2 --> 7 --> 12 ----------[in this direction]--------->
    //    1     6     13 
    //   0     5     14
    //
    // Then the "diameter" is 5, and the "length" is how many times
    // the profile is repeated in the sweep direction.  This function
    // generates faces in terms of vertex indices.  It is assumed that
    // the indices go as shown in the picture (the first dia vertices
    // correspond to the first repetition of the profile curve, and so
    // on).  It will generate faces [0 5 1], [1 5 6], [1 6 2], ...
    // The boolean variable "closed" will determine whether the
    // function closes the curve (that is, connects the last profile
    // to the first profile).
    static vector< FW::Vec3i > triSweep( unsigned dia, unsigned len, bool closed )
    {
        vector< FW::Vec3i > ret;

		// YOUR CODE HERE: generate zigzagging triangle indices and push them to ret.
		int num_vertices = dia * len;
		for (int cur_curve = 0; cur_curve < len - 1 + (int)closed; cur_curve++) {
			for (int cur_vertex = 0; cur_vertex < dia - 1; cur_vertex++) {
				int cur_index = dia * cur_curve + cur_vertex;
				ret.push_back(Vec3i(cur_index, (cur_index + 1) % num_vertices, (cur_index + dia) % num_vertices));
				ret.push_back(Vec3i((cur_index + 1) % num_vertices, (cur_index + dia + 1) % num_vertices, (cur_index + dia) % num_vertices));
			}
		}

        return ret;
    }
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // YOUR CODE HERE: build the surface.  See surf.h for type details.
	// Generate the vertices and normals by looping the number of the steps and again for each 
	// point in the profile (that's two cascaded loops), and finally get the faces with triSweep.
	// You'll need to rotate the curve at each step, similar to the cone in assignment 0 but
	// now you should be using a real rotation matrix.

	int dia = profile.size();

	for (int step = 0; step < steps; step++) {
		float angle = (float)step * 2.0f * (float)M_PI / steps;
		Mat3f rot = Mat3f::rotation(Vec3f(0, 1, 0), angle);

		for (int cur_point = 0; cur_point < dia; cur_point++) {
			surface.VV.push_back(rot * profile[cur_point].V);
			surface.VN.push_back(-1.0f * rot * profile[cur_point].N);
		}		
	}

	surface.VF = triSweep(dia, steps, true);
    return surface;
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // YOUR CODE HERE: build the surface. 
	// This is again two cascaded loops. Build the local coordinate systems and transform
	// the points in a very similar way to the one with makeSurfRev.
	
	const float eps = 1e-8f;
	Vec3f sDif = sweep.front().V - sweep.back().V;
	
	bool sClosed = sDif.lenSqr() < eps;
	int dia = profile.size();
	int steps = sweep.size() - sClosed;

	for (int step = 0; step < steps; step++) {
		Vec3f N = sweep[step].N;
		Vec3f B = sweep[step].B;
		Vec3f T = sweep[step].T;
		Vec3f V = sweep[step].V;

		Mat4f M;
		M.setCol(0, Vec4f(N, 0));
		M.setCol(1, Vec4f(B, 0));
		M.setCol(2, Vec4f(T, 0));
		M.setCol(3, Vec4f(V, 1));

		Mat3f invT;
		invT.setCol(0, N);
		invT.setCol(1, B);
		invT.setCol(2, T);
		invT.invert();
		invT.transpose();
		
		for (int cur_point = 0; cur_point < dia; cur_point++) {
			surface.VV.push_back((M * profile[cur_point].V.toHomogeneous()).toCartesian());
			surface.VN.push_back((-1.0f * invT * profile[cur_point].N).normalized());
		}
	}
	
	surface.VF = triSweep(dia, steps, sClosed);
    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
