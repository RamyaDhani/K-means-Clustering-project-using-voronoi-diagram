#include <iostream>
#include <fstream>
#include <cassert>
#include <iterator>
#include <CGAL/Cartesian.h>
#include <vector>
#include <math.h>
#include <algorithm>
using namespace std;

// includes for defining the Voronoi diagram adaptor
#include <CGAL/basic.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
#include <CGAL/centroid.h>
#include <CGAL/Dimension.h>

// typedefs for defining the adaptor
typedef CGAL::Exact_predicates_inexact_constructions_kernel                  K;
typedef CGAL::Delaunay_triangulation_2<K>                                    DT;
typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT>                 AT;
typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT> AP;
typedef CGAL::Voronoi_diagram_2<DT,AT,AP>                                    VD;
typedef CGAL::Dimension_tag<0>  tag;

typedef K::Point_2 Point;
typedef K::Iso_rectangle_2 Iso_rectangle_2;
typedef K::Segment_2 Segment_2;
typedef K::Ray_2 Ray_2;
typedef K::Line_2 Line_2;

#define numcentroids 4

// typedef for the result type of the point location
typedef AT::Site_2                    Site_2;
typedef AT::Point_2                   Point_2;

typedef VD::Locate_result             Locate_result;
typedef VD::Vertex_handle             Vertex_handle;
typedef VD::Face_handle               Face_handle;
typedef VD::Halfedge_handle           Halfedge_handle;
typedef VD::Ccb_halfedge_circulator   Ccb_halfedge_circulator;
typedef VD::Halfedge_around_vertex_circulator Halfedge_around_vertex_circulator;

typedef std::vector<Point> Points;

std::list<Segment_2>:: iterator it;

std::vector<Point>::iterator vfrom,vto;
std::vector<Point>::iterator fcit;

Points vfaces[numcentroids],facepoints[numcentroids]; // FACES AND POINTS INSIDE FACES OF VORONOI DIAGRAM
Points vpoints;
Points alpoints[2];// CROPPED SEGMENTS OF VORONOI DIAGRAM
Points oldcentroids,newcentroids; // OLD AND NEW CENTROIDS


//A class to recover Voronoi diagram from stream.
//Rays, lines and segments are cropped to a rectangle
//so that only segments are stored
struct Cropped_voronoi_from_delaunay{
  std::list<Segment_2> m_cropped_vd;
  Iso_rectangle_2 m_bbox;
  
  Cropped_voronoi_from_delaunay(const Iso_rectangle_2& bbox):m_bbox(bbox){}
  
  template <class RSL>
  void crop_and_extract_segment(const RSL& rsl){
    CGAL::Object obj = CGAL::intersection(rsl,m_bbox);
    
    const Segment_2* s=CGAL::object_cast<Segment_2>(&obj);
    if (s)
    {
     
    
   /*std::cout << "Segment pushed is:" << *s << std::endl;*/
    	 m_cropped_vd.push_back(*s);
     
    }
  }
  
  void operator<<(const Ray_2& ray)    { crop_and_extract_segment(ray); }
  void operator<<(const Line_2& line)  { crop_and_extract_segment(line); }
  void operator<<(const Segment_2& seg){ crop_and_extract_segment(seg); }
};


void print_endpoint(Halfedge_handle e, bool is_src) {
  std::cout << "\t";
  if ( is_src ) {
    if ( e->has_source() )  std::cout << e->source()->point() << std::endl;
    else  std::cout << "point at infinity" << std::endl;
  } else {
    if ( e->has_target() )  std::cout << e->target()->point() << std::endl;
    else  std::cout << "point at infinity" << std::endl;
  }
}



