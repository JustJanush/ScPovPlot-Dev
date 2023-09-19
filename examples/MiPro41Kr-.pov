/****E* ScPovPlot3D/MiPro41-.pov
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
*  3.3.0.0, tested on PovRay 3.7.
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

#version 3.7;
#include "stdinc.inc"
#include "colors.inc"
#include "textures.inc"
#include "Cameras.inc"
#include "CoordsSys.inc"
#include "TextExt.inc"
#include "ColorMaps.inc"
#include "math.inc"

// +LC:\Users\SilverBear\Dropbox\MiPro2018\Plot3Dv33
#ifndef (Variant)   
   #declare Variant=0; // bare molecule
#end

// very ugly solution :/
#if (clock_on & Variant>0 )
  #declare _TanTau = 0.4+clock*0.2;  ///(frame_number-6)/10;
#end  
#include "Potential.inc"
#include "VectorField.inc"

#version 3.7;
#local _FileName   = "MiPro41-.pov";
#local _FileVerMaj = "3.4.0.0";
#local _FileVerMin = "2018-02-17";
#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

// Below are some command line switches (sometimes) required for succesful render (refer to POVRay documentation):
//all requires: +LC:\Users\SilverBear\Dropbox\MiPro2018\Plot3Dv33
// +TH +Q11  +W1200 +H900  +RP5 +A +AM2 +R2 +FN8  Declare=Variant=14 Declare=Debug=0  Declare=AddOns=0 Declare=radio=0 Declare=Photons=0
// +C +GIMiPro41-.ini +TH +Q11  +W800 +H600  +RP5 +A +AM2 +R2 +FN8  Declare=Variant=14 Declare=Debug=1  Declare=AddOns=0 Declare=radio=0 Declare=Photons=0 
// +UA +Q1 +W400 +H300 +RP5 -A +AM1 +J +THFS +R5 +FN8 Declare=Variant=14 Declare=Method=1 Declare=Photons=1 Declare=radio=0 

// index:
// +KIF0 +KFF3 +SF2 +F3 Declare=Variant=0

#ifndef (Debug)
   #declare Debug=0;
#end

#ifndef (radio)
   #declare radio=0;
#end

#ifndef (Photons)
   #declare Photons=0;
#end            

#ifndef (AddOns)
   #declare AddOns=0;
#end            

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
// #declare Molecule = object{CreateStruct( Graphene_Atoms, Graphene_Bonds, 0.2)}; // PTube, Bonds, .05)};
// #declare Molecule = object{CreateStruct( H2O_Atoms, H2O_Bonds, 0.07)}; // *** PTube, Bonds, .05)};7

// Water ice simple six molecule flat cluster with free a'x'is rotation,
// Water as a "soup" of flat rings and tetrahedral icebergs: "Water: water-an enduring mystery":
// Ball P., Nature. 2008 Mar 20;452(7185):291-2. doi: 10.1038/452291a.,
// URI - https://www.ncbi.nlm.nih.gov/pubmed/18354466
// (H2O)8 - http://www1.lsbu.ac.uk/water/water_equilibria.html
 #declare Hbond = 1.86; // [A] hydrogen bond
 #declare tbnd = H2O_Bonds; // 1
 #declare tarr = H2O_Atoms;
 #declare vp   = (Hbond+HOdist)*< .5,0,-sin(radians(60))>;
 #declare vm   = (Hbond+HOdist)*<-.5,0,-sin(radians(60))>;
 #declare vx   = (Hbond+HOdist)*<1,0,0>;
 #declare xang = 0;  // molecule tilt around temporary a'x'is

 StrRotate(tarr, y, -dAngleHOH) // 1st
 StrRotate(tarr, x, xang) // 1st

 #declare tmpr = H2O_Atoms; // 2nd
   StrRotate(tmpr, y, -dAngleHOH )
   StrRotate(tmpr, x, xang )
   StrRotate(tmpr, y,  60 )
   StrTrans(tmpr, vx)//<Hbond+HOdist, 0, 0>)
 #declare tbnd = BndMerge( tbnd, H2O_Bonds,  dimension_size(tarr,1)-1);   // 4
 #declare tarr = StrMerge( tarr, tmpr);

 #declare tmpr = H2O_Atoms; // 3rd
   StrRotate(tmpr, y, -dAngleHOH )
   StrRotate(tmpr, x, -xang )
   StrRotate(tmpr, y, 120 )
   StrTrans(tmpr, vx+vp)
 #declare tbnd = BndMerge( tbnd, H2O_Bonds, dimension_size(tarr,1)-1);    // 7
 #declare tarr = StrMerge( tarr, tmpr);

 #declare tmpr = H2O_Atoms; // 4th
   StrRotate(tmpr, y, -dAngleHOH )
   StrRotate(tmpr, x, xang )
   StrRotate(tmpr, y, 180  )
   StrTrans(tmpr, vx+vp+vm)
 #declare tbnd = BndMerge( tbnd, H2O_Bonds, dimension_size(tarr,1)-1);    // 10
 #declare tarr = StrMerge( tarr, tmpr);

 #declare tmpr = H2O_Atoms; // 5th
   StrRotate(tmpr, y, -dAngleHOH )
   StrRotate(tmpr, x, -xang )
   StrRotate(tmpr, y, 240  )
   StrTrans(tmpr, vp+vm)
 #declare tbnd = BndMerge( tbnd, H2O_Bonds, dimension_size(tarr,1)-1);    // 13
 #declare tarr = StrMerge( tarr, tmpr);

 #declare tmpr = H2O_Atoms; // 6th
   StrRotate(tmpr, y, -dAngleHOH )
   StrRotate(tmpr, x, -xang )
   StrRotate(tmpr, y, 300  )
   StrTrans(tmpr, vm)
 #declare tbnd = BndMerge( tbnd, H2O_Bonds, dimension_size(tarr,1)-1);   // 16
 #declare tarr = StrMerge( tarr, tmpr);

 #declare HB = array[6][3]{
    { 1,18,3}  //  1
   ,{ 4, 3,3}  //  4
   ,{ 7, 6,3}  //  7
   ,{10, 9,3}  // 10
   ,{13,12,3}  // 13
   ,{16,15,3}  // 16
 };

 #declare tbnd = ArMerge( tbnd, HB ); // Array Merger

// StrTrans(tarr, <-4, 0, 4>)// optional move
 StrRotate(tarr, y, 25 )

 DeclareStructure( tarr )   // required by "ComputeStreamline()" then by "DrawStreamLines()"
 #declare Molecule = object{CreateStruct( tarr, tbnd, 0.07)}; // makes visible object, another defined structure: (PTube, Bonds, .05)};
 #if(Variant<9)
   object{Molecule}
 #end  

// Coulomb only in this work
 #declare VVV_C1  = CreateVFC(tarr)          // *** VVV() - without scaling factor
 #declare VVV_C2  = CreateVFC2(tarr, 1000)   // *** VVV() - WITH scaling factor

//====[ Control box ]==========
#declare dy = .95;  // *** BoxMax modifier, [0.0..>1] designates upper cross section plane position: 0=XY plane, 1=no change >1 increase BoxMax
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

SetAccuracy(.005)//.1) // setting for isosurface part, the smaller the longer computation
//SetAccura(.001)
SetMaxGrad(100)          // setting for isosurface part,

//  available "Variant" values
#declare _JustMolecule  =  0;
// == scalar potential ======
#declare _InterPot      =  1;
#declare _CubicPotInter =  2;
#declare _KrigPotInter  =  3;
//===========================

//===========================
// some interesting textures....
#declare CardinalPoint = texture{pigment {color    Red} finish{ambient .36 reflection metallic phong .3 emission .2}}
#declare ControlPoint  = texture{pigment {color Orange} finish{ambient .36 reflection metallic phong .3 emission .2}}
#declare ControlLine   = texture{pigment {color Silver} finish{ambient .60 reflection metallic phong .3}}

// handy color maps
#declare Basic = color_map {
          [ 0.05 rgbt < .1, .1, .1, .0>]
          [ 0.25 rgbt < 0.5,  0.2, 0, .0>]
          //[ 0.5 rgbt < 0.75,  0.0, 0, .0>]
          [ 0.95 rgbt < 1,  0, 0, .0>]
          };

#declare RGB0 = color_map {
          [ 0.05 rgbt < 1, 1, 1, 1>]
          [ 0.95 rgbt < 1, 1, 1, 1>]
          };

#declare RGB1 = color_map {
          [ 0.05 rgbt < 1, 1, 1, .0>]
          [ 0.95 rgbt < 0, 0, 0, .0>]
          [ 0.95 rgbt < 1, 1, 1, .0>]
          };

#declare RGB2 = color_map {
          [ 0.00 rgbt < .2, .2, .2, .0>]
          [ 0.05 rgbt < .5, .5, .5, .0>]
          [ 0.15 rgbt < 0, 0, 1, .0>]
          [ 0.45 rgbt < 0, .7, 0, .0>]
          [ 0.75 rgbt < 1, .7, 0, .0>]
          [ 0.95 rgbt < 1, 0, 0, .0>]
          };

#declare RGB3 = color_map {
          [ 0.000 rgbt < 1 , 1 , 1 , .0>]
          [ 0.010 rgbt < .4, .4, .4, .0>]
          [ 0.050 rgbt < .4, .4, .6, .0>]
          [ 0.200 rgbt < 0, 0, 1, .0>]
          [ 0.300 rgbt < 0, .5, 1, .0>]
          [ 0.400 rgbt < 0, 1, .50, .0>]
          [ 0.500 rgbt < 0, 1, 0, .0>]
          [ 0.600 rgbt < 1, 1, 0, .0>]
          [ 0.700 rgbt < 1, 1, 0, .0>]
          [ 0.800 rgbt < 1, 0, 0, .0>]
          [ 0.900 rgbt < 1, 0, 0.7, .0>]
          [ 0.990 rgbt < 0.7, 0.0, 0.7, 0.0>]
          [ 0.999 rgbt < 1.0, 0.0, 1.0, 0.0>]
          [ 0.999 rgbt < 1.0, 1.0, 1.0, 0.0>]
          [ 1.000 rgbt < 1.0, 1.0, 1.0, 0.0>]
          };

#declare RGB3a = color_map {
          [ 0.000 rgb < 0.1, 0.1, 0.1>]
          [ 0.025 rgb < 0.0, 0.0, 0.1>]
          [ 0.100 rgb < 0.0, 0.0, 0.2>]
          [ 0.200 rgb < 0.0, 0.0, 1.0>]
          [ 0.300 rgb < 0.0, 0.2, 1.0>]
          [ 0.400 rgb < 0.0, 1.0, 0.2>]
          [ 0.500 rgb < 0.0, 1.0, 0.0>]
          [ 0.600 rgb < 0.2, 1.0, 0.0>]
          [ 0.700 rgb < 0.8, 0.8, 0.0>]
          [ 0.740 rgb < 0.8, 0.2, 0.0>]
          [ 0.850 rgb < 1.0, 0.0, 0.0>]
          [ 0.999 rgb < 0.5, 0.0, 0.7>]
          [ 1.000 rgb < 1.0, 0.0, 1.0>]
          };

#declare RGB4 = color_map {
          [ 0.00 rgbt < 0.0, .0, .5, .0>]
          [ 0.25 rgbt < 0.0, .8  .8, .0>]
          [ 0.50 rgbt < 0.0, .5, .5, .0>]
          [ 0.70 rgbt < 0.8, .8, .0, .0>]
          [ 0.98 rgbt < 1.0, .0, .0, .0>]
};

#declare RGB4a = color_map {
          [ 0.00 rgbt < 0.0, .0,  .1, .0>]
          [ 0.25 rgbt < 0.0, .8   .8, .0>]
          [ 0.50 rgbt < 0.0, .5,  .5, .0>]
          [ 0.70 rgbt < 0.8, .8,  .0, .0>]
          [ 0.88 rgbt < 1.0, .0,  .0, .0>]
          [ 0.95 rgbt < 1.0, .0, 1.0, .0>]
};

#declare RGB4bw = color_map {
          [ 0.00 rgbt < 0.0, 0.0, 0.0, .0>]
          [ 0.10 rgbt < 0.1, 0.1, 0.1, .0>]
          [ 0.20 rgbt < 0.2, 0.2, 0.2, .0>]
          [ 0.30 rgbt < 0.3, 0.3, 0.3, .0>]
          [ 0.40 rgbt < 0.4, 0.4, 0.4, .0>]
          [ 0.50 rgbt < 0.5, 0.5, 0.5, .0>]
          [ 0.60 rgbt < 0.6, 0.6, 0.6, .0>]
          [ 0.70 rgbt < 0.7, 0.7, 0.7, .0>]
          [ 0.80 rgbt < 0.8, 0.8, 0.8, .0>]
          [ 0.90 rgbt < 0.9, 0.9, 0.9, .0>]
          [ 0.95 rgbt < 1.0, 1.0, 1.0, .0>]
};

#declare RGB4bwt = texture_map {
          [ 0.00 pigment{color rgbt < 0.0, 0.0, 0.0, .340>} finish{ Dull emission 0.1 }]
          [ 0.10 pigment{color rgbt < 0.1, 0.1, 0.1, .340>} finish{ Dull emission 0.2 }]
          [ 0.20 pigment{color rgbt < 0.2, 0.2, 0.2, .340>} finish{ Dull emission 0.3 }]
          [ 0.30 pigment{color rgbt < 0.3, 0.3, 0.3, .340>} finish{ Dull emission 0.4 }]
          [ 0.40 pigment{color rgbt < 0.4, 0.4, 0.4, .340>} finish{ Dull emission 0.5 }]
          [ 0.50 pigment{color rgbt < 0.5, 0.5, 0.5, .340>} finish{ Dull emission 0.6 }]
          [ 0.60 pigment{color rgbt < 0.6, 0.6, 0.6, .340>} finish{ Dull emission 0.7 }]
          [ 0.70 pigment{color rgbt < 0.7, 0.7, 0.7, .340>} finish{ Dull emission 0.8 }]
          [ 0.80 pigment{color rgbt < 0.8, 0.8, 0.8, .340>} finish{ Dull emission 0.9 }]
          [ 0.90 pigment{color rgbt < 0.9, 0.9, 0.9, .340>} finish{ Dull emission 1.0 }]
          [ 0.95 pigment{color rgbt < 1.0, 1.0, 1.0, .340>} finish{ Dull emission 1.0 }]
};


#declare RGB4t = texture_map {
          [ 0.00 pigment{color rgbt < 0.0, .0,  .1, .30>} finish{ Dull emission .01 }]
          [ 0.25 pigment{color rgbt < 0.0, .8   .8, .30>} finish{ Dull emission .01 }]
          [ 0.50 pigment{color rgbt < 0.0, .5,  .5, .30>} finish{ Dull emission .051 }]
          [ 0.70 pigment{color rgbt < 0.8, .8,  .0, .30>} finish{ Dull emission .01 }]
          [ 0.88 pigment{color rgbt < 1.0, .0,  .0, .30>} finish{ Dull emission 1.1 }]
          [ 0.95 pigment{color rgbt < 1.0, .0, 1.0, .30>} finish{ Dull emission 1.1 }]
};

#declare RGB5 = color_map {
          [ 0.000 rgbt < 0.0, 0.0, 0.0, 0.0>]
          [ 0.050 rgbt < 0.0, 0.0, 0.5, 0.0>]
          [ 0.250 rgbt < 0.0, 0.7, 0.5, 0.0>]
          [ 0.350 rgbt < 0.0, 0.8, 0.0, 0.0>]
          [ 0.490 rgbt < 0.0, 0.2, 0.0, 0.0>]
          [ 0.499 rgbt < 0.1, 0.1, 0.1, 0.0>]
          [ 0.501 rgbt < 0.1, 0.1, 0.1, 0.0>]
          [ 0.510 rgbt < 0.2, 0.2, 0.0, 0.0>]
          [ 0.610 rgbt < 0.5, 0.5, 0.0, 0.0>]
          [ 0.700 rgbt < 0.8, 0.8, 0.0, 0.0>]
          [ 0.750 rgbt < 0.8, 0.4, 0.0, 0.0>]
          [ 0.900 rgbt < 0.8, 0.2, 0.0, 0.0>]
          [ 1.000 rgbt < 1.0, 0.0, 0.0, 0.0>]
};

#declare RGB6 = color_map {
          [ 0.000 rgbt < 0.0, 0.0, 2.0, 0.0>]
          [ 0.445 rgbt < 0.0, 0.9, 0.9, 0.0>]
          [ 0.499 rgbt < 0.3, 0.3, 0.3, 0.0>]
          [ 0.500 rgbt < 0.1, 0.1, 0.1, 0.0>]
          [ 0.501 rgbt < 0.3, 0.3, 0.3, 0.0>]
          [ 0.555 rgbt < 0.9, 0.9, 0.0, 0.0>]
          [ 1.000 rgbt < 2.0, 0.0, 0.0, 0.0>]
};

          #declare RGBextra = color_map {
                      [ 0.00 rgbt < 0.0, 0.0, 0.0, .0>]
                      [ 0.05 rgbt < 0.0, 0.1, 0.2, .0>]
                      [ 0.25 rgbt < 0.0, 0.3, 0.3, .0>]
                      [ 0.75 rgbt < 0.5, 0.0, 0.0, .0>]
                      [ 0.95 rgbt < 2.0, 0.0, 0.0, .0>]
                      };
          #declare RGBstp = color_map {
                      [ 0.00 rgb 0.0]
                      [ 0.10 rgb 0.0]
                      [ 0.10 rgb 0.1]
                      [ 0.20 rgb 0.1]
                      [ 0.20 rgb 0.2]
                      [ 0.30 rgb 0.2]
                      [ 0.30 rgb 0.3]
                      [ 0.40 rgb 0.3]
                      [ 0.40 rgb 0.4]
                      [ 0.50 rgb 0.4]
                      [ 0.50 rgb 0.5]
                      [ 0.60 rgb 0.5]
                      [ 0.60 rgb 0.6]
                      [ 0.70 rgb 0.6]
                      [ 0.70 rgb 0.7]
                      [ 0.80 rgb 0.7]
                      [ 0.80 rgb 0.8]
                      [ 0.90 rgb 0.8]
                      [ 0.90 rgb 0.9]
                      [ 1.00 rgb 2.0]
                      };

// sets internal variable _RGBMap :
SetRGBMap(RGB4bw)//RGB4a)//RGB3a)
//SetRGBMap(RGB3a)

//=======================
light_source { <  3, 10, -3> 1.00 shadowless}
light_source { < -1,  7,  1> .171 shadowless}
light_source { <  6,  7,  1> .171 shadowless}
light_source { < -1,  7, -3> .171 shadowless}
light_source { <  6,  7, -3> .171 shadowless}

#ifdef(bgr)
  background {rgbt bgr*<1,1,1,0>+<0,0,0,1>} // bgr - shade of gray
#else
  background {rgbt <.0150, .0150, .0150, 1>}
#end
//========[ control box ]===============
// compute node values
// option: read them in   
// required: set all N# even!!

#declare N1  = 30;  //  x
#declare N2  = 30;  //  y
#declare N3  = 10;  //  z

// #declare N1  = 20;  //  x
// #declare N2  = 20;  //  y
// #declare N3  =  8;  //  z

// #declare N1  = 5*frame_number; //60;    //  x
// #declare N2  = 5*frame_number; //60;    //  y
// #declare N3  = 2*frame_number; //16;    //  z
//                
// #declare N1  =           60;    //  x
// #declare N2  =           N1;    //  y
// #declare N3  = ceil(N1/2.5);    //  z
               
// mesh data, computed // real //
#local _deltaX = abs(BoxMax.x - BoxMin.x); // N2
#local _deltaZ = abs(BoxMax.z - BoxMin.z); // N1
#local    _ddx = _deltaX/(N2-1);
#local    _ddz = _deltaZ/(N1-1);
#local      dd = (_ddx+_ddz)/2;

#debug concat("Cell size=", TrimStr(str(dd,8,3)),"[",_Ao_,"]\n")

// change of meaning
#local _cx = (BoxMin.x + BoxMax.x)/2;   // povray coords
#local _cz = (BoxMin.z + BoxMax.z)/2;   // povray coords
//sphere{<_cx, 0, _cz>, .5 texture{pigment{color rgb <1,0,0>}}}
// corrected BoxMin, BoxMax:
#declare BoxMin = <_cx-(N2-1)*dd/2, -(N3-1)*dd+BoxMax.y, _cz-(N1-1)*dd/2 >;
#declare BoxMax = <_cx+(N2-1)*dd/2,            BoxMax.y, _cz+(N1-1)*dd/2 >;

// sphere{BoxMin, .5 texture{pigment{color rgb <1,0,0>}}}
// sphere{BoxMax, .5 texture{pigment{color rgb <0,0,1>}}}

#declare dd3 = pow(dd,3);
#declare rr  = .1*dd;
#declare rc  =  rr/3;
#local alf_  =    25; // +90*clock;
#local the_  =    41; // +20*clock;
#local dst_  =    40; // default camera distance 
#local _T    =   165; // [mV] 23094.0*sin(pi*pow((2*clock-1),5)/3); //+20000*pow(clock,3);
#local DZero = 1E-4;  // singularity barrier 
//============================

// box contaning solely mapped object (molecule) is extended to cover whole scene
// #declare BoxMin =  <BoxMin.x, 2.5*BoxMin.y, BoxMin.z>; // ***
// tune BoxMin/Max using this__^^^ coefficient

#local gMin  = 0;//-3.5;
#local gMin2 = 0;
#local gMax  = 35E3; // [mV/A] gradient range, tuned to THIS example; (1-clock)*5E3+clock*5E4; // 5E7;

//SetRGBMap(RGB4bw)
SetRGBMap(RGB4a)
SetGradientAccuracy(.001) // .0001)

#local VVV  = VVV_C2; // VVV is function pointer. Now it is assigned a job to do, result is in [mV].

#local grV0 = fn_Gradient(VVV);     // *** VVV
#local grV  = function{ clip(log(grV0(x,y,z)-gMin+1)-gMin2, 0, log(gMax-gMin+1))/log(gMax-gMin+1) };
// other formulations
// #local grV = function{ clip(grV0(x,y,z)-gMin,  0, gMax-gMin )/(gMax-gMin ) };
// #local grV = function{ clip(grV0(x,y,z)-gMin2, 0, gMax-gMin2)/(gMax-gMin2) };

#ifndef(SetISO)  
    #declare SetISO=1;
    SetIsoTexture( texture{pigment {
          function{ grV(x,y,z) }
          // alternatives?
          // *** fn_Gradient(function{ clip(log(VVV(x,y,z)-gMin),0,gMax)/(gMax-gMin) })
          // *** function{ clip(log(VVV(x,y,z)), 0, gMax )/gMax})}
          // *** fn_Gradient(function{ clip((VVV(x,y,z)-gMin),0,gMax-gMin)/(gMax-gMin) })
          color_map{_RGBMap}
        }
        finish{ Dull } // *** scDullMirror }
    } )
#end

#declare R1 =  seed(1);                      
#declare R2 =  seed(113);
#declare R3 =  seed(6969);
#declare RR =  seed(9117);

// povray coordinates
#local _dxBox = BoxMax.z-BoxMin.z; #local _dx0Box = -BoxMax.z;
#local _dyBox = BoxMax.x-BoxMin.x; #local _dy0Box =  BoxMin.x;
#local _dzBox = BoxMax.y-BoxMin.y; #local _dz0Box =  BoxMin.y;

#local MatrixThe = array[8]{100, 1000, 5000, 10000, 15000, 30000, 45000, 60000}

//#declare Nmax     = MatrixThe[frame_number-1];   // 15000; //100+pow(frame_number,2)*100;
#declare Nmax     = 45000;   // 15000; //100+pow(frame_number,2)*100;

#declare _Rfactor =   1.8;
#declare Qrand = array[Nmax][4];
#local rbox =  sqrt(pow(_dxBox/2,2)+pow(_dyBox/2,2)+pow(_dzBox/2,2))/3;   
#for(in, 0, Nmax-1)
//   #local xxx =    rand(R1)*_dxBox+_dx0Box;
//   #local yyy =    rand(R2)*_dyBox+_dy0Box;
//   #local zzz = 2*(rand(R3)-0.5)*_dzBox;//+_dz0Box;            
//   
//   #local rrr =  rbox*pow(rand(RR),2); 
//   #local rrr =  rbox*sqrt(RR); 
//   #local rrr =  4+2*(rand(RR)-0.5);//2*rbox*exp(-pow((RR-2)/8,2)); 
   
//   #local rrr = 2*rbox * Rand_Triangle(0,1,.6,RR);
//   #local _ph =  2*pi*rand(R1);
//   #local _th =    0;//pi*(rand(R2)-0.5);
//   #local xxx =   rrr*cos(_th)*cos(_ph)-_MoleculeCenter.z;
//   #local yyy =   rrr*cos(_th)*sin(_ph)+_MoleculeCenter.x;   
//   #local zzz =   0.5*rrr*sin(_th)+_MoleculeCenter.y;                            

   #local rrr = 3*rbox * Rand_Triangle(0,1,.6,RR);
   #local _ph =  2*pi*rand(R1);
   #local _th =    0;//pi*(rand(R2)-0.5);
   #local xxx =   rrr*cos(_th)*cos(_ph)-_MoleculeCenter.z;
   #local yyy =   rrr*cos(_th)*sin(_ph)+_MoleculeCenter.x;   
   #local zzz =   _dyBox*(rand(R3)-0.5)+_MoleculeCenter.y;                            
   
   #declare Qrand[in][0] = VVV(yyy,zzz,-xxx);
   #declare Qrand[in][1] = xxx;
   #declare Qrand[in][2] = yyy;
   #declare Qrand[in][3] = zzz;
   
   
//   sphere{<yyy, -abs(zzz), -xxx>, .1 
//          texture{
//                pigment{
//                  // *** fn_Gradient(function{ clip(log(VVV(x,y,z)-gMin),0,gMax)/(gMax-gMin) })
//                  // *** function{ clip(log(VVV(x,y,z)), 0, gMax )/gMax})}
//                  // *** fn_Gradient(function{ clip((VVV(x,y,z)-gMin),0,gMax-gMin)/(gMax-gMin) })
////                  function{ grV(x,y,z) } color_map{_RGBMap}
//                  color rgb .7
//                }
//          finish{emission 1}
//       }   
//   }
#end             
// safe initial setting
#declare MaxQ=Qrand[0][0];
#declare MinQ=MaxQ;

#macro VolumeGridder(Smooth, Singular, sigma, x1, y1, z1,  Nxx, Nyy, Nzz, _dd)
// In the future - open data file and import all points
//    #fopen DataFile InFileName read
//    #read(DataFile, DescriptX, DescriptY, DescriptZ)
//    #read(DataFile, NumPts)
//    #declare RNodes = array[NumPts];    // float vector: height of nodes AND x, y
//    #declare RNodes[0] = <0.0, 0.0, 0.0>;
//    #declare rix=0;
// 
//    #while(rix < NumPts)
//       #read(DataFile, xx, yy, zz)
//       #declare RNodes[rix] = <xx, yy, VScale*zz+VShift>; // real world
//       #declare rix = rix + 1;
//    #end
//    #fclose DataFile
//==========================//
   
// sanitization of input
   #declare Nx=(Nxx>1?Nxx:2);     // at least 2 nodes
   #declare Ny=(Nyy>1?Nyy:2);     // at least 2 nodes
   #declare Nz=(Nzz>1?Nzz:2);     // at least 2 nodes

// macro makes this array
   #declare QVr = array[Nx][Ny][Nz][5]; // global

   #declare SceneMaxX =  Ny*_dd+y1;  #declare SceneMinX =  y1-_dd;
   #declare SceneMaxY =  Nz*_dd+z1;  #declare SceneMinY =  z1-_dd; 
   #declare SceneMaxZ = -Nx*_dd-x1;  #declare SceneMinZ = -x1+_dd; // little bit strange MaxZ < MinZ!!
   #debug concat(" Scene Min X =", TrimStr(str(SceneMinX,8,2)), "; Scene Max X =", TrimStr(str(SceneMaxX,8,2)), " \n")       
   #debug concat(" Scene Min Y =", TrimStr(str(SceneMinY,8,2)), "; Scene Max Y =", TrimStr(str(SceneMaxY,8,2)), " \n")       
   #debug concat(" Scene Min Z =", TrimStr(str(SceneMinZ,8,2)), "; Scene Max Z =", TrimStr(str(SceneMaxZ,8,2)), " \n")       
    
    #local NxLayers = dimension_size(QVr,1);
    #for(in1,0,dimension_size(QVr,1)-1) 
       #debug 
       #if (NxLayers-in1>1)
         concat(" Layers left: [", str(NxLayers-in1,3,0), "] \n")
       #else
         concat(" Last layer !! \n")
       #end  
       #for(in2,0,dimension_size(QVr,2)-1)
          #for(in3,0,dimension_size(QVr,3)-1)
                // real coordinates!!
                #local xxx = in1*_dd + x1;
                #local yyy = in2*_dd + y1;
                #local zzz = in3*_dd + z1;
 
                // POVRay coords
                #local  wghtQSum = 0.0;
                #local  wghtSum  = 0.0;
                #for(ip,0,dimension_size(Qrand,1)-1)
                   #local _qd = sqrt(pow(xxx-Qrand[ip][1],2)+pow(yyy-Qrand[ip][2],2)+pow(zzz-Qrand[ip][3],2))/sigma;
                   #local _wg = (_qd>DZero?Smooth(_qd):Singular(_qd)); 
                   #local  wghtQSum = _wg*Qrand[ip][0]+wghtQSum;
                   #local  wghtSum  = _wg+wghtSum;
                #end

                #local _tQ = (wghtSum>DZero?wghtQSum/wghtSum:0); // 4 security reasons
                #declare MaxQ = (MaxQ<_tQ?_tQ:MaxQ);
                #declare MinQ = (MinQ>_tQ?_tQ:MinQ);
                #declare QVr[in1][in2][in3][0] = _tQ;
                #declare QVr[in1][in2][in3][1] = xxx;
                #declare QVr[in1][in2][in3][2] = yyy;
                #declare QVr[in1][in2][in3][3] = zzz;
                #declare QVr[in1][in2][in3][4] = 0*mod(in1, 10); // 0-render, !=0 do not render or another action (possibly will be rendered with another resolution)
          #end
       #end
    #end
#end
// now QVr[][] is ready for subsequent calculations

#ifdef (AddOns)
    #if (AddOns) // add switch: "Declare=Dodatki=1" to command line
        // marks center of the "Molecule"
        sphere{<(N2-2)*dd/2, 0*(N3-1)*dd/2, -(N1-1)*dd/2> .5 texture{pigment{color rgb <1,0,0>}} }
    #end
#else
    #warning "Add switch: \"Declare=AddOns=1\" to command line to see private stuff ;) \n"
#end

#if ((clock_on) & (Variant=0))
  #declare Variant=frame_number-1;
#end

#declare Newton  = function (d) { 1/d      }      // named after newtonian potential function
#declare Limiter = function (d) { 1/DZero  }      // this function is called when d<DZero
#declare Exponent= function (d) { exp(-d)  }
#declare Gauss   = function (d) { exp(-pow(d,2))} // resembles gauss PDF distribution function

// simple kriging - more general example, which needs  
// two input values, interpreted as 'distance' and 'power' respectively
#declare SKrig   = function (d, pw) { 1/(1.0+pow(d, pw)) }  // SKrig(distance, power)

// ======[ specialised function ]======
#declare SKrig1  = function (d) { 1/(1.0+d) }
#declare SKrig2  = function (d) { 1/(1.0+pow(d,2)) }
#declare SKrig4  = function (d) { 1/(1.0+pow(d,4)) }
/******/

