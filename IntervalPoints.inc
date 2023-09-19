/****h* ScPovPlot3D/IntervalPoints.inc
* PURPOSE
*  This is |html <b>preliminary edition</b>, I'll try to implement some interval formalism (implementation of "fuzzy points") to kriging algorithm in the future. Welcome comments!
*
*     image:./imgs/IntervalKrigging-01.png
*     |html <hr width=50% align="left">
*     image:./imgs/IntervalKrigging-02.png
*
*   Fig.[IntervalPoints] double 3D surface modelled by multiple Bezier patches objects
*   |html <hr width=50% align="left">
*     *********************************************************
*     **   Tested on PovRay 3.7                              **
*     **   License: GNU GPL                                  **
*     **   Homepage:    http://scpovplot3d.sourceforge.net   **
*     *********************************************************
*     **   version: 3.0.6 (& have a nice time ;)             **
*     *********************************************************
* AUTHOR
*  Janusz Opi³a Ph.D.
*   jmo@agh.edu.pl, janusz.opila@gmail.com
*   Dept. of Applied Informatics
*   AGH University of Science & Technology, Cracow, Poland
*   Maintained by Janusz Opi³a Ph.D.
* COPYRIGHT
*  GNU GPL v.3 License
*  (c) 2012-now by Janusz Opi³a Ph.D.         
*  AGH University of Science and Technology
*
****
  end of RoboDoc comment
****/

#ifndef(IntervalPoints_INC)
#declare IntervalPoints_INC = "2014-05-25"; // last modification date
#version 3.7;

#declare _FileName   = "IntervalPoints.INC";
#declare _FileVerMaj = "3.1.0.1";
#declare _FileVerMin = IntervalPoints_INC;

#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

#ifndef (Debug) // Just 4 safety
   #declare Debug=0;
#end

// a lot of stone textures: T_Stone1 - T_Stone44
// Standard pre-defined colors
//
#include "math.inc"
#include "colors.inc"
#include "textures.inc"
#include "stones1.inc"


#include "dev02\Plot3Dv3\CommonDefs.inc"
#include "dev02\Plot3Dv3\Cameras.inc"
#include "dev02\Plot3Dv3\WireSurf.inc"
#include "dev02\Plot3Dv3\Mesh2Surf.inc"

#macro SQR(A)
    (A)*(A)
#end

#declare Photons=true;
global_settings{
  ambient_light color rgb <.41,.41,.9> // sets an overall brightness/ambient light level in the scene
  max_trace_level 5                 // sets the maximum ray tracing bounce depth (1 or more) [5]
  assumed_gamma 1.0
  #if (Photons)                     // global photon block try to tune to Your needs
    photons {
      spacing 0.02               // specify the density of photons
      media 100
      count 100000               // alternatively use a total number of photons
      jitter 1.0                 // jitter phor photon rays
      max_trace_level 460         // optional separate max_trace_level
      adc_bailout 1/255          // see global adc_bailout
      autostop 0                 // photon autostop option
      radius 10                  // manually specified search radius
      expand_thresholds 0.2, 40  // (---Adaptive Search Radius---)
    }
  #end

 #if (radio)
  radiosity{
      pretrace_start 0.08
      pretrace_end   0.01
      count 150
      nearest_count 10
      error_bound 0.5
      recursion_limit 3
      low_error_factor 0.5
      gray_threshold 0.0
      minimum_reuse 0.005
      maximum_reuse 0.2
      brightness 1
      adc_bailout 0.005
  }
  #end
}


background {rgb 0}
//camera { location <35.1,17,-42> look_at <15,0,-12.5> angle 50 }
//camera { location < -7,17,7> look_at <5,1,-3> angle 50 }
//camera { location <17,12,5> look_at <13, 0,-4> angle 50 }
//camera { location <16, 10,-12> look_at <12, 0,-6> angle 50 }
//camera { location <-6, 10,-14> look_at <8, 0,-3> angle 50 }

light_source {<500, 500, -500> color rgb<.721, .721, .721> }
light_source {< 20,  10,  -10> color rgb<.31, .31, .31> }   // natural light source and shadowless 2
light_source {<  5,  20,   -4> color rgb<.31, .31, .31> }     // natural light source and shadowless 2
light_source {< 10, -20,  -25> color rgb<.31, .31, .31> }

// soft light (areal_light) ---------  An area light (creates soft shadows)
// WARNING: This special light can significantly slow down rendering speed!
light_source { <10,20,-15>
               color rgb <.4, .7, .7>    // light's color
               area_light                // kind of light source (Art der Lichtquelle)
               <12, 0, 0> <0, 0, 12>     // lights spread out across this distance (x * z)
               10, 8                     // total number of lights in grid (4x*4z = 16 lights)
               adaptive 2                // 0,1,2,3...
               jitter                    // adds random softening of light
               circular
               translate <0, 10, -6>      // <x,y,z> position of light
             } //--- end of area_light ---//

// defaults settings for safety

