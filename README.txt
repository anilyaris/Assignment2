# ME-C3100 Computer Graphics, Fall 2017
# Lehtinen / Kemppinen, Ollikainen, Aarnio
#
# Assignment 2: Curves and Surfaces

Student name: Anıl YARIŞ
Student number: 643454
Hours spent on requirements (approx.): 7-8 hours
Hours spent on extra credit (approx.): 10-11 hours

# First, some questions about how you worked on this assignment.
# Your answers in this section will be used to improve the course.
# They will not be judged or affect your points, but please answer them.

- Did you go to lectures?
Yes.
- Did you go to exercise sessions?
No.
- Did you work on the assignment using Aalto computers, your own computers, or both?
I used my own computer.
# Which parts of the assignment did you complete? Mark them 'done'.
# You can also mark non-completed parts as 'attempted' if you spent a fair amount of
# effort on them. If you do, explain the work you did in the problems/bugs section
# and leave your 'attempt' code in place (commented out if necessary) so we can see it.

                        R1 Evaluating Bezier curves (2p): done
                      R2 Evaluating B-spline curves (2p): done
       R3 Subdividing a mesh into smaller triangles (2p): done
        R4 Computing positions for the new vertices (2p): done
R5 Smoothing the mesh by repositioning old vertices (2p): done

# Did you do any extra credit work?

Proper handling of boundaries for Loop subdivision
Local coordinate frames on the curve
Surfaces of revolution
Generalized cylinders

# Are there any known problems/bugs remaining in your code?

Proper handling of boundaries for Loop subdivision: New positions of some boundary points are not calculated correctly. I believe I implemented what is given on the tutorial link provided in the PDF 
but I couldn't find the issue. I could've worked on it a little bit more but after some time I skipped it because it was consuming my time and I could do other extras in that time.

Local coordinate frames on the curve: I noticed orientations of the axes begin to differ slightly from the example executable's output as the calculation travels along the curve. Again, I don't know what I did wrong and 
I didn't want to spend too much time to debug this. Due to this issue the outputs of the generalized cylinder task are slightly different in shading and overall shape compared to the expected outputs (most extreme difference
occurs in the weirder.swp file, others are very subtle). Also the camera orientation works but not the same way as how it works on the example executable, I believe that's also because of this issue.

# Did you collaborate with anyone in the class?

I answered a friend's question about what the argument Binit means in the coreBezier function.

# Any other comments you'd like to share about the assignment or the course so far?
