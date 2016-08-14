#include "Geofence.h"

//Macros
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


Geofence::Geofence(){
;;
}


float Geofence::degrees2radians(float degrees){
    return degrees*M_PI/180;
}
float Geofence::sqr(float x){
  return x*x;
}


//Step 1: Equirectangular Approximation Distance Calculation Function
float Geofence::distance(struct position v, struct position w) {   //Equirectangular Approximation
  float p1 = degrees2radians(v.lon - w.lon)* cos( 0.5*degrees2radians(v.lat+w.lat) ) ;
  float p2 = degrees2radians(v.lat - w.lat);
  return 6371000 * sqrt( p1*p1 + p2*p2);
} 


//Step 2: Test if the point is within the polygon of points
float Geofence::pointInPolygon(struct position p, struct position points[], int polyCorners) {
  //Based on http://alienryderflex.com/polygon/
  //oddNodes = 1 means within the polygon, oddNodes = 0 outside the polygon.
  int   i, j=polyCorners-1 ;
  bool  oddNodes=0;
  float maxbehind = 0,temp=0;
  int sidebehind;

  for(i=0; i<polyCorners; i++) {		
    if(((points[i].lat< p.lat && points[j].lat>=p.lat) 
      || (points[j].lat< p.lat && points[i].lat>=p.lat))  
      &&  (points[i].lon<=p.lon || points[j].lon<=p.lon)) {
      oddNodes^=(points[i].lon+(p.lat-points[i].lat) / 
        (points[j].lat-points[i].lat)*(points[j].lon-points[i].lon)<p.lon); 
    }
    j=i; 
  }

  if(!oddNodes){
  	 if( (distBehind(p,points[polyCorners-1],points[0])*100000) < 0) {
  	 	maxbehind=dist2segment(p,points[polyCorners-1],points[0]);
  	 	sidebehind=0;
  	 }
  	for(int i=1; i<polyCorners; i++){ 
    	if( (distBehind(p,points[i-1],points[i])*100000) <0) {
    		temp = dist2segment(p,points[i-1],points[i]);
    		if (temp > maxbehind) {
    			maxbehind = temp; //if further away from side update the max distance behind
    		 	sidebehind=i;
			}
    	}
    }
  }	
  //Determine left or right hand side alert

  return maxbehind; 
}

//Step 3: Find which sides of the boundary we are outside
float Geofence::distBehind(struct position p, struct position w, struct position v){
  //Returns a negative if outside that boundary.
  float Fplat =w.lat;
  float Fplon =w.lon;
  //Calculate the unit length normal vector: Fn
  float Fnlat =  w.lon-v.lon;       // y' 
  float Fnlon = - (w.lat-v.lat);    //-x'
  float mag=sqrt(sqr(Fnlat)+sqr(Fnlon));
  Fnlat /= mag;           //Unit length
  Fnlon /= mag;
  //p-Fp
  Fplat = p.lat- Fplat;   //Reuse variables
  Fplon = p.lon - Fplon;
  //Return the dot product
  return Fplat*Fnlat + Fplon*Fnlon;
}

//Step 4: Get an accurate shortest distance to a side of the fence
float Geofence::dist2segment(struct position p, struct position v, struct position w){
  //Check if the two side points are in the same location (avoid dividing by zero later)
  float l = distance(v,w);
  if(l==0) return distance(p,v);
  //Find the max and min x and y points
  float minx = MIN(v.lat, w.lat);
  float maxx = MAX(v.lat, w.lat);
  float miny = MIN(v.lon, w.lon);
  float maxy = MAX(v.lon, w.lon);

  struct position projection;
  if(p.lat<minx && p.lon<miny){         //p does not fall between the two points and is closest to the lower corner
    projection.lat = minx; 
    projection.lon = miny; 
  }else if(p.lat>maxx && p.lon>maxy){   //p does not fall between the two points and is closest to the lower corner
    projection.lat = maxx; 
    projection.lon = maxy; 
  }else{                                //p does fall between the two points, project point onto the line
    float x1=v.lat, y1=v.lon, x2=w.lat, y2=w.lon, x3=p.lat, y3=p.lon;
    float px = x2-x1, py = y2-y1, dAB = px*px + py*py;
    float u = ((x3 - x1) * px + (y3 - y1) * py) / dAB;
    float x = x1 + u * px, y = y1 + u * py;
    projection.lat = x; 
    projection.lon = y; 
  }
  //Return the distance to the closest point on the line.
  return distance(p, projection);
}