// read data from file or set directly by function, best way is to use
// Qds[] matrix to point to cardinal points
// Func -> f(x,y) => #declare abc=function(x,y){x-y}
// x1, x2, Nxx    => x-min, x-max, num. of cardinal (exact value) points
// y1, y2, Nyy    => the same but for y axis
// Structure of input file:
// * -----> Y ---->
// |  x1y1 x1y2 x1y3 x1y4
// |  x2y1
// |X x3y1...
// V  X4y1..
#macro MatrixFileSurface(InFileName)
    #fopen DataFile InFileName read
    #read(DataFile, DescriptX, DescriptY)
    #read(DataFile, Nx, x1, x2)
    #read(DataFile, Ny, y1, y2)
    #if ((Nx>1)&(Ny>1)) // at least 2x2 mesh

      #declare Nodes=array[Ny][Nx];    // float: height of nodes only, x, y defined directly
      #declare Nodes[0][0]=0.0;        // define as float!

      #declare MaxNx=3*(Nx-1)+1;       // number of all nodes (cardinal+control) in 'x' direction
      #declare MaxNy=3*(Ny-1)+1;       // number of all nodes (cardinal+control) in 'y' direction

      #declare AllVertices = MaxNx*MaxNy;
      #declare PtV = array[AllVertices];       // vectors, real coords of all points
      #declare Pts = array[MaxNy][MaxNx];      // helper pointers to cardinal vectors PtV[]
      #declare Qds = array[Ny-1][Nx-1][4][4];  // array of integer pointers to vector, all patch cardinal&control points

// Initialization of all points
      #declare iy=0;
      #while(iy<Ny)
        #declare ix=0;
        #while(ix<Nx)
//#debug concat("\n\n********\nCountX= ",str(CountX,5,0), "\n")
//#debug concat("CountY= ",str(CountY,5,0), "\n********\n\n")
          #read(DataFile, Nodes[iy][ix])
          #declare ix=ix+1;
        #end
        #declare iy=iy+1;
      #end

      #local deltax = (x2-x1)/(MaxNx-1);
      #local deltay = (y2-y1)/(MaxNy-1);

      #declare iy = 0;
      #while (iy < MaxNy)
         #declare ix = 0;
         #while (ix < MaxNx) // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!
            #local zpov = -ix*deltax-x1; // x //!! changed 2012-05-10
            #local xpov = iy*deltay+y1;  // y //!! changed 2012-05-10

//!! changed 2012-05-10
            #declare PtV[MaxNx*iy+ix] = <xpov, 0, zpov>;  // 'y' for cardinal points define later on
            #declare Pts[iy][ix] = MaxNx*iy+ix;            // indices to the vector above
            #declare ix = ix+1;
         #end
         #declare iy = iy+1;
      #end

// I think it should be changed x<>z due to POvRay Coord System - done
      #declare SceneMaxX = 0;  #declare SceneMinX = 0;
      #declare SceneMaxY = 0;  #declare SceneMinY = 0;
      #declare SceneMaxZ = 0;  #declare SceneMinZ = 0;

      #declare iy=0;
      #while (iy<Ny)
         #declare ix=0;
         #while (ix<Nx) // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!
// now define main storage table, cardinal points only
            #local PtVCurr=PtV[3*(MaxNx*iy+ix)];
            #local xpov = PtVCurr.x; // y real
            #local zpov = PtVCurr.z; // x real
            #local ypov = VScale*Nodes[iy][ix]+VShift; // z
            #declare PtV[3*(MaxNx*iy+ix)] = <xpov, ypov, zpov>;

// evaluate bounding cube in PovRay coords..
            #local PtVCurr=PtV[3*(MaxNx*iy+ix)];
            #if (PtVCurr.x > SceneMaxX)
               #declare SceneMaxX=PtVCurr.x;
            #end
            #if (PtVCurr.x < SceneMinX)
               #declare SceneMinX=PtVCurr.x;
            #end

            #if (PtVCurr.y > SceneMaxY)
               #declare SceneMaxY=PtVCurr.y;
            #end
            #if (PtVCurr.y < SceneMinY)
               #declare SceneMinY=PtVCurr.y;
            #end
            // left-handed coordinate system: Z-axis is reversed!!
            #if (PtVCurr.z < SceneMaxZ)
               #declare SceneMaxZ=PtVCurr.z;
            #end
            #if (PtVCurr.z > SceneMinZ)
               #declare SceneMinZ=PtVCurr.z;
            #end

            #declare ix=ix+1;
         #end
         #declare iy=iy+1;
      #end
      #declare ZCoeff=(SceneMaxY>SceneMinY?1.0/(SceneMaxY-SceneMinY):1.0); // to avoid division by zero for flat surfaces..
// finally create all patches:
     EvalPatchedSurface()
// ready!
    #else
      #debug "* * * * * * * * * * * * * * * * * * * * * * * *\n"
      #debug "* To few nodes. You need at least 2x2=4 nodes! *\n"
      #debug "* * * * * * * * * * * * * * * * * * * * * * * *\n"
    #end  // main if#if ((Nx>1)&(Ny>1)) // at least 2x2 mesh
#end


