My version of "Raytracer in a weekend"

This is a unidirectional monte-carlo path tracer.
It computes the scene using only one kind of pass.
Some raytracers, for example, have a different path for shadows, etc.

The benefit of doing it this way is that you get all the various effects as a consequence of some simple code.
The drawbacks are that
1) You can't turn off a particular effect
2) It's probably slower