#switch (Variant)
    #case (_JustMolecule)
       #debug concat(" [", str(_JustMolecule, 1, 0), "] - My Dear, you ordered naked geometry! So, here it is!\n")
       #local _Ts = "(H_2O)_6 molecule";
        // #local alf_  =    25; // +90*clock;
        // #local the_  =    41; // +20*clock;
        // #local dst_  =    40; // 13 - default camera distance 
    #break   
///////////
    #case (_InterPot)
        #debug concat(" [", str(_InterPot, 1, 0), "] - Isosurface colored by gradient, interpolated trilinear version.\n")

        #declare QVr = array[N1][N2][N3][5];
        // QV stores [xR index][yR index][zR index][V,x,y,z]real!!! \
        // real data convert to internal representation: x>-zp, y>xp, z>yp
        // no translation
        // bottom   1  2  3
        // the view" from top:
        // bottom layer
        // zR +--> yR     Q1--Q2
        //    |           |   |
        //    V xR        Q3--Q4
        // upper layer
        // zR +--> yR     Q5--Q6
        //    |           |   |
        //    V xR        Q7--Q8
        //============================//

        // Definition of the grid - usually will be completed outside POVRay
        // then read in by another #macro
        // format of the file:
        // "Data_title"
        // "Data_Comment,
        // "X_Label", "Y_Label", "Z_Label",
        // node-node interval - 'dd', N(rows in file), Nx, Ny, Nz (array dimensions),
        // ix, iy, iz, Q, flag,
        // .
        // .
        // .

        #for(in1,0,dimension_size(QVr,1)-1)
           #for(in2,0,dimension_size(QVr,2)-1)
              #for(in3,0,dimension_size(QVr,3)-1)
                    // real coordinates!!
                    #local xxx = in1*dd - BoxMax.z;
                    #local yyy = in2*dd + BoxMin.x;
                    #local zzz = in3*dd + BoxMin.y;

                    #declare QVr[in1][in2][in3][0] = VVV(yyy, zzz, -xxx); //zzz*(xxx-yyy)/(1+rrV); // (xxx*xxx-yyy*yyy-zzz*zzz)/(.1+rrV); //exp(-(rrV*rrV)/2);
                    #declare QVr[in1][in2][in3][1] = xxx;
                    #declare QVr[in1][in2][in3][2] = yyy;
                    #declare QVr[in1][in2][in3][3] = zzz;
                    #declare QVr[in1][in2][in3][4] = 0*mod(in1, 10); // 0-render, !=0 do not render or another action (possibly will be rendered with another resolution)

              #end
           #end
        #end

        // volume marker - prism edges
        DrawQVBox(QVr)
        // alternatively draw ALL cells (a lot of mess!)
        // DrawAllCells(QVr)
        //SetRGBMap(RGB4bw)
        SetRGBMap(RGB4a)
        #local _Ls   = TrimStr(str(gMax/1000,8,1));
        #local _Ts   = concat(TrimStr(str(gMin/1000,5,1)), _fromTo_, _Ls,"|[V/",_Ao_,"]");
        #local _Ts   = concat("Trilin. Pot, N.= ",TrimStr(str(dimension_size(QVr,1)*dimension_size(QVr,2)*dimension_size(QVr,3), 10, 0)),"; V_{const}|=",TrimStr(str(_T/1000,8,3)), "||[V];  #|-",_nabla_,"V(r)#|",_inSet_," <",TrimStr(str(gMin/1000,8,1)),"|",_fromTo_,"|", TrimStr(str(gMax/1000,8,1)), ">||[V/",_Ao_,"]");//"Isosurface colored by gradient";
        #debug _Ts