// Func -> f(x,y) => #declare abc=function(x,y){x-y}
// x1, x2, Nxx    => x-min, x-max, num. of cardinal (exact value) points
// y1, y2, Nyy    => the same but for y axis
#macro FunctionSurface(Func, x1, x2, Nxx, y1, y2, Nyy)

   #declare Nx=(Nxx>1?Nxx:2);     // at least 2 nodes
   #declare MaxNx=3*(Nx-1)+1;     // number of all nodes (cardinal+control) in 'x' direction

   #declare Ny=(Nyy>1?Nyy:2);     // at least 2 nodes
   #declare MaxNy=3*(Ny-1)+1;     // number of all nodes (cardinal+control) in 'y' direction

   #declare AllVertices = MaxNx*MaxNy;
   #declare PtV=array[AllVertices];       // vectors, real coords of all points
   #declare Pts=array[MaxNy][MaxNx];      // helper pointers to cardinal vectors PtV[]
   #declare Qds=array[Ny-1][Nx-1][4][4];  // array of integer pointers to vector, all patch cardinal&control points
   // Initialization of all points
   #declare iy=0;
   #while (iy<MaxNy)
      #declare ix=0;
      #while (ix<MaxNx) // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!
         #declare PtV[MaxNx*iy+ix] = <0, 0, 0>;
         #declare Pts[iy][ix]= MaxNx*iy+ix;               // indices to the vector above
         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end

   #declare SceneMaxX=0;
   #declare SceneMinX=0;

   #declare SceneMaxY=0;
   #declare SceneMinY=0;

   #declare SceneMaxZ=0;
   #declare SceneMinZ=0;

   #local deltax=(x2-x1)/(MaxNx-1);
   #local deltay=(y2-y1)/(MaxNy-1);

   #declare iy=0;
   #while (iy<MaxNy)
      #declare ix=0;
      #while (ix<MaxNx) // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!

//!! changed 2012-05-10
         #local xreal = ix*deltax+x1; // x
         #local yreal = iy*deltay+y1; // y
         #local ypov = VScale*Func(xreal, yreal)+VShift;     // z
///         #declare PtV[MaxNx*iy+ix] = <xpov, ypov, -zpov>; // real vectorsw/points - original
         #declare PtV[MaxNx*iy+ix] = <yreal, ypov, -xreal>;  // real vectorsw/points in PovRay coord system : experimental in real world-[0Y,0Z,0X]
         #local PtVCurr=PtV[MaxNx*iy+ix];

         #if (PtVCurr.x > SceneMaxX)
            #declare SceneMaxX=PtVCurr.x;
         #end
         #if (PtVCurr.x < SceneMinX)
            #declare SceneMinX=PtVCurr.x;
         #end

         #if (PtVCurr.y > SceneMaxY)
            #declare SceneMaxY=PtVCurr.y;
         #end
         #if (PtVCurr.y < SceneMinY)
            #declare SceneMinY=PtVCurr.y;
         #end
         // left-handed coordinate system: Z-axis is reversed!!
         #if (PtVCurr.z < SceneMaxZ)
            #declare SceneMaxZ=PtVCurr.z;
         #end
         #if (PtVCurr.z > SceneMinZ)
            #declare SceneMinZ=PtVCurr.z;
         #end

         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end
   #declare ZCoeff=1/(SceneMaxY-SceneMinY);
// finally create all patches:
  EvalPatchedSurface()
// ready!
#end


// The most configurable version
// Potential -> f(x,y) => smoothing potential, 1/r, 1/r^2, .., exp(-a*r), ...
// Input file:
// "Comment X", "Comment Y"
// num_of_data
// x_1, y_1, z_1
// x_2, y_2, z_2
// .............
// x1, x2, Nxx    => x-min, x-max, num. of cardinal (exact value) points
// y1, y2, Nyy    => the same but for y axis
// Smooth   => smoothing potential function
// Singular => what to do if grid point is too close to data dile point
// ex: with potential funcion V(r1, r0):=1/|r1-r0| there singularity at point
//     r1=r0. So we need special function for solving this problem.
// Wrapper for backward compatibility

#macro RawDataSurfaceExt(InFileName, Smooth, Singular, x1, x2, Nxx, y1, y2, Nyy)
    RawDataSurface(InFileName, Smooth, Singular, 1.0, x1, x2, Nxx, y1, y2, Nyy)
#end

#macro RawDataSurface(InFileName, Smooth, Singular, sigma, x1, x2, Nxx, y1, y2, Nyy)

// Now open data file and import all points
   #fopen DataFile InFileName read
   #read(DataFile, DescriptX, DescriptY, DescriptZ)
   #read(DataFile, NumPts)
   #declare RNodes = array[NumPts];    // float vector: height of nodes AND x, y
   #declare RNodes[0] = <0.0, 0.0, 0.0>;
   #declare rix=0;

   #while(rix < NumPts)
      #read(DataFile, xx, yy, zz)
      #declare RNodes[rix] = <xx, yy, zz>; // real world
      #declare rix = rix + 1;
   #end
   #fclose DataFile
//==========================//

   #declare Nx=(Nxx>1?Nxx:2);     // at least 2 nodes
   #declare MaxNx=3*(Nx-1)+1;     // number of all nodes (cardinal+control) in 'x' direction

   #declare Ny=(Nyy>1?Nyy:2);     // at least 2 nodes
   #declare MaxNy=3*(Ny-1)+1;     // number of all nodes (cardinal+control) in 'y' direction

   #declare AllVertices = MaxNx*MaxNy;
   #declare PtV=array[AllVertices];       // vectors, real coords of all points
   #declare Pts=array[MaxNy][MaxNx];      // helper pointers to cardinal vectors PtV[]
   #declare Qds=array[Ny-1][Nx-1][4][4];  // array of integer pointers to vector, all patch cardinal&control points
