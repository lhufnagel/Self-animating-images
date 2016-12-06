# Self-animating-images
## Illusory Motion Using Repeated Asymmetric Patterns Code

![alt Karman vortex street](hhttps://github.com/lhufnagel/Self-animating-images/example2.png "Karman vortex street")


A little project to create optical illusions of moving images from arbitrary vector fields (e.g. fluid flow).
Presented in a computer graphics seminar in summer '15.

### Requirements:
* [CGAL library](https://www.cgal.org/)
* [Asymptote](http://asymptote.sourceforge.net/)

### Usage
Compile code with:
```ccmake .
[Press c twice, possibly you need to adapt the path to CGAL. Then press g] 
make```

Then run
```./code
		 <Streamline-Distance>
		 <RAP-Length>
		 <RAP-width>
		<vector-file>```
or only `./code <vector-file>`

Example input-files are in subdirectory `vector_fields`. 
Use `asy *.asy` to render output images

![alt Arbitrary vector field](hhttps://github.com/lhufnagel/Self-animating-images/example.png "Arbitrary vector field")