#ifndef (SetISO)      
    #declare SetISO=1;
    SetIsoTexture( texture{
        pigment{
          // *** fn_Gradient(function{ clip(log(VVV(x,y,z)-gMin),0,gMax)/(gMax-gMin) })
          // *** function{ clip(log(VVV(x,y,z)), 0, gMax )/gMax})}
          // *** fn_Gradient(function{ clip((VVV(x,y,z)-gMin),0,gMax-gMin)/(gMax-gMin) })
          function{ grV(x,y,z) }
          color_map{_RGBMap}
        }
        finish{ Dull } // *** scDullMirror }
    } )
#end
//        SetIsoTexture( texture{ pigment{color rgbt<.5, .5, .5, 0>}} )
        DrawInterpolSrf( _T, dd)

//        SetIsoTexture( texture{ pigment{color rgbt<.75,0.1,0,0>}} )
//        DrawInterpolSrf( 20000, dd)

//        SetIsoTexture( texture{ pigment{color rgbt<0, 1, .75, .0>}} )
//        DrawInterpolSrf( -20000, dd)

//        SetIsoTexture( texture{ pigment{color rgbt<1,0.1, .5, 0>}} )
//        DrawInterpolSrf( 15000, dd)

//        SetIsoTexture( texture{ pigment{color rgbt<0, 1, 1., .0>}} )
//        DrawInterpolSrf( -15000, dd)