// Initialization of ALL points
//   #declare iy=0;
//   #while (iy<MaxNy)
//      #declare ix=0;
//      #while (ix<MaxNx)                   // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!
//         #declare PtV[MaxNx*iy+ix] = <0, 0, 0>;
//         #declare Pts[iy][ix]= MaxNx*iy+ix;     // indices to the vector above
//         #declare ix=ix+1;
//      #end
//      #declare iy=iy+1;
//   #end
// Now initialize arrays and define Nodes array
   #local deltax = (x2-x1)/(MaxNx-1);
   #local deltay = (y2-y1)/(MaxNy-1);

   #declare iy = 0;
   #while (iy < MaxNy)
      #declare ix = 0;
      #while (ix < MaxNx) // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!
         #local xpov = ix*deltax+x1; // x
         #local zpov = iy*deltay+y1; // y

         #declare PtV[MaxNx*iy+ix] = <zpov, 0, -xpov>;   // here corrected: <xpov, 0, -zpov>;  // 'y' for cardinal points define later on
         #declare Pts[iy][ix] = MaxNx*iy+ix;            // indices to the vector above
         #declare ix = ix+1;
      #end
      #declare iy = iy+1;
   #end

   // I think it should be chaanged x<>y due to POvRay Coord System
   #declare SceneMaxX = y2;   #declare SceneMinX = y1;
   #declare SceneMaxY =0.0;   #declare SceneMinY =0.0;
   #declare SceneMaxZ =-x2;   #declare SceneMinZ =-x1;   // little bit strange MaxZ < MinZ!!

//   #declare Nodes = array [Ny][Nx];
//   #declare Nodes[0][0] = 0.0;

// delta's for Qts matrix
   #declare iy=0;
   #declare wghtSum = 0.0;
   #declare wght = 0.0;
   #while (iy<Ny)
      #declare ix=0;
      #while (ix<Nx)              // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!
         #local PtVCurr = PtV[3*(MaxNx*iy+ix)];

         #local xpov =  PtVCurr.x; //   x
         #local zpov = -PtVCurr.z; // --y because I need real world values

         #local ypov = 0.0;
         #local rix  = 0;
         #declare wghtSum = 0.0;
         #declare wght= 0.00;
         #while(rix<NumPts) // take into account all points from input file
//            #declare d  = sqrt(pow(xpov-RNodes[rix].x,2)+pow(zpov-RNodes[rix].y,2));
            #declare d  = sqrt(pow(zpov-RNodes[rix].x,2)+pow(xpov-RNodes[rix].y,2))/sigma; // distance between approximated and reference points ///corrected
            #declare wght = (d>DZero?Smooth(d):Singular(d));
            #declare wghtSum = wghtSum+wght;
            #local ypov = ypov+RNodes[rix].z*wght;
            #local rix=rix+1;
         #end
 //        #local ypov = ypov/NumPts;  // normalization
         #declare wghtSum = (wghtSum>0?wghtSum:1.0);   // just for stability
         #local ypov = VScale*ypov/wghtSum+VShift;
         #declare PtV[3*(MaxNx*iy+ix)] = <xpov, ypov, -zpov>; // -zpov back to internal representation
// #declare PtV[MaxNx*iy+ix] = <xpov, ypov, -zpov>; // real vectorsw/points
//         #local PtVCurr = PtV[MaxNx*iy+ix];

//         #if (PtVCurr.x > SceneMaxX)
//            #declare SceneMaxX=PtVCurr.x;
//         #end
//         #if (PtVCurr.x < SceneMinX)
//            #declare SceneMinX=PtVCurr.x;
//         #end

         #if (ypov > SceneMaxY)
            #declare SceneMaxY=ypov;
         #end
         #if (ypov < SceneMinY)
            #declare SceneMinY=ypov;
         #end
// left-handed coordinate system: Z-axis is reversed!!
//         #if (PtVCurr.z < SceneMaxZ)
//            #declare SceneMaxZ=PtVCurr.z;
//         #end
//         #if (PtVCurr.z > SceneMinZ)
//            #declare SceneMinZ=PtVCurr.z;
//         #end

         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end
   #declare ZCoeff=1/(SceneMaxY-SceneMinY);
// finally create all patches:
  EvalPatchedSurface()
// ready!
#end

#macro ScatterDataSurfaceExp(InFileName, x1, x2, Nxx, y1, y2, Nyy, power, sigma)
// Now open data file and import all points
   #fopen DataFile InFileName read
   #read(DataFile, DescriptX, DescriptY, DescriptZ)
   #read(DataFile, NumPts)
   #declare RNodes = array[NumPts];    // float vector: height of nodes AND x, y
   #declare RNodes[0] = <0.0, 0.0, 0.0>;
   #declare rix=0;

   #while(rix < NumPts)
      #read(DataFile, xx, yy, zz)
      #declare RNodes[rix] = <xx, yy, zz>; // real world coords
      #declare rix = rix + 1;
   #end
   #fclose DataFile
//==========================//

   #declare Nx=(Nxx>1?Nxx:2);     // at least 2 nodes
   #declare MaxNx=3*(Nx-1)+1;     // number of all nodes (cardinal+control) in 'x' direction

   #declare Ny=(Nyy>1?Nyy:2);     // at least 2 nodes
   #declare MaxNy=3*(Ny-1)+1;     // number of all nodes (cardinal+control) in 'y' direction

   #declare AllVertices = MaxNx*MaxNy;
   #declare PtV=array[AllVertices];       // vectors, real coords of all points
   #declare Pts=array[MaxNy][MaxNx];      // helper pointers to cardinal vectors PtV[]
   #declare Qds=array[Ny-1][Nx-1][4][4];  // array of integer pointers to vector, all patch cardinal&control points
