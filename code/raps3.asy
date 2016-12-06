settings.outformat = "png";
settings.prc = false;
settings.render = 10;

import solids;
import three;
import graph3;

size(15cm);

file in=input("rap_placement3.txt"); 

//currentprojection=orthographic(-1,-1,1);
currentprojection=orthographic(0,0,1);

int segmentIt=0;

path3 p;
path3 p2;

//Blue/Red with 25 percent luminosity in LAB
pen blue = rgb(0,0.294,1);
pen red = rgb(0.792,0,0.263);
//Yellow/Green with 75 percent luminosity in LAB
pen yellow = rgb(0.843,0.714,0);
pen green = rgb(0,0.906,0.710);

//Illusion direction: -----> ---> --->
pen colors[] = {black, blue, blue, white, yellow, yellow};
//pen colors[] = {black, red, red, white, green, green};

while(true) {
  if(eol(in)) 
  {
    real[] tmp = in.line();

    segmentIt = (segmentIt+1)%6;
    continue;
  }
  if(eof(in)) break;

  real[] pt1=in.line();
  real[] pt2=in.line();
  real[] pt3=in.line();
  real[] pt4=in.line();

  p=(pt1[0],pt1[1],pt1[2]) 
    -- (pt2[0],pt2[1],pt2[2]) 
    -- (pt3[0],pt3[1],pt3[2]) 
    -- (pt4[0],pt4[1],pt4[2]) -- cycle;

  pt1=in.line();
  pt2=in.line();
  pt3=in.line();
  pt4=in.line();

  p2=(pt1[0],pt1[1],pt1[2]) 
    -- (pt2[0],pt2[1],pt2[2]) 
    -- (pt3[0],pt3[1],pt3[2]) 
    -- (pt4[0],pt4[1],pt4[2]) -- cycle;

  draw( surface(p), colors[5-segmentIt] , nolight ); 
  draw(surface(p2), colors[5-segmentIt] , nolight ); 
}
