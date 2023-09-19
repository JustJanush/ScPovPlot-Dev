/****E* ScPovPlot3D/4Prometheus.pov
* PURPOSE
*   This module is in introductory state, prepared for MiPro39/2016 conference.
*   It contains macros for representation of vector field in form of set of vectors symbolised by
*   various shapes and color coding systems. Direction and strength of vector at given space point
*   can be visualised in different ways. While direction of the vector can be shown by main axis
*   of some figure, for example cone or cylinder, strength and turn can be represented by length
*   or color or volume or so on. It depends mainly on the goal of visualisation.
*   I think, that representation of vectors in single plane is most informative by now.
*   Besides that  we need superimposition of source objects, as coils, charges, permanent magnet
*   poles or even oceanic bed if one takes into account visualisation of oceanic currents.
*     image:../imgs/VectorField.png
*
*   Fig.[VectorFld]. Visualization of vector field
*     |html <hr width=50% align="left">
* VERSION
*  1.0.0, tested on PovRay 3.7.
* AUTHOR
*  Janusz Opi³a Ph.D.
*   jmo{at}agh.edu.pl, janusz.opila{at}gmail.com
*   Dept. of Applied Informatics,
*   href:https://www.facebook.com/KatedraInformatykiStosowanejWZAGH/
*   href:http://kis.zarz.agh.edu.pl/
*   AGH University of Science & Technology, Cracow, Poland href:http://www.zarz.agh.edu.pl/English/index.asp
*   Maintained by Janusz Opi³a Ph.D.
*   Homepage: http://scpovplot3d.sourceforge.net
* COPYRIGHT
*  GNU GPL v.3 License
*  (c) 2012-now by Janusz Opi³a Ph.D.
*  AGH University of Science and Technology
*
****
  end of RoboDoc comment
****/
// Example of naphtalene definition
//_Bibliography for "Naphthalene":
// The crystalline structure of naphthalene. A quantitative X-ray investigation
// John Monteath Robertson  and William Henry Bragg
// Published:01 November 1933https://doi.org/10.1098/rspa.1933.0197
// URL: https://royalsocietypublishing.org/doi/abs/10.1098/rspa.1933.0197
//
// direct XYZ: http://www.rsc.org/suppdata/cp/c3/c3cp44305a/c3cp44305a_2.pdf
// Electronic Supplementary Material (ESI) for Physical Chemistry Chemical Physics
// This journal is (C)The Owner Societies 2013
//
// ScienceDirect:
// https://www.sciencedirect.com/topics/chemical-engineering/naphthalene  
// Table 3.1. Average geometrical parameters of naphthalene46 and its anions in 2–4 (in A and °)
//
//           C10H80      C10H8•- in 3 (SSIP)37    C10H8•- in 2 (CIP)43	  C10H82- in 4 (CIP)37
// C1–C1'	1.4255(8)	     1.446(4)	               1.439(5)	              1.455(2)
// C1–C2	1.4209(6)	     1.413(3)	               1.406(3)	              1.421(2)
// C2–C3	1.3767(6)	     1.395(4)	               1.383(4)	              1.435(2)
// C3–C3'	1.4203(6)	     1.384(4)	               1.365(4)	              1.363(2)
// d	     < 0.01	         0.02                      0.04	                  0.30
// ?	     179.7	       179.0                     178.3	                166.1
//
//======================================================
// Pirali O., Goubet M., Huet T.R., Georges R., Soulard P., Asselin P., et.al., (2013) 
// "The far infrared spectrum of naphthalene characterized by high resolution 
// synchrotron FTIR spectroscopy and anharmonic DFT calculations", 
// Phys Chem Chem Phys. 2013 Jul 7;15(25):10141-50. doi: 10.1039/c3cp44305a: Table S3., 
// http://www.rsc.org/suppdata/cp/c3/c3cp44305a/c3cp44305a_2.pdf, accessed 2019-02-22,
// "Table S3 Atoms' cartesian coordinates (in Angström)
//  in the principal axis orientation (x,y,z = a,b,c)
//  corresponding to the calculated equilibrium 
//  geometry of naphthalene at the B97-1/ANO-DZP level."                        
//======================================================
// atom       x         y       z
//   C   2.423247  0.705674 0.000000
//   C   1.239335  1.396987 0.000000
//   C   0.000000  0.713185 0.000000
//   C   0.000000 -0.713185 0.000000
//   C   1.239335 -1.396987 0.000000
//   C   2.423247 -0.705674 0.000000
//   H  -1.237376  2.476742 0.000000
//   H   3.361100  1.238312 0.000000
//   H   1.237376  2.476742 0.000000
//   C  -1.239335  1.396987 0.000000
//   C  -1.239335 -1.396987 0.000000
//   H   1.237376 -2.476742 0.000000
//   H   3.361100 -1.238312 0.000000
//   C  -2.423247 -0.705674 0.000000
//   C  -2.423247  0.705674 0.000000
//   H  -1.237376 -2.476742 0.000000
//   H  -3.361100 -1.238312 0.000000
//   H  -3.361100  1.238312 0.000000