void croppedvoronoi()
{

  DT dt2;
  std::cout<< "Inside cropped voronoi.." << std::endl; 
  alpoints[0].clear();
  alpoints[1].clear();
  vpoints.clear();
  //insert points into the triangulation
  std::vector<Point>::iterator nc;

  nc = newcentroids.begin();
  for(nc = newcentroids.begin(); nc!= newcentroids.end(); nc++)
  {
	dt2.push_back(Point((*nc)));       

  }

  DT::Edge_iterator eit =dt2.edges_begin();
  std::cout << "Segments are..: "<< std::endl;
  
  for ( ; eit !=dt2.edges_end(); ++eit) 
{
    CGAL::Object o = dt2.dual(eit);
    if (CGAL::object_cast<K::Segment_2>(&o)) 
    {
	
        const Segment_2 * segment = CGAL::object_cast<K::Segment_2>(&o);
	
	std::cout <<"[[" << (*segment).source().x() << "," << (*segment).target().x() << "],[" <<(*segment).source().y() << "," << (*segment).target().y() <<"]]" << std::endl;
    }
    else if (CGAL::object_cast<K::Ray_2>(&o)) 
    {
	const Ray_2 * ray = CGAL::object_cast<K::Ray_2>(&o);
	
	std::cout << "[[" << (*ray).source().x() << "," << (*ray).to_vector().x() << "],[" <<(*ray).source().y() << "," <<  (*ray).to_vector().y() << "]]"<< std::endl;
	
    }
  }

  std::vector<Point_2>::iterator it2;
  
  //construct a rectangle
  Iso_rectangle_2 bbox(-100,-100,100,100);
  Cropped_voronoi_from_delaunay vor(bbox);
  //extract the cropped Voronoi diagram
  dt2.draw_dual(vor);
  //print the cropped Voronoi diagram as segments
  std::list<Segment_2>:: iterator it;
  std::vector<Point>::iterator sit1;
  std::vector<Point>::iterator sit2,vitt;

  int count = 0;
  for (it =  vor.m_cropped_vd.begin(); it!= vor.m_cropped_vd.end(); it++)
  {

	
	double a = (*it).source().x();
	double b = (*it).source().y();
        double c = (*it).target().x();
	double d = (*it).target().y();
      
	
	double ai = round(a);
	double bi = round(b);
	double ci = round(c);
        double di = round(d);
	
	//std::cout << "Cropped segment:" << std::endl;
        //std::cout << a <<" " << b << " " << c << " " << d << std::endl;

	double diff1 = abs(a-c);
	double diff2 = abs(b-d);
     
	
        if(!((ai == ci) && (bi == di)))
	{
		
    		

                int ff1 = 0,ff2 = 0,ff3 = 0;
                
		for(sit1 = alpoints[0].begin(),sit2 = alpoints[1].begin(); sit1 != alpoints[0].end(),sit2 != alpoints[1].end(); sit1++,sit2++)
		{
			

			if(((*sit1).x() == ai) && ((*sit1).y() == bi) && ((*sit2).x() == ci) && ((*sit2).y() == di))
			{
				ff1 = 1;
				//std::cout << "Found: " << (*sit1) << (*sit2) << std::endl;
				
			}			


		}
  
		for(vitt = vpoints.begin(); vitt != vpoints.end(); vitt++)
		{
			if(((*vitt).x() == ai) && ((*vitt).y() == bi)) 
			{
				ff2 = 1;
			}
			if(((*vitt).x() == ci) && ((*vitt).y() == di))
			{

				ff3 = 1;
			}
		}
		
		if(ff1 == 0)
		{
			count = count + 1;
			//std::cout << "Cropped Segment: " << ai << " " << bi << " " << ci << " " << di << std::endl;
        		alpoints[0].push_back(Point(ai,bi));
			alpoints[1].push_back(Point(ci,di));
		}

                if(ff2 == 0)
		{
			vpoints.push_back(Point(ai,bi));
			//std::cout << "Not Found Point: " << ai << " " << bi << std::endl;

		}
		if(ff3 == 0)
		{

			vpoints.push_back(Point(ai,bi));
			//std::cout << "Not Found Point: " << ci << " " << di << std::endl;


		}
	               
	}	
		

  }
  

}