// Initialization of ALL points
//   #declare iy=0;
//   #while (iy<MaxNy)
//      #declare ix=0;
//      #while (ix<MaxNx)                   // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!
//         #declare PtV[MaxNx*iy+ix] = <0, 0, 0>;
//         #declare Pts[iy][ix]= MaxNx*iy+ix;     // indices to the vector above
//         #declare ix=ix+1;
//      #end
//      #declare iy=iy+1;
//   #end
// Now initialize arrays and define Nodes array
   #local deltax = (x2-x1)/(MaxNx-1);
   #local deltay = (y2-y1)/(MaxNy-1);

   #declare iy = 0;
   #while (iy < MaxNy)
      #declare ix = 0;
      #while (ix < MaxNx) // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!
         #local xpov = ix*deltax+x1; // x
         #local zpov = iy*deltay+y1; // y

         #declare PtV[MaxNx*iy+ix] = <zpov, 0, -xpov>;  // 'y' for cardinal points define later on crazy PovRay coords x>-z; y>x; z>y !
         #declare Pts[iy][ix] = MaxNx*iy+ix;            // indices to the vector above
         #declare ix = ix+1;
      #end
      #declare iy = iy+1;
   #end


   // I think it should be chaanged x<>y due to POvRay Coord System
   #declare SceneMaxX = x2;   #declare SceneMinX = x1;
   #declare SceneMaxY =0.0;   #declare SceneMinY =0.0;
   #declare SceneMaxZ =-y2;   #declare SceneMinZ =-y1;

//   #declare Nodes = array [Ny][Nx];
//   #declare Nodes[0][0] = 0.0;

// delta's for Qts matrix
   #declare iy=0;
   #while (iy<Ny)
      #declare ix=0;
      #while (ix<Nx)              // !! crazy left handed Pov Coord system: x=xpov, y=-zpov, z=y !! corrected: x=-zpow; y=xpov, z=ypow;
         #local PtVCurr = PtV[3*(MaxNx*iy+ix)];

         #local xpov =  PtVCurr.x; //   x
         #local zpov = -PtVCurr.z; // --y because I need temporary real world values

         #local ypov      = 0.0;
         #declare wghtSum = 0.0;  // sum of weights
         #declare wght    = 0.0;
         #local rix       = 0;
         #while(rix<NumPts) // now let's take into account all points from input file
            #declare d  = sqrt(pow(zpov-RNodes[rix].x,2)+pow(xpov-RNodes[rix].y,2))/sigma; // distance between approximated and reference points ///corrected
            #local wght = exp(-pow(d,power)); // specific weight
//#debug concat("d = ", str(d,10,7), "wght  ", str(exp(-pow(d, power)), 19, 17),"\n")

            #local ypov = ypov+RNodes[rix].z*wght;
            #declare wghtSum = wghtSum + wght;       // total weights
            #local rix=rix+1;
         #end

 //        #local ypov = ypov/NumPts;  // normalization
 #declare wghtSum = (wghtSum>0?wghtSum:1.0);   // just for stability
// #debug concat("wghtSum=", str(ypov,10,7), "\n")
         #local ypov = VScale*ypov/wghtSum + VShift;
         #declare PtV[3*(MaxNx*iy+ix)] = <xpov, ypov, -zpov>; // -zpov sign change back to internal representation
// #declare PtV[MaxNx*iy+ix] = <xpov, ypov, -zpov>; // real vectorsw/points
//         #local PtVCurr = PtV[MaxNx*iy+ix];

//         #if (PtVCurr.x > SceneMaxX)
//            #declare SceneMaxX=PtVCurr.x;
//         #end
//         #if (PtVCurr.x < SceneMinX)
//            #declare SceneMinX=PtVCurr.x;
//         #end

         #if (ypov > SceneMaxY)
            #declare SceneMaxY=ypov;
         #end
         #if (ypov < SceneMinY)
            #declare SceneMinY=ypov;
         #end
// left-handed coordinate system: Z-axis is reversed!!
//         #if (PtVCurr.z < SceneMaxZ)
//            #declare SceneMaxZ=PtVCurr.z;
//         #end
//         #if (PtVCurr.z > SceneMinZ)
//            #declare SceneMinZ=PtVCurr.z;
//         #end

         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end
   #declare ZCoeff=1/(SceneMaxY-SceneMinY);
// finally create all patches:
  EvalPatchedSurface()
// ready!
#end





#macro EvalPatchedSurface() // Evaluates data for func(x,y) patched bicubic surface
#declare iii = 0;
#while (iii<AllVertices)
   #declare PtV[iii]=<PtV[iii].x, PtV[iii].y - SceneMinY,PtV[iii].z>; // set zero base=> Min=0, Max=SceneMaxY-SceneMinY
   #declare iii=iii+1;
#end

#declare iy=0;
#while (iy<Ny-1)
   #declare ix=0;
   #while (ix<Nx-1)
      #declare py=0;
      #while (py<4)
         #declare px=0;
         #while (px<4)
            #declare Qds[iy][ix][py][px]=(3*iy+py)*MaxNx+3*ix+px; // indices to PtV=real vectors/points
            #declare px=px+1;
         #end
         #declare py=py+1;
      #end
      #declare ix=ix+1;
   #end
   #declare iy=iy+1;
#end

//=========================
// test #declare PtV[0]=<0,-.5,0>;
//=========================