#version 3.7;
#include "colors.inc"
#include "textures.inc"
#include "math.inc"
#include "Cameras.inc"
#include "CoordsSys.inc"
#include "TextExt.inc"
#include "ColorMaps.inc"

#ifndef (Variant)   
   #declare Variant=0; // bare molecule
#end

// very ugly solution :/
#if (clock_on & Variant>0 )
  #declare _TanTau = 0.4+clock*0.2;  ///(frame_number-6)/10;
#end  
#include "Structures.inc"
#include "Potential.inc"
// #include "VectorField.inc"

#local _FileName   = "Naphtalene.pov";
#local _FileVerMaj = "1.0.0";
#local _FileVerMin = "2019-02-18";
#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")
#ifndef (Method)
   #declare Method=0;
#end

global_settings{
  ambient_light color rgb <.9,.9,.9> // sets an overall brightness/ambient light level in the scene
  max_trace_level 5                  // sets the maximum ray tracing bounce depth (1 or more) [5]
  assumed_gamma      2.2

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

  #if (Photons)                  // global photon block try to tune to Your needs
    photons {
      spacing 0.02               // specify the density of photons
      media 100
      count 100000               // alternatively use a total number of photons
      jitter 1.0                 // jitter phor photon rays
      max_trace_level 460        // optional separate max_trace_level
      adc_bailout 1/255          // see global adc_bailout
      autostop 0                 // photon autostop option
      radius 10                  // manually specified search radius
      expand_thresholds 0.2, 40  // (---Adaptive Search Radius---)
    }
  #end
}

SetMoleculeZoom(.6) // ***
SetAccuracy(.005)//.1) // setting for isosurface part, the smaller the longer computation
//SetAccura(.001)
SetMaxGrad(100)          // setting for isosurface part,

//Structure
#declare _Nmol = 18;
#declare Napht_1  = array[_Nmol+1][8];      

#declare Napht_1[0][0] = _Nmol;
#declare Napht_1[0][1] = 0.0;
#declare Napht_1[0][2] = 0.0;
#declare Napht_1[0][3] = 0.0;
#declare Napht_1[0][4] = 0.0;
#declare Napht_1[0][5] = 0.0;
#declare Napht_1[0][6] = 0.0;
#declare Napht_1[0][7] = 0.0;
#declare Napht_1[0][7] = 0.0;

#local _H = 1;
#local _C = 6;
// x/y/z real
// C 1
#local e   = <2.423247,  0.705674, 0.000000>;   
#local ind = 1;
#declare Napht_1[ind][0] =       -1.0;  // charge
#declare Napht_1[ind][7] =         _C;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_C][0];  //              
#declare Napht_1[ind][5] = _PT[_C][1];  // 
#declare Napht_1[ind][6] = _PT[_C][2];  //   
#declare Napht_1[ind][7] =         _C;  //

