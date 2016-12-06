#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Stream_lines_2.h>
#include <CGAL/Runge_kutta_integrator_2.h>
#include <CGAL/Regular_grid_2.h>

#include <iostream>
#include <tuple>
#include <fstream>

#include <cstdlib>
#include <ctime>

#define JITTER (1. + double(10 - rand()%20)/80)

typedef CGAL::Exact_predicates_inexact_constructions_kernel         K;
typedef CGAL::Regular_grid_2<K> Field;
typedef CGAL::Runge_kutta_integrator_2<Field> Runge_kutta_integrator;
typedef CGAL::Stream_lines_2<Field, Runge_kutta_integrator> Strl;
typedef Strl::Point_iterator_2 Point_iterator;
typedef Strl::Stream_line_iterator_2 Strl_iterator;
typedef Strl::Point_2 Point_2;
typedef Strl::Vector_2 Vector_2;

int main(int argc, char ** argv)
{
  //Streamline seperation distance
  double dSep;
  //RAP segment length (Constraint: must be larger than integration step length, see below..)
  double segLen;
  //RAP segment width
  double segWidth;

  if (argc == 4 || argc == 5)
  {
    dSep = atof(argv[1]);
    segLen = atof(argv[2]);
    segWidth = atof(argv[3]);
  }
  else if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " \t <Streamline-Distance> \n\t\t <RAP-Length> \n\t\t <RAP-width> \n\t\t<vector-file>\n\t" 
      << "or only " << argv[0] << " <vector-file>\nUsing default values" << std::endl;
  }

  std::string input = "vector_fields/random.txt";
  if (argc==5)
    input = argv[4];
  else if (argc==2)
    input = argv[1];


  /* ASCII file containing the vector field values, 
     first four values are 
     (int) noSamples_x noSamples_y
     (double) domainLength_x domainLength_y
     Then pairs (double) x-y vector-components follow, (x-)Row-Major
     */
  std::ifstream infile(input, std::ios::in);
  if (!infile.is_open())
  {
    std::cerr << "Failed to open input-file!" << std::endl;
    return -1;
  }

  double iXSize, iYSize;
  unsigned int x_samples, y_samples;
  infile >> x_samples;
  infile >> y_samples;
  infile >> iXSize;
  infile >> iYSize;

  //Streamline "saturation ratio" (see Mebarki) usually doesn't change
  const double dRat = 1.6;


  if (argc == 1 || argc == 2){

    //Streamline seperation distance
    //Must be larger than integration step length (see below)
    dSep = double(iXSize+iYSize)/(2.*70.); //those values are "good choices", no theoretical background. 
   
    //RAP segment length 
    segLen = 2.8*dSep;
    //RAP segment width
    segWidth = 1.7*dSep*dRat;
  }

  std::cout << "Parameters: \t dSep: " << dSep << "\n\t\t segLen: " << segLen << "\n\t\t segWidth: " << segWidth << std::endl;

  srand(time(NULL)); //for jittering the segment length

  Field regular_grid_2(x_samples, y_samples, iXSize, iYSize);
  /*fill the grid with the appropreate values*/
  for (unsigned int j=0;j<y_samples;j++)
    for (unsigned int i=0;i<x_samples;i++)
    {
      double xval, yval;
      infile >> xval;
      infile >> yval;
      regular_grid_2.set_field(i, j, Vector_2(xval, yval));
    }
  infile.close();

  Runge_kutta_integrator runge_kutta_integrator/*(30.0)*/; //integration step length, should be >= 1.

  /* the placement of streamlines */
  std::cout << "processing...\n";

  Strl Stream_lines(regular_grid_2, runge_kutta_integrator,dSep,dRat);
  std::cout << "placement generated\n";

  std::ofstream fw2("rap_placement2.txt",std::ios::out);
  std::ofstream fw3("rap_placement3.txt",std::ios::out);
  std::ofstream stlw("streamline_placement.txt",std::ios::out);
  stlw << Stream_lines.number_of_lines() << "\n";

  for(Strl_iterator sit = Stream_lines.begin(); sit != Stream_lines.end(); sit++)
  {
    Point_iterator pit = sit->first;
    Point_2 p_prev = *pit;

    //Assume streamline has more than two points..
    pit++;

    double remSegLen = JITTER*segLen/6.;
    std::list<std::tuple<Point_2,Point_2,Point_2> > segment;
    Vector_2 orth_vec;

    stlw << p_prev.x() << " " << p_prev.y() << "\n";
    for(; pit != sit->second; pit++){

      Point_2 p = *pit;
      stlw << p.x() << " " << p.y() << "\n";

      //build vector connecting previous and current point
      Vector_2 vec(p, p_prev);
      //normalize it
      double len = sqrt(vec.squared_length());
      vec = vec/len;
      orth_vec=vec.perpendicular(CGAL::COUNTERCLOCKWISE);

      Point_2 p1 = p_prev + segWidth/2.*orth_vec;
      Point_2 p2 = p_prev - segWidth/2.*orth_vec;

      std::tuple<Point_2,Point_2,Point_2> ptsTuple(p1,p_prev,p2);
      segment.push_back(ptsTuple);


      if (remSegLen > len)
      {
        remSegLen -= len;
      }
      else
      {
        segment.pop_back();

        Point_2 p_x = p_prev + remSegLen * vec;
        Point_2 p3 = p_x + segWidth/2 * orth_vec;
        Point_2 p4 = p_x - segWidth/2 * orth_vec;

        std::tuple<Point_2,Point_2,Point_2> endPts(p3,p_x,p4);
        segment.push_back(endPts);

        //output a cyclic path of points, enclosing the polygon to be colored
        std::list<std::tuple<Point_2,Point_2,Point_2> >::iterator it = segment.begin();
        while (it!= segment.end())
        {
          fw2 << std::get<0>(*it).x() << " " << std::get<0>(*it).y() << "\n";

          std::tuple<Point_2,Point_2,Point_2> & currPts = *it;
          it++;
          if (it != segment.end())
          {
            std::tuple<Point_2,Point_2,Point_2> & nextPts = *it;
            fw3 << std::get<0>(currPts).x() << " " << std::get<0>(currPts).y() << " 0\n";
            fw3 << std::get<1>(currPts).x() << " " << std::get<1>(currPts).y() << " 10\n";
            fw3 << std::get<1>(nextPts).x() << " " << std::get<1>(nextPts).y() << " 10\n";
            fw3 << std::get<0>(nextPts).x() << " " << std::get<0>(nextPts).y() << " 0\n";

            fw3 << std::get<2>(currPts).x() << " " << std::get<2>(currPts).y() << " 0\n";
            fw3 << std::get<2>(nextPts).x() << " " << std::get<2>(nextPts).y() << " 0\n";
            fw3 << std::get<1>(nextPts).x() << " " << std::get<1>(nextPts).y() << " 10\n";
            fw3 << std::get<1>(currPts).x() << " " << std::get<1>(currPts).y() << " 10\n";
          }
        }  
        it--;
        while (it != segment.begin())
        {
          fw2 << std::get<2>(*it).x() << " " << std::get<2>(*it).y() << "\n";
          it--;
        }
        fw2 << std::get<2>(*it).x() << " " << std::get<2>(*it).y() << "\n";

        fw2 << "\n";
        fw3 << "\n";

        segment.clear();


        //produce a little overlap to hack bogus pdf renderer ;)
        p_x = p_prev + (1.15*remSegLen) * vec;
        p3 = p_x + segWidth/2 * orth_vec;
        p4 = p_x - segWidth/2 * orth_vec;

        segment.push_back(std::make_tuple(p3,p_x,p4));
        remSegLen = JITTER*segLen/6.;
      }

      p_prev = p;
    }

    //output rest of current fragment
    Point_2 p1 = p_prev + segWidth/2.*orth_vec;
    Point_2 p2 = p_prev - segWidth/2.*orth_vec;

    std::tuple<Point_2,Point_2, Point_2> ptsTuple(p1,p_prev,p2);
    segment.push_back(ptsTuple);
    std::list<std::tuple<Point_2,Point_2, Point_2> >::iterator it = segment.begin();
    while (it!= segment.end())
    {
      fw2 << std::get<0>(*it).x() << " " << std::get<0>(*it).y() << "\n";

      std::tuple<Point_2,Point_2,Point_2> & currPts = *it;
      it++;
      if (it != segment.end())
      {
        std::tuple<Point_2,Point_2,Point_2> & nextPts = *it;
        fw3 << std::get<0>(currPts).x() << " " << std::get<0>(currPts).y() << " 0\n";
        fw3 << std::get<1>(currPts).x() << " " << std::get<1>(currPts).y() << " 10\n";
        fw3 << std::get<1>(nextPts).x() << " " << std::get<1>(nextPts).y() << " 10\n";
        fw3 << std::get<0>(nextPts).x() << " " << std::get<0>(nextPts).y() << " 0\n";

        fw3 << std::get<2>(currPts).x() << " " << std::get<2>(currPts).y() << " 0\n";
        fw3 << std::get<2>(nextPts).x() << " " << std::get<2>(nextPts).y() << " 0\n";
        fw3 << std::get<1>(nextPts).x() << " " << std::get<1>(nextPts).y() << " 10\n";
        fw3 << std::get<1>(currPts).x() << " " << std::get<1>(currPts).y() << " 10\n";
      }
    }  
    it--;
    while (it != segment.begin())
    {
      fw2 << std::get<2>(*it).x() << " " << std::get<2>(*it).y() << "\n";
      it--;
    }
    fw2 << std::get<2>(*it).x() << " " << std::get<2>(*it).y() << "\n";
    fw2 << "\n";
    fw3 << "\n";


    fw2 << "\n";
    stlw << "\n";
  }

  stlw.close();
  fw2.close();
  fw3.close();

  std::cout << "placement files written" << std::endl;
}
