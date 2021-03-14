/**
 *
 *   types.h
 *
 *   Library of typedef and macros used in the whole project.
 *
 *   Project    PERSEO 2.0
 *
 */



#ifndef __TYPES_H__
#define __TYPES_H__


/** 
 *  An index unambiguous addressing a neuron in the network.
 */

typedef unsigned int indexn;


/** 
 *  A generic real number, useful to define state variables 
 *  of the dynamics element in the network.
 */

typedef double real;


/**
 *  Structure representing a time.
 *  It use in order to have a large numeric precision
 *  like in the case of huge external frequencies of 
 *  incoming events when the simulation time spans 
 *  throughout long period of time (hours). 
 *  Seconds + Millis represents the time expressed 
 *  in double.
 */

typedef struct {
           int    Seconds; /* Seconds of time.                          */
           double  Millis; /* Milliseconds of time: it should not be    *
                            * never greater than 1.0, in order to avoid *
                            * lost of numeric precision.                */
        } timex;


/**
 *  A byte (8 bit), useful to address the distance between 
 *  two consecutive synapse, or to single out a synaptic 
 *  efficacy from a look-up table, keeping reduced the amount 
 *  of memory hosting synaptic matrix.
 */

typedef unsigned char byte;


/** 
 *  A spike defined by the emission time, the index to the 
 *  emitting neuron (or receiving neuron, if the event comes 
 *  from the outside), and the time interval from the 
 *  preceding spike, if any.
*/

typedef struct {
           timex Emission; /* Emission time, or the receiving time for a spikes *
                            * coming from the outside..                         */
           indexn  Neuron; /* Index to the emitting (or receiving) neuron (if   *
                            * the event comes from the outside).                */
           real       ISI; /* Time interval from the last spike emitted (or     *
                            * received), if any.                                */
        } spike;



/*------------------------------*
 *  MACROS FOR TIMEX HANDLING.  *
 *------------------------------*/

/**
 *  Converts timex into a double, expressing time in ms.
 *  The operation may lose precision.
 */

#define timexToDouble(t)      (t.Seconds+t.Millis)


/**
 *  Converts a double, expressing a time in ms, into a timex structure.
 *  No numeric precision is lost.
 */

#define doubleToTimex(d,t)     t.Seconds = (int)(d); \
                               t.Millis  = (d) - t.Seconds*1.0;


/**
 *  Returns the difference (t1 - t2) in time between two timex 
 *  structures t1 e t2, expressed in double. It is useful for 
 *  small time interval (lower than a second). In this case no 
 *  numeric precision is lost.
 */

#define diffTimex(t1,t2)      ((t1).Seconds == (t2).Seconds ? \
                              ((t1).Millis-(t2).Millis) :     \
                              ((t1).Seconds-(t2).Seconds)*1.0+(t1).Millis-(t2).Millis)
 /*
#define diffTimex(t1,t2)      (((t1).Seconds-(t2).Seconds)*1000.0+ \
                               (t1).Millis-(t2).Millis)
 */


#ifndef _WIN32

/**
 *  Return the maximum between two number.
 *  The macro is required 
 */

#define max(a,b) ((a)>(b) ? (a) : (b))

#endif


/**
 *  This is an empty string, useful to allocate
 *  and initialize it a string to assign a char 
 *  pointer.
 */

#define EMPTY_STRING "                                                          "



#endif /* __TYPES_H__ */