// C  2
#local e = <1.239335,  1.396987, 0.000000>;
#local ind = 2;
#declare Napht_1[ind][0] =       -1.0;  // charge
#declare Napht_1[ind][7] =         _C;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_C][0];  //              
#declare Napht_1[ind][5] = _PT[_C][1];  // 
#declare Napht_1[ind][6] = _PT[_C][2];  //   
#declare Napht_1[ind][7] =         _C;  //
      
// C  3
#local e = < 0.000000,  0.713185, 0.000000>;
#local ind = 3;
#declare Napht_1[ind][0] =        0.0;  // charge
#declare Napht_1[ind][7] =         _C;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_C][0];  //              
#declare Napht_1[ind][5] = _PT[_C][1];  // 
#declare Napht_1[ind][6] = _PT[_C][2];  //   
#declare Napht_1[ind][7] =         _C;  //

// C  4
#local e = < 0.000000, -0.713185, 0.000000>;
#local ind = 4;
#declare Napht_1[ind][0] =        0.0;  // charge
#declare Napht_1[ind][7] =         _C;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_C][0];  //              
#declare Napht_1[ind][5] = _PT[_C][1];  // 
#declare Napht_1[ind][6] = _PT[_C][2];  //   
#declare Napht_1[ind][7] =         _C;  //

// C 5
#local e = < 1.239335, -1.396987, 0.000000>;
#local ind = 5;
#declare Napht_1[ind][0] =       -1.0;  // charge
#declare Napht_1[ind][7] =         _C;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_C][0];  //              
#declare Napht_1[ind][5] = _PT[_C][1];  // 
#declare Napht_1[ind][6] = _PT[_C][2];  //   
#declare Napht_1[ind][7] =         _C;  //

// C 6
#local e = < 2.423247, -0.705674, 0.000000>;
#local ind = 6;
#declare Napht_1[ind][0] =       -1.0;  // charge
#declare Napht_1[ind][7] =         _C;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_C][0];  //              
#declare Napht_1[ind][5] = _PT[_C][1];  // 
#declare Napht_1[ind][6] = _PT[_C][2];  //   
#declare Napht_1[ind][7] =         _C;  //

// H# 7
#local e = <-1.237376,  2.476742, 0.000000>;
#local ind = 7;
#declare Napht_1[ind][0] =          1;  // charge
#declare Napht_1[ind][7] =         _H;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_H][0];  //              
#declare Napht_1[ind][5] = _PT[_H][1];  // 
#declare Napht_1[ind][6] = _PT[_H][2];  //   
#declare Napht_1[ind][7] =         _H;  //

// H 8
#local e = < 3.361100,  1.238312, 0.000000>;
#local ind = ind+1;
#declare Napht_1[ind][0] =          1;  // charge
#declare Napht_1[ind][7] =         _H;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_H][0];  //              
#declare Napht_1[ind][5] = _PT[_H][1];  // 
#declare Napht_1[ind][6] = _PT[_H][2];  //   
#declare Napht_1[ind][7] =         _H;  //

// H 9
#local e = < 1.237376,  2.476742, 0.000000>;
#local ind = ind+1;
#declare Napht_1[ind][0] =          1;  // charge
#declare Napht_1[ind][7] =         _H;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_H][0];  //              
#declare Napht_1[ind][5] = _PT[_H][1];  // 
#declare Napht_1[ind][6] = _PT[_H][2];  //   
#declare Napht_1[ind][7] =         _H;  //

// C 10
#local e = <-1.239335,  1.396987, 0.000000>;
#local ind = ind+1;
#declare Napht_1[ind][0] =       -1.0;  // charge
#declare Napht_1[ind][7] =         _C;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_C][0];  //              
#declare Napht_1[ind][5] = _PT[_C][1];  // 
#declare Napht_1[ind][6] = _PT[_C][2];  //   
#declare Napht_1[ind][7] =         _C;  //

