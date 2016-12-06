settings.outformat="pdf";
import graph;

//size(400,350,IgnoreAspect);
unitsize(1mm);


file in=input("streamline_placement.txt"); //.line();
int noLines = in.line();

int line=0;
real[][][] streamlines = new real[noLines][2][]; 
// dimensions: [no Of streamlines] [2 (x and y)] [no of points]


while(line<noLines) {

  while(true) {
    real[] pt=in.line();

    if(eol(in)) 
    {
      streamlines[line][0].push(pt[0]);
      streamlines[line][1].push(pt[1]);
      pt = in.line();
      break;
    }

    streamlines[line][0].push(pt[0]);
    streamlines[line][1].push(pt[1]);

  }
  //draw(graph(streamlines[line][0],streamlines[line][1]),rgb(255-line,line,line));
  draw(reverse(graph(streamlines[line][0],streamlines[line][1])),black, arrow = Arrow);

  line=line+1;
  if(eof(in)) break;
}