#declare XCoeff=1/3; // this could be parametrized in the future
#declare YCoeff=1/3;
// A: Internal cross points

#declare ipy=1;
#while (ipy<Ny-1)
   #declare ipx=1;
   #while (ipx<Nx-1)
      // horizontal control points
      #declare dXVec=XCoeff*(PtV[Qds[ipy][ipx][0][3]]-PtV[Qds[ipy][ipx-1][0][0]])/2;
      #declare PtV[Qds[ipy][ipx-1][0][2]]=PtV[Qds[ipy][ipx][0][0]]-dXVec;
      #declare PtV[Qds[ipy][ipx  ][0][1]]=PtV[Qds[ipy][ipx][0][0]]+dXVec;

      // vertical control points
      #declare dYVec=YCoeff*(PtV[Qds[ipy][ipx][3][0]]-PtV[Qds[ipy-1][ipx][0][0]])/2;
      #declare PtV[Qds[ipy-1][ipx][2][0]]=PtV[Qds[ipy][ipx][0][0]]-dYVec;
      #declare PtV[Qds[ipy  ][ipx][1][0]]=PtV[Qds[ipy][ipx][0][0]]+dYVec;
      // next column
      #declare ipx=ipx+1;
   #end
   // next (horizontal) row
   #declare ipy=ipy+1;
#end

// B: boundary & skeleton control points
// 1: internal points only (NOT corners!)
// 1: horizontal 00->0X, Y0->YX
#declare ipy=0;
#declare ipx=1;
#while (ipx<Nx-1)
   #declare dXVec=XCoeff*(PtV[Qds[ipy][ipx][0][3]]-PtV[Qds[ipy][ipx-1][0][0]])/2;
   #declare PtV[Qds[ipy][ipx-1][0][2]]=PtV[Qds[ipy][ipx][0][0]]-dXVec;
   #declare PtV[Qds[ipy][ipx  ][0][1]]=PtV[Qds[ipy][ipx][0][0]]+dXVec;
   #declare ipx=ipx+1;
#end
#declare ipy=Ny-2;
#declare ipx=1;
#while (ipx<Nx-1)
   #declare dXVec=XCoeff*(PtV[Qds[ipy][ipx][3][3]]-PtV[Qds[ipy][ipx-1][3][0]])/2;
   #declare PtV[Qds[ipy][ipx-1][3][2]]=PtV[Qds[ipy][ipx][3][0]]-dXVec;
   #declare PtV[Qds[ipy][ipx  ][3][1]]=PtV[Qds[ipy][ipx][3][0]]+dXVec;
   #declare ipx=ipx+1;
#end

// 2: verticals (00->Y0, 0X->Yx)
#declare ipy=1;
#declare ipx=0;
#while (ipy<Ny-1)
   #declare dYVec=YCoeff*(PtV[Qds[ipy][ipx][3][0]]-PtV[Qds[ipy-1][ipx][0][0]])/2;
   #declare PtV[Qds[ipy-1][ipx][2][0]]=PtV[Qds[ipy][ipx][0][0]]-dYVec;
   #declare PtV[Qds[ipy  ][ipx][1][0]]=PtV[Qds[ipy][ipx][0][0]]+dYVec;
   #declare ipy=ipy+1;
#end
#declare ipy=1;
#declare ipx=Nx-2;
#while (ipy<Ny-1)
   #declare dYVec=YCoeff*(PtV[Qds[ipy][ipx][3][3]]-PtV[Qds[ipy-1][ipx][0][3]])/2;
   #declare PtV[Qds[ipy-1][ipx][2][3]]=PtV[Qds[ipy][ipx][0][3]]-dYVec;
   #declare PtV[Qds[ipy  ][ipx][1][3]]=PtV[Qds[ipy][ipx][0][3]]+dYVec;
   #declare ipy=ipy+1;
#end

// near boundary & corner points
// 1:corner 0-0=>0-X (Top)
// initial segment
#declare PtV[Qds[0][0][1][0]]=PtV[Qds[0][0][0][0]]+vnormalize(PtV[Qds[0][0][2][0]]-PtV[Qds[0][0][0][0]])
                                                    * vlength(PtV[Qds[1][0][0][0]]-PtV[Qds[0][0][2][0]]);

// final loop
#declare ipp=0;
#while (ipp<Nx-1)
   #declare PtV[Qds[0][ipp][1][3]]=PtV[Qds[0][ipp][0][3]]+vnormalize(PtV[Qds[0][ipp][2][3]]-PtV[Qds[0][ipp][0][3]])
                                                       * vlength(PtV[Qds[1][ipp][0][3]]-PtV[Qds[0][ipp][2][3]]);

   #declare ipp=ipp+1;
#end
// 0-0=>Y-0 (Left)
// initial segment
#declare PtV[Qds[0][0][0][1]]=PtV[Qds[0][0][0][0]]+vnormalize(PtV[Qds[0][0][0][2]]-PtV[Qds[0][0][0][0]])
                                                    * vlength(PtV[Qds[0][1][0][0]]-PtV[Qds[0][0][0][2]]);
// final loop
#declare ipp=0;
#while (ipp<Ny-1)
   #declare PtV[Qds[ipp][0][3][1]]=PtV[Qds[ipp][0][3][0]]+vnormalize(PtV[Qds[ipp][0][3][2]]-PtV[Qds[ipp][0][3][0]])
                                                       * vlength(PtV[Qds[ipp][1][3][0]]-PtV[Qds[ipp][0][3][2]]);
   #declare ipp=ipp+1;