// C 11
#local e = <-1.239335, -1.396987, 0.000000>;
#local ind = ind+1;
#declare Napht_1[ind][0] =       -1.0;  // charge
#declare Napht_1[ind][7] =         _C;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_C][0];  //              
#declare Napht_1[ind][5] = _PT[_C][1];  // 
#declare Napht_1[ind][6] = _PT[_C][2];  //   
#declare Napht_1[ind][7] =         _C;  //

// H 12
#local e = < 1.237376, -2.476742, 0.000000>;
#local ind = ind+1;
#declare Napht_1[ind][0] =          1;  // charge
#declare Napht_1[ind][7] =         _H;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_H][0];  //              
#declare Napht_1[ind][5] = _PT[_H][1];  // 
#declare Napht_1[ind][6] = _PT[_H][2];  //   
#declare Napht_1[ind][7] =         _H;  //

// H 13
#local e = < 3.361100, -1.238312, 0.000000>;
#local ind = ind+1;
#declare Napht_1[ind][0] =          1;  // charge
#declare Napht_1[ind][7] =         _H;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_H][0];  //              
#declare Napht_1[ind][5] = _PT[_H][1];  // 
#declare Napht_1[ind][6] = _PT[_H][2];  //   
#declare Napht_1[ind][7] =         _H;  //

// C 14
#local e = <-2.423247, -0.705674, 0.000000>;   
#local ind = ind+1;
#declare Napht_1[ind][0] =       -1.0;  // charge
#declare Napht_1[ind][7] =         _C;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_C][0];  //              
#declare Napht_1[ind][5] = _PT[_C][1];  // 
#declare Napht_1[ind][6] = _PT[_C][2];  //   
#declare Napht_1[ind][7] =         _C;  //

// C 15
#local e = <-2.423247,  0.705674, 0.000000>;
#local ind = ind+1;
#declare Napht_1[ind][0] =       -1.0;  // charge
#declare Napht_1[ind][7] =         _C;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_C][0];  //              
#declare Napht_1[ind][5] = _PT[_C][1];  // 
#declare Napht_1[ind][6] = _PT[_C][2];  //   
#declare Napht_1[ind][7] =         _C;  //

// H 16
#local e = <-1.237376, -2.476742, 0.000000>;
#local ind = ind+1;
#declare Napht_1[ind][0] =          1;  // charge
#declare Napht_1[ind][7] =         _H;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_H][0];  //              
#declare Napht_1[ind][5] = _PT[_H][1];  // 
#declare Napht_1[ind][6] = _PT[_H][2];  //   
#declare Napht_1[ind][7] =         _H;  //

// H 17
#local e = <-3.361100, -1.238312, 0.000000>;
#local ind = ind+1;
#declare Napht_1[ind][0] =          1;  // charge
#declare Napht_1[ind][7] =         _H;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_H][0];  //              
#declare Napht_1[ind][5] = _PT[_H][1];  // 
#declare Napht_1[ind][6] = _PT[_H][2];  //   
#declare Napht_1[ind][7] =         _H;  //

// H 18
#local e = <-3.361100,  1.238312, 0.000000>;
#local ind = ind+1;
#declare Napht_1[ind][0] =          1;  // charge
#declare Napht_1[ind][7] =         _H;  // element number: 1 - H; 6 - C 
#declare Napht_1[ind][1] =        e.x;  // xR
#declare Napht_1[ind][2] =        e.y;  // yR
#declare Napht_1[ind][3] =        e.z;  // zR
#declare Napht_1[ind][4] = _PT[_H][0];  //              
#declare Napht_1[ind][5] = _PT[_H][1];  // 
#declare Napht_1[ind][6] = _PT[_H][2];  //   
#declare Napht_1[ind][7] =         _H;  //