void computenewcentroids(int readflag)
{
  Site_2 t;
  
  
  VD vd;
  DT dt2;
  AT at1;
  std::vector<Point>::iterator oc,nc;
  
  for(int j = 0; j <numcentroids ; j++)
  {
	vfaces[j].clear();
	facepoints[j].clear();

  }
  if(readflag == 0)
  {
  	std::ifstream ifs("voronoi.cin");
  	assert( ifs );

  	Point vpo;
  	while ( ifs >> vpo ) {
  	oldcentroids.push_back(vpo);
  	}
  	ifs.close();
  }

  else
  {
	oldcentroids.clear();
	nc = newcentroids.begin();
  	for(nc = newcentroids.begin(); nc!= newcentroids.end(); nc++)
  	{
		oldcentroids.push_back(Point((*nc)));       

  	}


  }


  

  
  tag tg;
  
  oc = oldcentroids.begin();
  for(oc = oldcentroids.begin(); oc!= oldcentroids.end(); oc++)
  {
	t = Site_2(Point_2(*oc));
	vd.insert(t);       

  }

  
  assert( vd.is_valid() );
  

  VD::Vertex_iterator vt;
  //std::cout << "Voronoi vertices are: " << std::endl;

  for (vt = vd.vertices_begin(); vt != vd.vertices_end(); vt++)
  {
      Halfedge_around_vertex_circulator ec_start = vd.incident_halfedges(*vt);
      Halfedge_around_vertex_circulator ec = ec_start;
      
      //print_endpoint(ec, false);
      
    
      //std::cout << "" << std::endl;

  }

  //std::cout << "Number of faces: " << vd.number_of_faces() << std::endl;
  int nf = vd.number_of_faces();
  
  
  

  VD::Face_iterator fc;
  int fcc = 0;
  for (fc = vd.faces_begin(); fc != vd.faces_end(); fc++)
  {
     // std::cout << "Face is:" << std::endl;
      Ccb_halfedge_circulator ec_start = vd.ccb_halfedges(*fc);
      Ccb_halfedge_circulator ec = ec_start;
      do {
        	//print_endpoint(ec, false);
		bool is_src = false;
		Halfedge_handle e = ec;
		if ( is_src ) {
    		if ( e->has_source() )  vfaces[fcc].push_back(Point(e->source()->point()));
    		else  vfaces[fcc].push_back(Point(1500,1500));
  		} 
		else {
    		if ( e->has_target() )  vfaces[fcc].push_back(Point(e->target()->point()));
    		else  vfaces[fcc].push_back(Point(1500,1500));
  		}
		
	 

      } while ( ++ec != ec_start );
     fcc = fcc + 1;
     //std::cout << "" << std::endl;
  }
   

  for(int j =0 ; j< numcentroids; j++)
  {
	//std::cout << "FACE POINTS:" << std::endl;
  	fcit = vfaces[j].begin();
  	for(fcit = vfaces[j].begin(); fcit!= vfaces[j].end(); fcit++)
  	{
		//std::cout << "Facepoint:" << *fcit << std::endl;
  	}

  }


  

  std::ifstream ifq("data.cin");
  assert( ifq );

  Point_2 p;
  while ( ifq >> p ) {
    //std::cout << "Query point (" << p.x() << "," << p.y()
             // << ") lies on a Voronoi " << std::flush;

    Locate_result lr = vd.locate(p);
    if ( Vertex_handle* v = boost::get<Vertex_handle>(&lr) ) {
      //std::cout << "vertex." << std::endl;
      //std::cout << "The Voronoi vertex is:" << std::endl;
      //std::cout << "\t" << (*v)->point() << std::endl;

       Point q = Point((*v)->point());
	for(int j =0 ; j< numcentroids; j++)
  	{
		fcit = vfaces[j].begin();
  		for(fcit = vfaces[j].begin(); fcit!= vfaces[j].end(); fcit++)
  		{
			if(((*fcit).x() ==  q.x()) && ((*fcit).y() ==  q.y()))
			{
				
				facepoints[j].push_back(Point(p));
				break;

			}
				
		}

  	}

      


    } 


     else if ( Halfedge_handle* e = boost::get<Halfedge_handle>(&lr) )
     {
      	//std::cout << "edge." << std::endl;
      	//std::cout << "The source and target vertices "
                //<< "of the Voronoi edge are:" << std::endl;
	bool is_src = false;
      	//print_endpoint(*e, true);
	Point pcomp1,pcomp2;
	std::vector<Point>::iterator pit1,pit2;
       Halfedge_handle et = *e;
	is_src = true;
	if ( is_src ) 
	{
    		if ( et->has_source() )  pcomp1= Point(et->source()->point());
    		else  pcomp1 = Point(1500,1500);
  	}
       is_src = false;
	//print_endpoint(*e, false);
	if(!is_src)
	{

		if ( et->has_target() )  pcomp2 = Point(et->target()->point());
    		else  pcomp2 = Point(1500,1500);


	}
      	
       for(int j = 0; j < numcentroids; j++)
	{
		pit1 = find(vfaces[j].begin(),vfaces[j].end(),pcomp1);
		pit2 = find(vfaces[j].begin(),vfaces[j].end(),pcomp2);

		if((pit1 != vfaces[j].end()) && (pit2 != vfaces[j].end()))
		{
			facepoints[j].push_back(Point(p));
			
			break;
		}

	}
	
	
    } 
	else if ( Face_handle* f = boost::get<Face_handle>(&lr) ) {
      //std::cout << "face." << std::endl;
      
      //std::cout << "The vertices of the Voronoi face are"
               // << " (in counterclockwise order):" << std::endl;
      Ccb_halfedge_circulator ec_start = (*f)->outer_ccb();
      Ccb_halfedge_circulator ec = ec_start;
      Points pcomp;
      do 
      {
		//print_endpoint(ec, false);
		bool is_src = false;
		Halfedge_handle e = ec;
		
		if ( is_src ) 
		{
    			if ( e->has_source() )  pcomp.push_back(Point(e->source()->point()));
    			else  pcomp.push_back(Point(1500,1500));
  		} 
		else 
		{
    			if ( e->has_target() )  pcomp.push_back(Point(e->target()->point()));
    			else  pcomp.push_back(Point(1500,1500));
  		}
		
        
      } while ( ++ec != ec_start );


	// COMPARE AGAINST FACES AND COLLECT CORRESPONDING POINTS

	bool is_equal = false;
       for(int j =0 ; j< numcentroids; j++)
  	{
		if(pcomp.size() < vfaces[j].size())
		{
	  		is_equal = std::equal(pcomp.begin(),pcomp.end(),vfaces[j].begin());
		}
		else
		{
			is_equal = std::equal(vfaces[j].begin(),vfaces[j].end(),pcomp.begin());
		}

		if(is_equal)
		{
			is_equal = false;
			facepoints[j].push_back(Point(p));
			break;
		}
  	}


    }
    //std::cout << std::endl;
  }
  ifq.close();

   for(int j =0 ; j< numcentroids; j++)
  {
	//std::cout << "FACES:" << std::endl;
  	fcit = vfaces[j].begin();
  	for(fcit = vfaces[j].begin(); fcit!= vfaces[j].end(); fcit++)
  	{
		//std::cout << "Facepoint:" << *fcit << std::endl;
  	}

  }

   for(int j =0 ; j< numcentroids; j++)
  {
	//std::cout << "FACE POINTS:" << std::endl;
  	fcit = facepoints[j].begin();
  	for(fcit = facepoints[j].begin(); fcit!= facepoints[j].end(); fcit++)
  	{
		//std::cout << "Facepoint:" << *fcit << std::endl;
  	}

  }

  // UPDATE THE CENTROIDS HERE
 
   oc = oldcentroids.begin();
   newcentroids.clear();
   

   for(int j =0 ; j< numcentroids; j++)
  {

	
	if(facepoints[j].empty())
	{
		if(oc!= oldcentroids.end())
		{
			newcentroids.push_back(Point(*oc));
			
			oc++;
		}
	}
	else
	{
		// COMPUTE THE CENTROID OF ALL POINTS AND UPDATE THE NEW CENTROID
		

		Point newcent = centroid(facepoints[j].begin(),facepoints[j].end(),tg);
		newcentroids.push_back(newcent);
			
		oc++;
		

  		
	}

  }

  // PRINT THE CENTROIDS
   oc = oldcentroids.begin();
   nc = newcentroids.begin();

  std::cout << "OLD CENTROIDS ARE...: " << std::endl;

  for(oc = oldcentroids.begin(); oc!= oldcentroids.end();oc++)
  {
	std::cout << (*oc) << std::endl;

  }

  std::cout << "NEW CENTROIDS ARE...: " << std::endl;
  std::cout << "[[" ;
  for(nc = newcentroids.begin(); nc!= newcentroids.end();++nc)
  {
	if(nc != newcentroids.end())
	{
		std::cout << (*nc).x() << ",";
	}
	else
	{
		std::cout << (*nc).x();
	}

  }
  std::cout << "],[";
  for(nc = newcentroids.begin(); nc!= newcentroids.end();++nc)
  {
	if(nc != newcentroids.end())
	{
		std::cout << (*nc).y() << ",";
	}
	else
	{
		std::cout << (*nc).y();
	}

  }
  std::cout << "]]" << std::endl;
}