#end


// 2:corner 0-X=>Y-X (Right)
// initial segment..
#declare PtV[Qds[0][Nx-2][0][2]]=PtV[Qds[0][Nx-2][0][3]]+vnormalize(PtV[Qds[0][Nx-2][0][1]]-PtV[Qds[0][Nx-2][0][3]])
                                                          * vlength(PtV[Qds[0][Nx-2][0][0]]-PtV[Qds[0][Nx-2][0][1]]);
// & final loop
#declare ipp=0;
#while (ipp<Ny-1)
#declare PtV[Qds[ipp][Nx-2][3][2]]=PtV[Qds[ipp][Nx-2][3][3]]+vnormalize(PtV[Qds[ipp][Nx-2][3][1]]-PtV[Qds[ipp][Nx-2][3][3]])
                                                          * vlength(PtV[Qds[ipp][Nx-2][3][0]]-PtV[Qds[ipp][Nx-2][3][1]]);
   #declare ipp=ipp+1;
#end

// 4:corner Y-0=>Y-X (Bottom)
// initial segment
#declare PtV[Qds[Ny-2][0][2][0]]=PtV[Qds[Ny-2][0][3][0]]+vnormalize(PtV[Qds[Ny-2][0][1][0]]-PtV[Qds[Ny-2][0][3][0]])
                                                          * vlength(PtV[Qds[Ny-2][0][0][0]]-PtV[Qds[Ny-2][0][1][0]]);
#declare ipp=0;
#while (ipp<Nx-1)
   #declare PtV[Qds[Ny-2][ipp][2][3]]=PtV[Qds[Ny-2][ipp][3][3]]+vnormalize(PtV[Qds[Ny-2][ipp][1][3]]-PtV[Qds[Ny-2][ipp][3][3]])
                                                             * vlength(PtV[Qds[Ny-2][ipp][0][3]]-PtV[Qds[Ny-2][ipp][1][3]]);
   #declare ipp=ipp+1;
#end

// internal control points
#declare ipy=0;
#while (ipy<Ny-1)
   #declare ipx=0;
   #while (ipx<Nx-1)
      // horizontal control points
      #declare HH=PtV[Qds[ipy][ipx][0][1]]-PtV[Qds[ipy][ipx][0][0]];
      #declare VV=PtV[Qds[ipy][ipx][1][0]]-PtV[Qds[ipy][ipx][0][0]];
      #declare PtV[Qds[ipy][ipx][1][1]]=PtV[Qds[ipy][ipx][0][0]]+HH+VV;

      #declare HH=PtV[Qds[ipy][ipx][3][1]]-PtV[Qds[ipy][ipx][3][0]];
      #declare VV=PtV[Qds[ipy][ipx][2][0]]-PtV[Qds[ipy][ipx][3][0]];
      #declare PtV[Qds[ipy][ipx][2][1]]=PtV[Qds[ipy][ipx][3][0]]+HH+VV;

      #declare HH=PtV[Qds[ipy][ipx][3][2]]-PtV[Qds[ipy][ipx][3][3]];
      #declare VV=PtV[Qds[ipy][ipx][2][3]]-PtV[Qds[ipy][ipx][3][3]];
      #declare PtV[Qds[ipy][ipx][2][2]]=PtV[Qds[ipy][ipx][3][3]]+HH+VV;

      #declare HH=PtV[Qds[ipy][ipx][0][2]]-PtV[Qds[ipy][ipx][0][3]];
      #declare VV=PtV[Qds[ipy][ipx][1][3]]-PtV[Qds[ipy][ipx][0][3]];
      #declare PtV[Qds[ipy][ipx][1][2]]=PtV[Qds[ipy][ipx][0][3]]+HH+VV;


      // next column
      #declare ipx=ipx+1;
   #end
   // next (horizontal) row
   #declare ipy=ipy+1;
#end

#end // macro

// Finally draw entire set of patches !!
/*
#declare Yscale=2;
object {
  Round_Cone2_Union(PtV[Qds[1][0][0][0]], .03, PtV[Qds[0][0][0][0]], .03)
  texture{pigment{color rgb <.9,.4,.7>}}
  scale <1,Yscale,1>
}*/

#declare ReversedColorMap=color_map {
           [0.1 color Red]
           [0.3 color Yellow]
           [0.5 color Blue]
           [0.6 color Green]
           [0.8 color Cyan]
           [1.0 color rgbf <1, 0.2, 0.2, 1.0> ]
           }; // color_map

#declare Rainbow_Colors=
    color_map {
      [0.0   color Magenta]
      [0.33  color Yellow]
      [0.67  color Cyan]
      [1.0   color Magenta]
    };