// C  2.423247  0.705674 0.000000  1
// C  1.239335  1.396987 0.000000  2
// C  0.000000  0.713185 0.000000  3
// C  0.000000 -0.713185 0.000000  4               _     _
// C  1.239335 -1.396987 0.000000  5             /   \ /   \
// C  2.423247 -0.705674 0.000000  6            |     |     |
// H -1.237376  2.476742 0.000000   7            \ _ / \ _ /
// H  3.361100  1.238312 0.000000   8
// H  1.237376  2.476742 0.000000   9
// C -1.239335  1.396987 0.000000 10
// C -1.239335 -1.396987 0.000000 11
// H  1.237376 -2.476742 0.000000  12
// H  3.361100 -1.238312 0.000000  13
// C -2.423247 -0.705674 0.000000 14
// C -2.423247  0.705674 0.000000 15
// H -1.237376 -2.476742 0.000000  16
// H -3.361100 -1.238312 0.000000  17
// H -3.361100  1.238312 0.000000  18

#declare Napht_Bonds = array[20][3] { 
     { 8,  0, 0} // holds number of bonds [1..MaxB]/nothing/nothing
    ,{ 1,  2, 2} // start,end, type CC sp2 
    ,{ 2,  3, 4} // start,end, type CC
    ,{ 3,  4, 4} // start,end, type CC
    ,{ 4,  5, 4} // start,end, type CC
    ,{ 5,  6, 2} // start,end, type CC sp2 
    ,{ 6,  1, 4} // start,end, type CC
    ,{13,  6, 0} // start,end, type CH
    ,{ 8,  1, 0} // start,end, type CH
    ,{ 9,  2, 0} // start,end, type CH
    ,{10,  3, 2} // start,end, type CC sp2
    ,{ 7, 10, 0} // start,end, type CH
    ,{14, 11, 4} // start,end, type CC
    ,{12,  5, 0} // start,end, type CH
    ,{11,  4, 2} // start,end, type CC sp2
    ,{18, 15, 0} // start,end, type CH
    ,{14, 15, 2} // start,end, type CC sp2
    ,{17, 14, 0} // start,end, type CH 
    ,{10, 15, 4} // start,end, type CC 
    ,{16, 11, 0} // start,end, type CH 
}

#local alf_  =    15; // +360*clock; //15
#local the_  =    57; // 178*(clock-0.5);
#local dst_  =    25; // default camera distance 
// #local _T    = sgn(frame_number-div(final_frame,2)-1)*pow(10, abs(frame_number-div(final_frame,2)-1))/10; // 
// #local _T    =    -10000+(frame_number-final_frame)*2000;
// #local _T    =    10000+(frame_number-final_frame-1)*1000;
// #local _T    =  frame_number*0.1; // 
#local _T    =  0.0;

StrRotate(Napht_1, x,  90 )
StrRotate(Napht_1, y,  clock*360 )
#declare Molecule = object{CreateStruct( Napht_1, Napht_Bonds, 0.07)} // Real coordinates
// makes visible object, another defined structure: (PTube, Bonds, .05)};

object{Molecule}
// Coulomb only in this work
 #declare VVV_C1  = CreateVFC(Napht_1)          // *** VVV() - without scaling factor
 #declare VVV_C2  = CreateVFC2(Napht_1, 1000)   // *** VVV() - WITH scaling factor

//====[ Control box ]==========
#declare dy = 0.95;  // *** BoxMax modifier, [0.0..>1] designates upper cross section plane position: 0=XY plane, 1=no change >1 increase BoxMax
#declare BoxMin     = min_extent(Molecule);
#declare BoxMax     = max_extent(Molecule);
#declare _MoleculeCenter = (BoxMin+BoxMax)/2; // fresh BoxMin/Max