//        SetIsoTexture( texture{ pigment{color rgbt<1,0.1, .5, 0>}} )
//        DrawInterpolSrf( 7500, dd)

//        SetIsoTexture( texture{ pigment{color rgbt<0, 1, 1., .0>}} )
//        DrawInterpolSrf( -7500, dd)

    #break
/////////// 
    #case (_CubicPotInter)
       #debug concat(" [", str(_CubicPotInter, 1, 0), "] - Isosurface of potential textured by field strength.\n")
        
        // #declare QVr = array[N1+2][N2+2][N3+2][5]; // global
        
        // QVr array has additional nodes, required by algorithm for efficiency reasons 
        // QVr stores [xR index][yR index][zR index][V,x,y,z]real!!! 
        // real data convert to internal representation: x=>-zp, y=>xp, z=>yp
        // macro uses 64 nodes
        // ====================
        // "the view" from top:
        // [zR]->"iz layer" intermediate layer, others: iz-1; iz; iz+1; iz+2
        // zR [z]--> yR    +Q(ix-1;iy-1;iz  )-+-Q(ix-1;iy;iz  )-+-Q(ix-1;iy+1;iz  )-+-Q(ix-1;iy+2;iz  )+
        //     |           |                  |                 |                   |                  |
        //     V xR        +Q(ix  ;iy-1;iz  )-+-Q(ix  ;iy;iz  )-+-Q(ix  ;iy+1;iz  )-+-Q(ix  ;iy+2;iz  )+
        //                 |                  |                 |                   |                  |
        //                 +Q(ix+1;iy-1;iz  )-+-Q(ix+1;iy;iz  )-+-Q(ix+1;iy+1;iz  )-+-Q(ix+1;iy+2;iz  )+
        //                 |                  |                 |                   |                  |
        //                 +Q(ix+1;iy-1;iz  )-+-Q(ix+2;iy;iz  )-+-Q(ix+2;iy+1;iz  )-+-Q(ix+2;iy+2;iz  )+
        //=============================
        //^
        //:
        //v
        //=============================
        // [zR]->"iz+2 layer", most top
        // zR [z]--> yR    +Q(ix-1;iy-1;iz+2)-+-Q(ix-1;iy;iz+2)-+-Q(ix-1;iy+1;iz+2)-+-Q(ix-1;iy+2;iz+2)+
        //     |           |                  |                 |                   |                  |
        //     V xR        +Q(ix  ;iy-1;iz+2)-+-Q(ix  ;iy;iz+2)-+-Q(ix  ;iy+1;iz+2)-+-Q(ix  ;iy+2;iz+2)+
        //                 |                  |                 |                   |                  |
        //                 +Q(ix+1;iy-1;iz+2)-+-Q(ix+1;iy;iz+2)-+-Q(ix+1;iy+1;iz+2)-+-Q(ix+1;iy+2;iz+2)+
        //                 |                  |                 |                   |                  |
        //                 +Q(ix+1;iy-1;iz+2)-+-Q(ix+2;iy;iz+2)-+-Q(ix+2;iy+1;iz+2)-+-Q(ix+2;iy+2;iz+2)+
        //============================

        // Definition of the grid - in the future be completed outside POVRay (Prometheus?)
        // then read in by another #macro
        // format of the file:
        // "Data_title"
        // "Data_Comment",
        // "X_Label", "Y_Label", "Z_Label",
        // node-node interval - 'dd', N(rows in file), Nx, Ny, Nz (array dimensions),
        // ix, iy, iz, Q, flag,
        // .
        // .
        // .

        // automatic array size determination - all nodes
        // Limiter // Newton, Exponent, Gauss, SKrig1, SKrig2, SKrig4
        #debug concat("Cell size d =", TrimStr(str(dd,8,3)),"[A]\n")
        #local _ddf = dd*_Rfactor;
        #debug concat("d-factor/d  =", TrimStr(str(_ddf/dd,8,3)),"\n")
