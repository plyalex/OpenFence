

#ifndef Geofence_h
#define Geofence_h

#include "Arduino.h"

struct position{
  float lat; //x
  float lon; //y
};
struct datetime{
  uint32_t time;
  uint32_t date;
};
struct fenceProperty{
  int sideOutside; 
  float distance;
  float bearing;
};

class Geofence 
{
	public:
		Geofence(void);
		float distance(struct position v, struct position w);
		bool pointInPolygon(struct position me, struct position fencePoint[], int polyCorners);
		fenceProperty geofence(struct position me, struct position fencePoint[], int polyCorners);

	private:
		float degrees2radians(float degrees);
		float sqr(float x);
		float distBehind(struct position me, struct position w, struct position v);
		position findProjection(struct position me, struct position v, struct position w);
		//float dist2segment(struct position p, struct position v, struct position w, struct position * projection);
		float bearing2fence(struct position me, struct position projection);
};


#endif