//- box{BoxMin, BoxMax texture{pigment{ color rgbt<.3,.3,.3,.3>}} finish{scDullMirror}}        
//- sphere{_MoleculeCenter .5 texture{pigment{ color rgbt<.6,.0,.3,.3>}} finish{scDullMirror}} 
//- required by isosurface part.                                                               
#declare BoxMin = _MoleculeCenter+ 3*(BoxMin-_MoleculeCenter);
//                                 ^----- tune these coeeficients manually!
#declare BoxMin =  <BoxMin.x, 2.5*BoxMin.y, BoxMin.z>; // ***
//                            ^----- tune this coeficient manually!
//------------------------------------------------------------------<<
#declare BoxMax = _MoleculeCenter+ 3*(BoxMax-_MoleculeCenter);
//                                 ^----- tune these coeficients manually!
#declare BoxMax =  <.0+BoxMax.x, (1-dy)*BoxMax.y, BoxMax.z>; // ***
//                                  ^----- ..as well as this one

#if(Debug=4)
    box{BoxMin, BoxMax texture{pigment{ color rgbt<.3,.3,.3,.3>}} finish{scDullMirror}}
    #declare _MoleculeCenter = (BoxMin+BoxMax)/2; // fresh BoxMin/Max
    sphere{_MoleculeCenter 3.5 texture{pigment{ color rgbt<.6,.0,.3,.3>}} finish{scDullMirror}}
#end                               

//============================

#declare RGB4b = color_map {
          [ 0.00 rgbft CHSL2RGB(<260, .60,  .20, 0.0, .1>)] // < 0.0, .0,  .1, .2>]
          [ 0.15 rgbft CHSL2RGB(<180, .80,  .50, 0.0, .1>)] // < 0.0, .7   .5, .2>]
          [ 0.20 rgbft CHSL2RGB(<116, .60,  .50, 0.0, .1>)] // < 0.0, .7,  .0, .2>]
          [ 0.30 rgbft CHSL2RGB(<116, .80,  .50, 0.0, .1>)] // < 0.0, .7,  .0, .2>]
          [ 0.45 rgbft CHSL2RGB(< 60, .80,  .50, 0.0, .1>)] // < 0.7, .7,  .0, .2>]
          [ 0.50 rgbft CHSL2RGB(< 60, .90,  .50, 0.0, .1>)] // < 0.7, .7,  .0, .2>]
          [ 0.60 rgbft CHSL2RGB(< 27, .80,  .50, 0.0, .1>)] // < 0.7, .5,  .0, .2>]
          [ 0.80 rgbft CHSL2RGB(<  0, .80,  .50, 0.0, .1>)] // < 0.9, .1,  .0, .2>]
          [ 0.95 rgbft CHSL2RGB(<340, .80,  .50, 0.0, .1>)] // < 1.0, .0,  .0, .2>]
          [ 1.00 rgbft CHSL2RGB(<300, .80,  .50, 0.0, .1>)] // < 0.8, .0,  .8, .2>]
};

#local gMin  =   0;//-3.5;
#local gMin2 =   0;
#local gMax  = 1E6; //35E6; // [mV/A] gradient range, tuned to THIS example; (1-clock)*5E3+clock*5E4; // 5E7;   

SetGradMin (gMin)
SetGradMin2(gMin2)
SetGradMax (gMax)

//SetRGBMap(RGB4bw)
// SetRGBMap(RGB4b)
// SetGradientAccuracy(.001) // .0001)
// 
// #local VVV  = VVV_C2; // VVV is function pointer. Now it is assigned a job to do, result is in [mV].
// 
// #local grV0 = fn_Gradient(VVV);     // *** VVV
// #local grV  = function{ clip(log(grV0(x,y,z)-gMin+1)-gMin2, 0, log(gMax-gMin+1))/log(gMax-gMin+1) };
// // other formulations
// // #local grV = function{ clip(grV0(x,y,z)-gMin,  0, gMax-gMin )/(gMax-gMin ) };
// // #local grV = function{ clip(grV0(x,y,z)-gMin2, 0, gMax-gMin2)/(gMax-gMin2) };
// 
// #ifndef(SetISO)  
//     #declare SetISO=1;
//     SetIsoTexture( texture{pigment {
//           function{ grV(x,y,z) }
//           // alternatives?
//           // *** fn_Gradient(function{ clip(log(VVV(x,y,z)-gMin),0,gMax)/(gMax-gMin) })
//           // *** function{ clip(log(VVV(x,y,z)), 0, gMax )/gMax})}
//           // *** fn_Gradient(function{ clip((VVV(x,y,z)-gMin),0,gMax-gMin)/(gMax-gMin) })
//           color_map{_RGBMap}
//         }
//         finish{ Dull } // *** scDullMirror }
//     } )
// #end