// /*
        VolumeGridder(Gauss, Gauss, 
                      _ddf,
//                      dd*(1+2.5*clock), 
                     -BoxMax.z-dd, BoxMin.x-dd, BoxMin.y-dd, 
                      N1+2, N2+2, N3+2, dd)

        // volume marker - prism edges
//        DrawQVBox(QVr)
        // alternatively draw ALL cells (a lot of mess!)
        // DrawAllCells(QVr)
        //SetRGBMap(RGB4bw)
        SetRGBMap(RGB4a)
//        #local gMin = 0;//-3.5;
//        #local gMax = 1.8E6;

//        #local _Ls   = TrimStr(str(gMax/1000,8,1));
//        #local _Ts   = concat(TrimStr(str(gMin/1,5,1)), _fromTo_, _Ls,"|[V/",_Ao_,"] ");
//        #local _Ts   = concat("CR ", _tau_ ,"= ",TrimStr(str(_TanTau,4,2)), "V_0=", TrimStr(str(_T,7,1)), "; N. of cells= ",TrimStr(str(dimension_size(QVr,1)*dimension_size(QVr,2)*dimension_size(QVr,3), 10, 0)),";  #|-",_nabla_,"V(r)#|= ", _Ts );

        #local _Ls   = TrimStr(str(gMax/1000,8,1));
        #local _Ts   = concat(TrimStr(str(gMin/1000,5,1)), _fromTo_, _Ls,"|[V/",_Ao_,"]");
        #local _Ts   = concat("d=",TrimStr(str(_ddf/dd,8,2))," [d]; N.= ",TrimStr(str(dimension_size(QVr,1)*dimension_size(QVr,2)*dimension_size(QVr,3), 10, 0)),"; V_{const}|=",TrimStr(str(_T/1000,8,3)), "||[V];  #|-",_nabla_,"V(r)#|",_inSet_," <",TrimStr(str(gMin/1000,8,1)),"|",_fromTo_,"|", TrimStr(str(gMax/1000,8,1)), ">||[V/",_Ao_,"]");//"Isosurface colored by gradient";
        #debug _Ts

