#include "config.h"
#include "util.h"

#include <math.h>
#include <string.h>


void vector_cross(float vout[3], float v1[3],float v2[3])
{
  vout[0]= (v1[1]*v2[2]) - (v1[2]*v2[1]);
  vout[1]= (v1[2]*v2[0]) - (v1[0]*v2[2]);
  vout[2]= (v1[0]*v2[1]) - (v1[1]*v2[0]);
}

extern float rxcopy[];
extern float GEstG[3];
extern float Q_rsqrt( float number );


float errorvect[3];

void stick_vector( float maxangle , int inverted)
{
	
float stickvector[3];	
	
	// start with a down vector
	stickvector[0] = 0;
	stickvector[1] = 0;
	stickvector[2] = 1;

float pitch, roll;

	// rotate down vector to match stick position
pitch = rxcopy[1] * maxangle * DEGTORAD + (float) TRIM_PITCH;
roll = rxcopy[0] * maxangle * DEGTORAD + (float) TRIM_ROLL;

stickvector[0] = fastsin( roll );
stickvector[1] = fastsin( pitch );
stickvector[2] = fastcos( roll ) * fastcos( pitch );

float mag2 = Q_rsqrt( (stickvector[0] * stickvector[0] + stickvector[1] * stickvector[1]) / (1 - stickvector[2] * stickvector[2]));
stickvector[0] *=mag2;
stickvector[1] *=mag2;	

if ( inverted )
{
	stickvector[0] = - stickvector[0];
	stickvector[1] = - stickvector[1];
	stickvector[2] = - stickvector[2];
}

float g_vect[3];
	for ( int i = 0 ; i <3; i++)
		g_vect[i] = GEstG[i] * ( 1/2048.0f);

// find error between stick vector and quad orientation
// vector cross product (optimized) 
  errorvect[1]= - ( (g_vect[1] * stickvector[2]) - (g_vect[2]*stickvector[1]) );
  errorvect[0]= (g_vect[2] * stickvector[0]) - (g_vect[0]*stickvector[2]);

// some limits just in case
limitf( & errorvect[0] , 1.0);
limitf( & errorvect[1] , 1.0);


// fix to recover if triggered inverted
if ( inverted && (GEstG[2] > 1000) )
{
	// set maximum roll
	errorvect[0] = 1.0;
}
// fix to recover if triggered inverted
if ( !inverted && (GEstG[2] < -1000) )
{
	// set maximum roll
	errorvect[0] = 1.0;
}

}