// #local _Ts = concat("Iso-Potential: V_{const}|=",TrimStr(str(_T/1000,8,3)), "||[V];  #|-",_nabla_,"V(r)#|",_inSet_," <",TrimStr(str(gMin/1000,8,1)),"|",_fromTo_,"|", TrimStr(str(gMax/1000,8,1)), ">||[V/",_Ao_,"]");//"Isosurface colored by gradient";
// #local ISO0 = MakeEquiPlane( VVV, _T, BoxMin, BoxMax ); // *** zero plane
// object{ISO0}


// Camera, lights and keybox credentials
//=======================
#local _r = 5;
light_source { 5*<  _r,   0*_r,  _r> .51 shadowless rotate y*45}
light_source { 5*< -_r,   0*_r,  _r> .51 shadowless rotate y*45}
light_source { 5*< -_r,   0*_r, -_r> .51 shadowless rotate y*45}
light_source { 5*<  _r,   0*_r, -_r> .51 shadowless rotate y*45}

light_source {   <  _r,   2*_r,  _r> .51 } // shadowless}
light_source {   < -_r,   2*_r,  _r> .51 } // shadowless}
light_source {   < -_r,   2*_r, -_r> .51 shadowless}
light_source {   <  _r,   2*_r, -_r> .51 shadowless}

light_source {   <  -1.5, -1.1,  0>  .51 shadowless}
light_source {   <   1.5, -1.1,  0>  .51 shadowless}
light_source {   <     0,  5.0,  0>  .5  } 

light_source {
  <0,0,0>             // light's position (translated below)
  color rgb .20       // light's color
  area_light
  <8, 0, 0> <0, 0, 8> // lights spread out across this distance (x * z)
  4, 4                // total number of lights in grid (4x*4z = 16 lights)
  adaptive 0          // 0,1,2,3...
  jitter              // adds random softening of light
  circular            // make the shape of the light circular
  orient              // orient light
  translate <0, 8, 0>   // <x y z> position of light
}


#ifdef(bgr)
  background {rgbt < bgr,bgr,bgr, 1> } // bgr - shade of gray
#else
  background {rgbt <.0150, .0150, .0150, 1>}
#end
//[01]=======================================================

// vignette
#ifndef(alf_)
 #local alf_ = 81.0;  // azimuth measured from -<Z>_p axis
#end
#ifndef(the_) 
 #local the_ = 57.0;  // elevation relative to Z-X plane
#end
#ifndef(dst_)
 #local dst_ = 40.0;   // common distance from observed point
#end
PrepareCamera( alf_, the_, dst_)
Set_TFont(_FArialUni)
// InsertBarKeyFrItemBelow (TrimStr(_Ts), 
//                           txMica,  //txMica //,txWhiteFT //,T_Copper_3C
//                           //0.8, .00, 200 
//                            0.07, .02, .225
//                           // 0.06, .025, .230 // text, legend bar, estimated distance from bottom
//                           ,texture { Polished_Chrome finish { phong 1 }} // end of texture
//                           ,.02  )

// InsertCartesianArrows_LD(5, .15, .4)

// SetCameraTarget(.0, 1.1, -.7) // x_p, y_p, z_p
SetCameraTarget( _MoleculeCenter.x, _MoleculeCenter.y - .50, _MoleculeCenter.z)
SetCameraShift(<0,0,0>)
SetCameraAngle(25)
SetPerspective()      
//SetOrthographic(25) 
CameraOn(1)