int main()
{
       int kiter = 40, iiter = 0;
       bool is_equal = false;
       freopen("Clusteringresults.txt","w",stdout);
	while(1)
	{
		if(iiter != kiter)
		{
			if(iiter == 0)
			{
				computenewcentroids(iiter);
				croppedvoronoi();
				iiter = iiter + 1;

			}
			else
			{
				computenewcentroids(2);
  				sleep(1);
				croppedvoronoi();
				iiter = iiter + 1;
			
			}


			if(newcentroids.size() < oldcentroids.size())
			{
	  			is_equal = std::equal(newcentroids.begin(),newcentroids.end(),oldcentroids.begin());
			}
			else
			{
				is_equal = std::equal(oldcentroids.begin(),oldcentroids.end(),newcentroids.begin());
			}

			if(is_equal)
			{
				is_equal = false;
				std::cout << "Reached Convergence.." << std::endl;
				// PRINT THE CENTROIDS
				std::vector<Point>::iterator oc,nc;
   				oc = oldcentroids.begin();
   				nc = newcentroids.begin();

  				std::cout << "OLD CENTROIDS ARE: " << std::endl;

  				for(oc = oldcentroids.begin(); oc!= oldcentroids.end();oc++)
  				{
					std::cout << (*oc) << std::endl;

  				}

  				std::cout << "NEW CENTROIDS ARE: " << std::endl;

  				for(nc = newcentroids.begin(); nc!= newcentroids.end();nc++)
  				{
					std::cout << (*nc) << std::endl;

  				}



				 for(int j =0 ; j< numcentroids; j++)
 				 {
					std::cout << "CLUSTER " << j << " POINTS:" << std::endl;
  					fcit = facepoints[j].begin();
  					for(fcit = facepoints[j].begin(); fcit!= facepoints[j].end(); fcit++)
  					{
						std::cout << *fcit << std::endl;
  					}

  				}
				
				for(int j = 0; j< numcentroids; j++)
				{
					std::cout << "CLUSTER " << j << " HAS " << facepoints[j].size() << " POINTS" <<  std::endl;


				}
				std::cout << "Exiting..." << std::endl;
                                fclose(stdout);
				exit(1);
							
			}




		}
		else
		{
			std::cout << "Reached maximum number of iterations - Exiting..." << std::endl;
			fclose(stdout);
			exit(0);

		}	


	}



}