#ifndef (SetISO)      
    #declare SetISO=1;
    SetIsoTexture( texture{
        pigment{
          // *** fn_Gradient(function{ clip(log(VVV(x,y,z)-gMin),0,gMax)/(gMax-gMin) })
          // *** function{ clip(log(VVV(x,y,z)), 0, gMax )/gMax})}
          // *** fn_Gradient(function{ clip((VVV(x,y,z)-gMin),0,gMax-gMin)/(gMax-gMin) })
          function{ grV(x,y,z) }
          color_map{_RGBMap}
        }
        finish{ Dull } // *** scDullMirror }
    } )
#end

//        SetIsoTexture( texture{ pigment{color rgbt<.5, .5, .5, 0>}} )
//#local _dstx=20;
//#if (frame_number=1)
//        DrawCubicInterpolSrf( _T, dd)
//#else
//        Draw01CubicInterpolSrf( _T, dd)
        SetExxLimiter( 0.5 )
        DrawCatRomSrf ( _T, dd)
//#end        
// */
/*
        DrawCubicInterpolSrf(  25000, dd)
        DrawCubicInterpolSrf( -25000, dd)
        DrawCubicInterpolSrf(  15000, dd)
        DrawCubicInterpolSrf( -15000, dd)
        DrawCubicInterpolSrf(   7500, dd)
        DrawCubicInterpolSrf(  -7500, dd)
        DrawCubicInterpolSrf(   2500, dd)
        DrawCubicInterpolSrf(  -2500, dd)
        DrawCubicInterpolSrf(    500, dd)
        DrawCubicInterpolSrf(   -500, dd)
*/
        
