settings.outformat="pdf";
import graph;

unitsize(2mm);

file in=input("rap_placement2.txt"); 

int segmentIt=0;

path p;

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

    p=p -- cycle;
    fill(p, colors[5-segmentIt]); 
    p=nullpath;

    segmentIt = (segmentIt+1)%6;
    continue;
  }
  if(eof(in)) break;

  real[] pt=in.line();
  p=p -- (pt[0],pt[1]);
}