#declare Filter=.65;
#declare Warstwy=yes;
#declare Effects=false;
#declare LevCol=Black;
#declare Cartographic_Colors=
  color_map {
     [0.000 color NewMidnightBlue  filter Filter]
     [0.100 color Blue filter Filter]
     [0.148 color Cyan filter Filter]
     [0.245 color Green filter Filter]
    #if ( Warstwy )
         [0.245 color LevCol ]
        [0.250 color LevCol ]
         [0.250 color Green filter Filter]  //  yellow lemon
    #end

    [0.400 color rgbf< 0.75, 1.0, 0.0, Filter>]     //  yellow lemon

    [0.495 color Yellow  filter Filter]
    #if ( Warstwy )
         [0.495 color LevCol ]
        [0.500 color LevCol ]
         [0.500 color Yellow filter Filter]
    #end

   [0.545 color Gold  filter Filter]
   [0.575 color Orange  filter Filter]

   [0.765 color OrangeRed  filter Filter]
   #if ( Warstwy )
        [0.765 color LevCol ]
         [0.770 color LevCol ]
         [0.770  color OrangeRed filter Filter]
    #end
    [0.875 color Firebrick filter Filter]     //  very dark red
    [0.990 color rgbf< 1.0, 0.0, 0.0, Filter>]
}; //color_map



/******************************************/
#declare Cos2Sin = function(i, j){ 2*cos(1*(i*pi/18))*cos(1*(i*pi/18))-sin(2*(j*pi/18))}
#declare SinCosP2= function(i, j){ 2*sin(j*pi/18)*pow(  cos(i*pi/18),2) }
#declare SinCos3 = function(i, j){ 2*sin(pi/2*i)+cos(pi/3*j)+i*j/400 }

// InsertCartesian_LD(11,.2)
// left-handed(!) PovRay (x,y,z) -> Cartesian coordinate system (X,Y, Z) /right-handed
// change color of axes when needed
//  x - green => X
//  y - blue  => Z
// -z - red   => Y


#declare DeclareLevel = 0;
#declare tmpColour   = rgbf<0.2, 0.3, 0.4, 0.75>; //rgb <0, 0, 1>; // well, I like blue colour...
#declare tmpColour1  = rgbf<0.2, 0.3, 0.4, 0.75>; // ..transparent a little..
#declare tmpColour2  = rgbf<1.5, 1.3, 1.2, 0.73>;

#macro SetRGBFTColor( RR, GG, BB, FF, TTT )
    #declare tmpColour = rgbft<RR, GG, BB, FF, TTT>;
    #declare DeclareLevel = 0;
#end

#macro SetTexture( FullTexture ) // follow it by SetInterior("InteriorDefinition")
    #declare tmpTexture = FullTexture ;
    #declare DeclareLevel = 2;
#end

#macro SetInterior( tInt )       // valid only together with texture defintions
    #declare tmpInterior = tInt;
    #declare DeclareLevel = 2;
#end

SetVScale(3)
SetVShift(1E-3)
// EvalFuncPatchedSurf(SinCos3)
// FunctionSurface(SinCos3, 0, 4*pi, 20, 0, 4*pi, 20) // create PtV[], Pts[], Qds[] i.e. all surface patches: Func, xmin, xmax, Nx; ymin, ymax, Ny



//ScatterDataSurfaceExp("Saddle.dat", 0, 11, 10, 0, 11, 10, 2, 1)  // File/x1,x2,Nx/y1,y2,Ny,power,sigma, w(r0,r1)=exp( - (|r0-r1|/sigma)^power)

// #declare Newton  = function (d) { 1/d      }
// #declare Exponent= function (d) { exp(-d)  }

// #declare Limiter = function (d) { 1/DZero  }

#end // #ifdef
/*********  End of include file ***********/


#declare Newton  = function (d) { 1/d      }
#declare Limiter = function (d) { 1/DZero  }
#declare Exponent= function (d) { exp(-d)  }

#declare Gauss   = function (d) { exp(-pow(d,2))  }

#declare SKrig   = function (d,pw) { 1/(1.0+pow(d,pw)) }  // SKrig(distance, power)
// aliases
#declare SKrig1  = function (d) { 1/(1.0+d) }
#declare SKrig2  = function (d) { 1/(1.0+pow(d,2)) }
#declare SKrig3  = function (d) { SKrig(d,3) }
#declare SKrig4  = function (d) { 1/(1.0+pow(d,4)) }

// The most configurable version
// Potential -> f(x,y) => smoothing potential, 1/r, 1/r^2, .., exp(-a*r), ...
// Input file:
// "Comment X", "Comment Y"
// num_of_data
// x_1, y_1, z_1
// x_2, y_2, z_2
// .............
// x1, x2, Nxx    => x-min, x-max, num. of cardinal (exact value) points
// y1, y2, Nyy    => the same but for y axis
// Smooth   => smoothing potential function
// Singular => what to do if grid point is too close to data dile point
// ex: with potential funcion V(r1, r0):=1/|r1-r0| there singularity at point
//     r1=r0. So we need special function for solving this problem.

// RawDataSurface("Saddle.dat", Exponent, Exponent, 1, 0, 11, 20, 0, 11, 20)



RawDataSurface("SinCosLosPovUp1.dat", SKrig2, SKrig2, .25, 0, 10, 20, 0, 10, 20)
#declare wykres4 = object {
       DrawAllPatchesGeo()
};


 object {wykres4
     translate <0, .0, 0 >
     texture{ pigment {color rgbft<.55,.50,.50, .8,.2>}

     }
 }
// [END] wykres4 ====================



union{
   DrawRNodesGeo(.1)
   texture{ pigment {color VioletRed }}
}

#declare Plate = object {AddXZPlate(.5)   };

object {
  Plate
  translate -3.5*y
}

//plane{y, -0.280
//  texture {pigment {color rgbf<.5,.5,.0,.7>}}
//}



SetCameraTarget(4, -3, -2)
IntelligentEyeT(15, 40, 22)