//        DrawCubicInterpolSrf( .1, dd)
// //        SetIsoTexture( texture{ pigment{color rgbt<.75,0.1,0,0>}} )
//         DrawInterpolSrf( 20000, dd)
// 
// //        SetIsoTexture( texture{ pigment{color rgbt<0, 1, .75, .0>}} )
//         DrawInterpolSrf( -20000, dd)
// 
// //        SetIsoTexture( texture{ pigment{color rgbt<1,0.1, .5, 0>}} )
//         DrawInterpolSrf( 15000, dd)
// 
// //        SetIsoTexture( texture{ pigment{color rgbt<0, 1, 1., .0>}} )
//         DrawInterpolSrf( -15000, dd)
// 
// //        SetIsoTexture( texture{ pigment{color rgbt<1,0.1, .5, 0>}} )
//         DrawInterpolSrf( 7500, dd)
// 
// //        SetIsoTexture( texture{ pigment{color rgbt<0, 1, 1., .0>}} )
//         DrawCubicInterpolSrf( -7500, dd)
    #break
///////////
    #case (_KrigPotInter)
       #debug concat(" [", str(_KrigPotInter, 1, 0), "] - Isosurface of constant potential interpolated by kriging\ntextured by field strength.\n")
        #declare QVr = array[N1+2][N2+2][N3+2][5]; // global


        
        // QVr array has additional nodes, required by algorithm for efficiency reasons 
        // QVr stores [xR index][yR index][zR index][V,x,y,z]real!!! 
        // real data convert to internal representation: x=>-zp, y=>xp, z=>yp
        // macro uses 64 nodes
        // ====================
        // "the view" from top:
        // [zR]->"iz layer" intermediate layer, others: iz-1; iz; iz+1; iz+2
        // zR [z]--> yR    +Q(ix-1;iy-1;iz  )-+-Q(ix-1;iy;iz  )-+-Q(ix-1;iy+1;iz  )-+-Q(ix-1;iy+2;iz  )+
        //     |           |                  |                 |                   |                  |
        //     V xR        +Q(ix  ;iy-1;iz  )-+-Q(ix  ;iy;iz  )-+-Q(ix  ;iy+1;iz  )-+-Q(ix  ;iy+2;iz  )+
        //                 |                  |                 |                   |                  |
        //                 +Q(ix+1;iy-1;iz  )-+-Q(ix+1;iy;iz  )-+-Q(ix+1;iy+1;iz  )-+-Q(ix+1;iy+2;iz  )+
        //                 |                  |                 |                   |                  |
        //                 +Q(ix+1;iy-1;iz  )-+-Q(ix+2;iy;iz  )-+-Q(ix+2;iy+1;iz  )-+-Q(ix+2;iy+2;iz  )+
        //=============================
        //^
        //:
        //v
        //=============================
        // [zR]->"iz+2 layer", most top
        // zR [z]--> yR    +Q(ix-1;iy-1;iz+2)-+-Q(ix-1;iy;iz+2)-+-Q(ix-1;iy+1;iz+2)-+-Q(ix-1;iy+2;iz+2)+
        //     |           |                  |                 |                   |                  |
        //     V xR        +Q(ix  ;iy-1;iz+2)-+-Q(ix  ;iy;iz+2)-+-Q(ix  ;iy+1;iz+2)-+-Q(ix  ;iy+2;iz+2)+
        //                 |                  |                 |                   |                  |
        //                 +Q(ix+1;iy-1;iz+2)-+-Q(ix+1;iy;iz+2)-+-Q(ix+1;iy+1;iz+2)-+-Q(ix+1;iy+2;iz+2)+
        //                 |                  |                 |                   |                  |
        //                 +Q(ix+1;iy-1;iz+2)-+-Q(ix+2;iy;iz+2)-+-Q(ix+2;iy+1;iz+2)-+-Q(ix+2;iy+2;iz+2)+
        //============================

        // Definition of the grid - in the future be completed outside POVRay (Prometheus?)
        // then read in by another #macro
        // format of the file:
        // "Data_title"
        // "Data_Comment",
        // "X_Label", "Y_Label", "Z_Label",
        // node-node interval - 'dd', N(rows in file), Nx, Ny, Nz (array dimensions),
        // ix, iy, iz, Q, flag,
        // .
        // .
        // .

        // automatic array size determination - all nodes

        
        #for(in1,0,dimension_size(QVr,1)-1) 
           #for(in2,0,dimension_size(QVr,2)-1)
              #for(in3,0,dimension_size(QVr,3)-1)
                    // real coordinates!!
                    #local xxx = (in1-1)*dd - BoxMax.z + _rdx;
                    #local yyy = (in2-1)*dd + BoxMin.x + _rdy;
                    #local zzz = (in3-1)*dd + BoxMin.y + _rdz;

                    #declare QVr[in1][in2][in3][0] = VVV(yyy, zzz, -xxx); //zzz*(xxx-yyy)/(1+rrV); // (xxx*xxx-yyy*yyy-zzz*zzz)/(.1+rrV); //exp(-(rrV*rrV)/2);
                    #declare QVr[in1][in2][in3][1] = xxx;
                    #declare QVr[in1][in2][in3][2] = yyy;
                    #declare QVr[in1][in2][in3][3] = zzz;
                    #declare QVr[in1][in2][in3][4] = 0*mod(in1, 10); // 0-render, !=0 do not render or another action (possibly will be rendered with another resolution)
              #end
           #end
        #end

        // volume marker - prism edges 
        DrawQVBox(QVr)
        // box{BoxMin, BoxMax texture{pigment{color rgbt <0,1,0, .5>}}}
        // alternatively draw ALL cells (VERY slow & a lot of mess!)
        // DrawAllCells(QVr)
        //SetRGBMap(RGB4bw)
        SetRGBMap(RGB4a)
//        #local gMin = 0;//-3.5;
//        #local gMax = 1.8E6;
         #if(clock_on) 
           #local _smth = .025*frame_number;
         #else
           #local _smth = .2;
         #end
        
        #ifndef (SetISO)      
            #declare SetISO=1;
            SetIsoTexture( texture{
                pigment{
                  // *** fn_Gradient(function{ clip(log(VVV(x,y,z)-gMin),0,gMax)/(gMax-gMin) })
                  // *** function{ clip(log(VVV(x,y,z)), 0, gMax )/gMax})}
                  // *** fn_Gradient(function{ clip((VVV(x,y,z)-gMin),0,gMax-gMin)/(gMax-gMin) })
                  function{ grV(x,y,z) }
                  color_map{_RGBMap}
                }
                finish{ Dull } // *** scDullMirror }
            } )
        #end

//        SetIsoTexture( texture{ pigment{color rgbt<.5, .5, .5, 0>}} )
//========================================================== 
        #local _smth = .6*dd;    ///(1.45*clock+0.05)*dd;                                  
//        #local alf_  =  25; // +90*clock;
//        #local the_  =  31; // +20*clock;
//        #local dst_=  45;
//        #local _T  = 165;// 23094.0 *sin(pi*pow((2*clock-1),5)/3); //+20000*pow(clock,3);
//==========================================================
//        #local _Ls   = TrimStr(str(gMax/1000,8,1));
//        #local _Ts   = concat(TrimStr(str(gMin/1,5,1)), _fromTo_, _Ls,"|[V/",_Ao_,"] ");
//        #local _Ts   = concat(" smth=",TrimStr(str(_smth,7,3)),", V_0=", TrimStr(str(_T,7,1)), "; N.= ",TrimStr(str(dimension_size(QVr,1)*dimension_size(QVr,2)*dimension_size(QVr,3), 10, 0)),";  #|-",_nabla_,"V(r)#|= ", _Ts );
//        #debug _Ts

        #local _Ls   = TrimStr(str(gMax/1000,8,1));
        #local _Ts   = concat(TrimStr(str(gMin/1000,5,1)), _fromTo_, _Ls,"|[V/",_Ao_,"]");
        #local _Ts   = concat("KDE, d=",TrimStr(str(_smth,7,2)),"; N.= ",TrimStr(str(dimension_size(QVr,1)*dimension_size(QVr,2)*dimension_size(QVr,3), 10, 0)),"; V_{const}|=",TrimStr(str(_T/1000,8,3)), "||[V];  #|-",_nabla_,"V(r)#|",_inSet_," <",TrimStr(str(gMin/1000,8,1)),"|",_fromTo_,"|", TrimStr(str(gMax/1000,8,1)), ">||[V/",_Ao_,"]");//"Isosurface colored by gradient";
        #debug _Ts
        
        SetAccuracy(.1)
        DrawKrigedSurf2( _T, dd, _smth)
/*
        DrawKrigedSurf(  25000, dd, .1)
        DrawKrigedSurf( -25000, dd, .1)
        DrawKrigedSurf(  15000, dd, .1)
        DrawKrigedSurf( -15000, dd, .1)
        DrawKrigedSurf(   7500, dd, .1)
        DrawKrigedSurf(  -7500, dd, .1)
        DrawKrigedSurf(   2500, dd, .1)
        DrawKrigedSurf(  -2500, dd, .1)
        DrawKrigedSurf(    500, dd, .1)
        DrawKrigedSurf(   -500, dd, .1)
*/
        
//        DrawKrigedSurf( .1, dd)
// //        SetIsoTexture( texture{ pigment{color rgbt<.75,0.1,0,0>}} )
//         DrawKrigedSurf( 20000, dd)
// 
// //        SetIsoTexture( texture{ pigment{color rgbt<0, 1, .75, .0>}} )
//         DrawKrigedSurf( -20000, dd)
// 
// //        SetIsoTexture( texture{ pigment{color rgbt<1,0.1, .5, 0>}} )
//         DrawKrigedSurf( 15000, dd)
// 
// //        SetIsoTexture( texture{ pigment{color rgbt<0, 1, 1., .0>}} )
//         DrawKrigedSurf( -15000, dd)
// 
// //        SetIsoTexture( texture{ pigment{color rgbt<1,0.1, .5, 0>}} )
//         DrawKrigedSurf( 7500, dd)
// 
// //        SetIsoTexture( texture{ pigment{color rgbt<0, 1, 1., .0>}} )
//         DrawKrigedSurf( -7500, dd)

    #break

///////////
    #else
        #warning "No variant selected, use Declare=Variant=# on the command line\n"
        #warning "where '#' stands for 0-8 by now, [*] denotes default value:\n"
        #warning "Pure molecule geometry:\n"
        #warning " [0] - Just molecule for geometry analysis. \n"
        #warning " [1] - Trilinear interpolation of potential,\n"
        #warning " [2] - TriCubic interpolation of potential,\n"
        #warning " [3] - Gauss-KDE interpolation of field strength\n"
//
#end  // switch(Variant

// Camera and keybox credentials
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

//SetCameraTarget(.0, 1.1, -.7) // x_p, y_p, z_p
SetCameraTarget( _MoleculeCenter.x, _MoleculeCenter.y - .50, _MoleculeCenter.z)
SetCameraShift(<0,0,0>)
PrepareCamera( alf_, the_, dst_)
SetCameraAngle(40)
SetPerspective()      
//SetOrthographic(22) 
CameraOn(1)

//===[ Insert legend ]===
Set_TFont(_FArialUni)
//Set_TFont(_FArial)
//Set_TFont(_FTimes )
//SetRGBMap(RGB5)
//InsertBarKeyItem( "V(r)", txMica, .08, .04, .250)

// SetRGBMap(RGB4a)
#if(Variant=0)
  SetRGBMap(RGB0)
#end
#declare txWhite = texture{pigment{color rgbt<1,1,1,.2>}}
#ifdef (_Ts)
    InsertBarKeyFrItemBelow ( TrimStr(_Ts), 
                              txMica,  //txMica //,txWhiteFT //,T_Copper_3C
                              .065, 
                              .05, 
                              .240      
                              ,texture { Polished_Chrome finish { phong 1 }} // end of texture
                              ,.02)
#else
    // InsertBarKeyFrItem ( "Scale", txMica, .08, .04, .265, .002)
#end

#ifdef (_Ts12)
   InsertBarKeyFrItemBelow ( TrimStr(_Ts12), txMica, .07, .05, .220       //txMica
                                 //,txWhiteFT
                                 //,T_Copper_3C
                                 ,texture { Polished_Chrome finish { phong 1 }} // end of texture
                                 ,.007)
#else
    //InsertBarKeyFrItem ( "Scale", txMica, .08, .04, .265, .002)
#end


#ifdef (_Ts14)
   //SetRGBMap(RGB4bw)// reset legend color scale
   SetRGBMap(RGB4a)
   InsertBarKeyFrItemBelow ( TrimStr(_Ts14), txMica, .07, .05, .200       //txMica
                                 //,txWhiteFT
                                 //,T_Copper_3C
                                 ,texture { Polished_Chrome finish { phong 1 }} // end of texture
                                 ,.007)
#else
    //InsertBarKeyFrItem ( "Scale", txMica, .08, .04, .265, .002)
#end

#ifdef (AddOns)
    #if (AddOns) // add switch: "Declare=Dodatki=1" to command line
      sphere{ BoxMin, RHyd texture{ pigment{color 2*Blue} finish{emission 1} }}
      sphere{ BoxMax, RHyd texture{ pigment{color 2*Red } finish{emission 1} }}
      box{ BoxMin, BoxMax
           pigment{color rgbt<.75, .75, 0.75, .3>}
           finish{reflection metallic diffuse .3 ambient .3 phong .3}
      }
    #end
#else
    #warning "Add switch: \"Declare=AddOns=1\" to command line to see my private stuff :P \n"
#end

#if (Debug=3)
    // sphere{  0, 1 texture{pigment{color rgb <1,0,1>}}}
    InsertCartesianArrows_LD(5, .15, .4)
    // vignette
    #local alf_ = 41.0; // azimuth relative to -<Z>_p axis
    #local the_ = 37.0; // elevation relative to Z-X plane
    #local dst_ =  7.5; // distance from observed point
    sphere{  < camera_x, camera_y, camera_z > , .05 texture{pigment{color rgb <1,0,1>}} }
    SetCameraTarget( camera_x, camera_y, camera_z )
    SetCameraShift(<0,0,0>)
    PrepareCamera( alf_, the_, dst_)
    CameraOn(1)
#end

/* 2018-02-06, benchmark
CPU time used: kernel 0.55 seconds, user 2079.80 seconds, total 2080.34 seconds.
Elapsed time 548.99 seconds, CPU vs elapsed time ratio 3.79.
Render averaged 477.51 PPS (126.01 PPS CPU time) over 262144 pixels using 4 thread(s).
*/
